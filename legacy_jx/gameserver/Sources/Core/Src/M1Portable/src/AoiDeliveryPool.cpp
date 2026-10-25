#include "mmo/AoiDeliveryPool.h"
#include <stdexcept>
#include <utility>

namespace mmo { namespace portable {

AoiDeliveryPool::AoiDeliveryPool(AoiPoolLimits limits) : limits_(limits) {
    if (!limits_.maxSessions || !limits_.maxPendingBytes ||
        !limits_.maxBytesPerTick || !limits_.maxAttemptsPerTick ||
        !limits_.minRetryNs)
        throw std::invalid_argument("AOI pool requires positive admission and tick limits");
}
bool AoiDeliveryPool::bindOwnerToCurrentThread() noexcept {
    if (!bound_) {
        bound_ = true;
        owner_ = std::this_thread::get_id();
    }
    return owner_ == std::this_thread::get_id();
}
AoiPoolStatus AoiDeliveryPool::ownerStatus() const noexcept {
    if (!bound_) return AoiPoolStatus::Unbound;
    return owner_ == std::this_thread::get_id() ?
        AoiPoolStatus::Ok : AoiPoolStatus::WrongThread;
}
void AoiDeliveryPool::syncAccounting(Slot& slot) noexcept {
    const std::size_t actual = slot.delivery->admittedBytes();
    if (actual == slot.accountedBytes) return;
    if (slot.accountedBytes) {
        pendingBytes_ -= slot.accountedBytes;
        --pendingSessions_;
    }
    if (actual) {
        pendingBytes_ += actual;
        ++pendingSessions_;
    }
    slot.accountedBytes = actual;
}
void AoiDeliveryPool::refreshAfterMutation(Slot& slot) noexcept {
    syncAccounting(slot);
    if (!slot.accountedBytes) {
        slot.lastSendAtNs = 0;
        slot.dispatchedTick = 0;
    }
}
AoiPoolStatus AoiDeliveryPool::add(std::uint64_t id, EntityKey viewer,
    std::uint64_t connectionGeneration, std::uint64_t sceneEpoch,
    AoiDeliveryLimits deliveryLimits, std::size_t maxEvents) {
    const auto status = ownerStatus();
    if (status != AoiPoolStatus::Ok) return status;
    if (!id || !viewer.runtimeId || !viewer.generation ||
        !connectionGeneration || !sceneEpoch || !maxEvents ||
        !deliveryLimits.maxFrameBytes || !deliveryLimits.maxAttempts ||
        !deliveryLimits.maxPendingNs) return AoiPoolStatus::Invalid;
    if (sessions_.find(id) != sessions_.end()) return AoiPoolStatus::Duplicate;
    if (sessions_.size() >= limits_.maxSessions) return AoiPoolStatus::SessionLimit;
    Slot slot;
    slot.delivery.reset(new AoiBudgetedSession(
        viewer, connectionGeneration, sceneEpoch, deliveryLimits, maxEvents));
    if (!slot.delivery->bindOwnerToCurrentThread()) return AoiPoolStatus::WrongThread;
    slot.maxPendingNs = deliveryLimits.maxPendingNs;
    sessions_.emplace(id, std::move(slot));
    return AoiPoolStatus::Ok;
}
AoiPoolStatus AoiDeliveryPool::remove(std::uint64_t id) {
    const auto status = ownerStatus();
    if (status != AoiPoolStatus::Ok) return status;
    const auto it = sessions_.find(id);
    if (it == sessions_.end()) return AoiPoolStatus::NotFound;
    pendingBytes_ -= it->second.accountedBytes;
    if (it->second.accountedBytes) --pendingSessions_;
    sessions_.erase(it);
    return AoiPoolStatus::Ok;
}
AoiPoolPrepare AoiDeliveryPool::prepare(std::uint64_t id,
    SpatialAoi& world, std::int32_t x, std::int32_t y,
    std::uint32_t radius, std::uint64_t nowNs) {
    AoiPoolPrepare result;
    result.status = ownerStatus();
    if (result.status != AoiPoolStatus::Ok) return result;
    const auto it = sessions_.find(id);
    if (it == sessions_.end()) { result.status = AoiPoolStatus::NotFound; return result; }
    result.replication = it->second.delivery->prepare(world, x, y, radius, nowNs);
    return result;
}
AoiPoolAdmission AoiDeliveryPool::admit(std::uint64_t id,
    std::uint64_t sequence, std::vector<std::uint8_t> frame,
    std::uint64_t nowNs) {
    AoiPoolAdmission result;
    result.status = ownerStatus();
    if (result.status != AoiPoolStatus::Ok) return result;
    const auto it = sessions_.find(id);
    if (it == sessions_.end()) { result.status = AoiPoolStatus::NotFound; return result; }
    auto& slot = it->second;
    if (!slot.accountedBytes && !frame.empty()) {
        if (frame.size() > limits_.maxBytesPerTick) {
            slot.delivery->invalidateForResync();
            refreshAfterMutation(slot);
            result.status = AoiPoolStatus::FrameExceedsTickLimit;
            return result;
        }
        if (frame.size() > limits_.maxPendingBytes - pendingBytes_) {
            slot.delivery->invalidateForResync();
            refreshAfterMutation(slot);
            result.status = AoiPoolStatus::PendingBytesLimit;
            return result;
        }
    }
    result.delivery = slot.delivery->admit(sequence, std::move(frame), nowNs);
    refreshAfterMutation(slot);
    return result;
}
AoiPoolAck AoiDeliveryPool::acknowledge(
    std::uint64_t id, const AuthenticatedAoiAck& ack) {
    AoiPoolAck result;
    result.status = ownerStatus();
    if (result.status != AoiPoolStatus::Ok) return result;
    const auto it = sessions_.find(id);
    if (it == sessions_.end()) { result.status = AoiPoolStatus::NotFound; return result; }
    result.replication = it->second.delivery->acknowledge(ack);
    refreshAfterMutation(it->second);
    return result;
}
AoiPoolStatus AoiDeliveryPool::invalidateForResync(std::uint64_t id) {
    const auto status = ownerStatus();
    if (status != AoiPoolStatus::Ok) return status;
    const auto it = sessions_.find(id);
    if (it == sessions_.end()) return AoiPoolStatus::NotFound;
    if (!it->second.delivery->invalidateForResync()) return AoiPoolStatus::Invalid;
    refreshAfterMutation(it->second);
    return AoiPoolStatus::Ok;
}
AoiPoolStatus AoiDeliveryPool::advanceScene(std::uint64_t id,
    std::uint64_t epoch) {
    const auto status = ownerStatus();
    if (status != AoiPoolStatus::Ok) return status;
    const auto it = sessions_.find(id);
    if (it == sessions_.end()) return AoiPoolStatus::NotFound;
    if (!it->second.delivery->advanceScene(epoch)) return AoiPoolStatus::Invalid;
    refreshAfterMutation(it->second);
    return AoiPoolStatus::Ok;
}
AoiPoolStatus AoiDeliveryPool::advanceConnection(std::uint64_t id,
    std::uint64_t generation) {
    const auto status = ownerStatus();
    if (status != AoiPoolStatus::Ok) return status;
    const auto it = sessions_.find(id);
    if (it == sessions_.end()) return AoiPoolStatus::NotFound;
    if (!it->second.delivery->advanceConnection(generation)) return AoiPoolStatus::Invalid;
    refreshAfterMutation(it->second);
    return AoiPoolStatus::Ok;
}
AoiPoolStatus AoiDeliveryPool::beginTick(std::uint64_t tick) {
    const auto status = ownerStatus();
    if (status != AoiPoolStatus::Ok) return status;
    if (!tick || tick <= tick_) return AoiPoolStatus::TickOrder;
    tick_ = tick;
    tickBytes_ = 0;
    tickAttempts_ = 0;
    return AoiPoolStatus::Ok;
}
AoiPoolDispatch AoiDeliveryPool::nextSend(std::uint64_t nowNs) {
    AoiPoolDispatch result;
    result.status = ownerStatus();
    if (result.status != AoiPoolStatus::Ok) return result;
    if (!tick_) { result.status = AoiPoolStatus::TickNotStarted; return result; }
    if (haveTime_ && nowNs < lastTimeNs_) {
        result.status = AoiPoolStatus::ClockRollback; return result;
    }
    haveTime_ = true;
    lastTimeNs_ = nowNs;

    // Clean expired reservations even when tick send quotas are exhausted.
    for (auto& entry : sessions_) {
        auto& slot = entry.second;
        if (!slot.accountedBytes) continue;
        const auto admitted = slot.delivery->admittedAtNs();
        if (nowNs < admitted ||
            nowNs - admitted >= slot.maxPendingNs) {
            slot.delivery->authorizeSendAttempt(slot.delivery->admittedSequence(), nowNs);
            refreshAfterMutation(slot);
        }
    }
    if (tickAttempts_ >= limits_.maxAttemptsPerTick ||
        tickBytes_ >= limits_.maxBytesPerTick) {
        result.status = AoiPoolStatus::TickBudgetExhausted;
        return result;
    }
    if (sessions_.empty()) { result.status = AoiPoolStatus::NoEligible; return result; }

    auto it = sessions_.upper_bound(cursor_);
    if (it == sessions_.end()) it = sessions_.begin();
    bool blockedByBytes = false;
    for (std::size_t checked = 0; checked < sessions_.size(); ++checked) {
        const auto id = it->first;
        auto& slot = it->second;
        auto& delivery = *slot.delivery;
        if (slot.accountedBytes && slot.dispatchedTick != tick_ &&
            (delivery.attemptCount() == 0 ||
             (nowNs >= slot.lastSendAtNs &&
              nowNs - slot.lastSendAtNs >= limits_.minRetryNs))) {
            if (slot.accountedBytes > limits_.maxBytesPerTick - tickBytes_) {
                blockedByBytes = true;
            } else {
                const auto sequence = delivery.admittedSequence();
                const auto admitted = delivery.authorizeSendAttempt(sequence, nowNs);
                refreshAfterMutation(slot);
                if (admitted == AoiDeliveryStatus::Accepted) {
                    slot.dispatchedTick = tick_;
                    slot.lastSendAtNs = nowNs;
                    cursor_ = id;
                    ++tickAttempts_;
                    tickBytes_ += slot.accountedBytes;
                    result.status = AoiPoolStatus::Ok;
                    result.sessionId = id;
                    result.sequence = sequence;
                    result.frame = &delivery.admittedFrame();
                    return result;
                }
            }
        }
        ++it;
        if (it == sessions_.end()) it = sessions_.begin();
    }
    result.status = blockedByBytes ?
        AoiPoolStatus::TickBudgetExhausted : AoiPoolStatus::NoEligible;
    return result;
}
}} // namespace mmo::portable
