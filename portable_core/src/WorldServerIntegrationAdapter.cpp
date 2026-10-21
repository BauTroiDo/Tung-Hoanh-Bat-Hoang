#include "mmo/WorldServerIntegrationAdapter.h"
#include <stdexcept>
#include <utility>
namespace mmo { namespace portable {
WorldServerIntegrationAdapter::WorldServerIntegrationAdapter(
    WorldAdapterLimits limits, AoiPoolLimits pool, bool enabled)
    : enabled_(enabled), limits_(limits), queue_(limits.queueCapacity), pool_(pool) {
    if (!limits.commandsPerDrain || !limits.maxSessionIdentities || !limits.maxEntityIdentities ||
        !limits.maxEventsPerSession)
        throw std::invalid_argument("positive adapter quotas required");
}
bool WorldServerIntegrationAdapter::bindOwnerToCurrentThread() {
    if (bound_) return owner();
    if (!queue_.bindOwnerToCurrentThread() || !world_.bindOwnerToCurrentThread() ||
        !pool_.bindOwnerToCurrentThread()) return false;
    owner_ = std::this_thread::get_id(); bound_ = true; return true;
}
bool WorldServerIntegrationAdapter::owner() const {
    return bound_ && owner_ == std::this_thread::get_id();
}
WorldServerIntegrationAdapter::Session* WorldServerIntegrationAdapter::find(
    const TrustedWorldContext& c, bool allowRecovery) {
    if (!enabled_ || !owner() || (!allowRecovery && overflowRecovery_.load())) return nullptr;
    const auto it = sessions_.find(c.sessionId);
    if (it == sessions_.end() || !it->second.active) return nullptr;
    const auto& t = it->second.context;
    return t.viewer == c.viewer && t.connectionGeneration == c.connectionGeneration &&
        t.sceneEpoch == c.sceneEpoch ? &it->second : nullptr;
}
bool WorldServerIntegrationAdapter::attach(const TrustedWorldContext& c,
    AoiDeliveryLimits limits, std::size_t events) {
    if (!enabled_ || !owner() || !events || events > limits_.maxEventsPerSession ||
        sessions_.count(c.sessionId) ||
        sessions_.size() >= limits_.maxSessionIdentities) return false;
    // Allocate registry first: allocation failure cannot leave an untracked pool session.
    auto inserted = sessions_.emplace(c.sessionId, Session{c,true,0});
    try {
        if (pool_.add(c.sessionId,c.viewer,c.connectionGeneration,c.sceneEpoch,limits,events)
            == AoiPoolStatus::Ok) return true;
    } catch (...) { sessions_.erase(inserted.first); throw; }
    sessions_.erase(inserted.first); return false;
}
bool WorldServerIntegrationAdapter::transition(const TrustedWorldContext& c) {
    if (!enabled_ || !owner()) return false;
    auto it = sessions_.find(c.sessionId);
    if (it == sessions_.end() || !it->second.active) return false;
    auto& s = it->second;
    const auto old = s.context;
    if (c.viewer != old.viewer || c.connectionGeneration < old.connectionGeneration ||
        c.sceneEpoch < old.sceneEpoch ||
        (c.connectionGeneration == old.connectionGeneration && c.sceneEpoch == old.sceneEpoch))
        return false;
    if (c.connectionGeneration != old.connectionGeneration &&
        pool_.advanceConnection(c.sessionId,c.connectionGeneration) != AoiPoolStatus::Ok) return false;
    if (c.sceneEpoch != old.sceneEpoch &&
        pool_.advanceScene(c.sessionId,c.sceneEpoch) != AoiPoolStatus::Ok) return false;
    s.context = c; s.pending = 0; ++metrics_.resyncs; return true;
}
bool WorldServerIntegrationAdapter::detach(const TrustedWorldContext& c) {
    // Teardown releases resources even while recovery blocks data/ACK commits.
    // Thread ownership and every trusted identity field are still checked.
    auto* s = find(c,true); if (!s) return false;
    if (pool_.remove(c.sessionId) != AoiPoolStatus::Ok) return false;
    s->active = false; s->pending = 0; return true;
}
AoiStatus WorldServerIntegrationAdapter::upsert(const AoiEntity& entity) {
    if (!owner()) return bound_ ? AoiStatus::WrongThread : AoiStatus::Unbound;
    if (!enabled_) return AoiStatus::Invalid;
    if (!entityIdentities_.count(entity.key) &&
        entityIdentities_.size() >= limits_.maxEntityIdentities) return AoiStatus::Invalid;
    // Reserve identity before world mutation, also bounding generation tombstones.
    const auto inserted = entityIdentities_.insert(entity.key);
    const auto result = world_.upsert(entity);
    if (result != AoiStatus::Ok && inserted.second) entityIdentities_.erase(inserted.first);
    return result;
}
AoiPoolPrepare WorldServerIntegrationAdapter::prepare(const TrustedWorldContext& c,
    std::int32_t x, std::int32_t y, std::uint32_t radius, std::uint64_t now) {
    auto* s = find(c); if (!s) return {};
    auto result = pool_.prepare(c.sessionId,world_,x,y,radius,now);
    if (result.replication.status == ReplicationStatus::Prepared) s->pending = 0;
    return result;
}
AoiPoolAdmission WorldServerIntegrationAdapter::admit(const TrustedWorldContext& c,
    std::uint64_t sequence, std::vector<std::uint8_t> frame, std::uint64_t now) {
    auto* s = find(c); if (!s) return {};
    auto result = pool_.admit(c.sessionId,sequence,std::move(frame),now);
    if (result.status == AoiPoolStatus::Ok && result.delivery == AoiDeliveryStatus::Accepted)
        s->pending = sequence;
    return result;
}
WorldCommandQueue::PostStatus WorldServerIntegrationAdapter::postAck(
    TrustedWorldContext c, std::uint64_t sequence) {
    if (!enabled_ || !c.sessionId || !sequence) return WorldCommandQueue::PostStatus::Invalid;
    return queue_.tryPost([this,c,sequence] { execute(c,sequence,false); });
}
WorldCommandQueue::PostStatus WorldServerIntegrationAdapter::postSendFailure(
    TrustedWorldContext c, std::uint64_t sequence) {
    if (!enabled_ || !c.sessionId || !sequence) return WorldCommandQueue::PostStatus::Invalid;
    const auto result = queue_.tryPost([this,c,sequence] { execute(c,sequence,true); });
    // Losing an uncertain-send notification must never allow an ACK commit.
    // One fixed-size latch forces all active sessions to resync on owner drain.
    if (result != WorldCommandQueue::PostStatus::Accepted) overflowRecovery_.store(true);
    return result;
}
void WorldServerIntegrationAdapter::execute(TrustedWorldContext c,
    std::uint64_t sequence, bool failure) {
    auto* s = find(c);
    if (!s || s->pending != sequence) { ++metrics_.rejectedCommands; return; }
    if (failure) {
        pool_.invalidateForResync(c.sessionId); s->pending = 0; ++metrics_.resyncs;
    } else {
        const auto result = pool_.acknowledge(c.sessionId,
            {c.viewer,c.connectionGeneration,c.sceneEpoch,sequence});
        if (result.status == AoiPoolStatus::Ok && result.replication == ReplicationStatus::Prepared) {
            s->pending = 0; ++metrics_.acceptedAcks;
        } else ++metrics_.rejectedCommands;
    }
}
void WorldServerIntegrationAdapter::closeIngress() { queue_.close(); }
WorldCommandQueue::DrainResult WorldServerIntegrationAdapter::drain() {
    if (owner() && enabled_ && overflowRecovery_.exchange(false)) {
        for (auto& entry : sessions_) {
            if (!entry.second.active) continue;
            pool_.invalidateForResync(entry.first);
            entry.second.pending = 0;
            ++metrics_.resyncs;
        }
    }
    return queue_.drain(limits_.commandsPerDrain);
}
AoiPoolStatus WorldServerIntegrationAdapter::beginTick(std::uint64_t tick) {
    if (!enabled_) return AoiPoolStatus::Invalid;
    return pool_.beginTick(tick);
}
AoiPoolDispatch WorldServerIntegrationAdapter::nextSend(std::uint64_t now) {
    if (!enabled_ || overflowRecovery_.load()) return {AoiPoolStatus::Invalid,0,0,nullptr};
    return pool_.nextSend(now);
}
WorldAdapterMetrics WorldServerIntegrationAdapter::metrics() const {
    return owner() ? metrics_ : WorldAdapterMetrics{};
}
}}
