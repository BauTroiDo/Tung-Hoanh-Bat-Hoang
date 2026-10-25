#pragma once
#include "mmo/AoiDeliveryPool.h"
#include "mmo/WorldRuntime.h"
#include <unordered_set>
#include <atomic>

namespace mmo { namespace portable {
// Supplied ONLY by authenticated server session registry, never decoded from
// client identity fields. A value is a snapshot, revalidated at execution.
struct TrustedWorldContext {
    std::uint64_t sessionId = 0;
    EntityKey viewer;
    std::uint64_t connectionGeneration = 0;
    std::uint64_t sceneEpoch = 0;
};
struct WorldAdapterLimits {
    std::size_t queueCapacity = 0;
    std::size_t commandsPerDrain = 0;
    std::size_t maxSessionIdentities = 0;
    std::size_t maxEntityIdentities = 0;
    std::size_t maxEventsPerSession = 256;
};
struct WorldAdapterMetrics {
    std::uint64_t acceptedAcks = 0, rejectedCommands = 0, resyncs = 0;
};
// No network callbacks or codec. Configure enabled once before publication;
// default OFF. Bind before starting producers. Stop/join ALL producers before
// destruction; closeIngress alone does not join threads. Owner APIs and metrics
// are owner-only. Queue closures capture fixed value data, no external pointers.
class WorldServerIntegrationAdapter final {
public:
    WorldServerIntegrationAdapter(WorldAdapterLimits limits, AoiPoolLimits pool,
        bool enabled = false);
    WorldServerIntegrationAdapter(const WorldServerIntegrationAdapter&) = delete;
    WorldServerIntegrationAdapter& operator=(const WorldServerIntegrationAdapter&) = delete;
    bool bindOwnerToCurrentThread();
    bool attach(const TrustedWorldContext&, AoiDeliveryLimits, std::size_t maxEvents);
    // Same identity only; connection/scene generations cannot go backwards.
    // A viewer/world change needs a NEW trusted session ID.
    bool transition(const TrustedWorldContext&);
    bool detach(const TrustedWorldContext&);
    AoiStatus upsert(const AoiEntity&);
    // Owner-only despawn; SpatialAoi retains a generation tombstone while
    // releasing this adapter's quota for the no-longer-active entity.
    AoiStatus erase(const EntityKey&);
    AoiPoolPrepare prepare(const TrustedWorldContext&, std::int32_t x,
        std::int32_t y, std::uint32_t radius, std::uint64_t nowNs);
    AoiPoolAdmission admit(const TrustedWorldContext&, std::uint64_t sequence,
        std::vector<std::uint8_t> frame, std::uint64_t nowNs);
    // Thread-safe ingress. Accepted means queued, NOT an accepted ACK.
    WorldCommandQueue::PostStatus postAck(TrustedWorldContext, std::uint64_t sequence);
    WorldCommandQueue::PostStatus postSendFailure(TrustedWorldContext, std::uint64_t sequence);
    void closeIngress();
    WorldCommandQueue::DrainResult drain();
    AoiPoolStatus beginTick(std::uint64_t tick);
    // Borrowed frame contract identical to AoiDeliveryPool. Copy synchronously
    // into a separately bounded transport queue; on uncertainty post failure.
    AoiPoolDispatch nextSend(std::uint64_t nowNs);
    WorldAdapterMetrics metrics() const;
private:
    struct Session { TrustedWorldContext context; bool active = true; std::uint64_t pending = 0; };
    bool owner() const;
    Session* find(const TrustedWorldContext&, bool allowRecovery = false);
    void execute(TrustedWorldContext, std::uint64_t sequence, bool failure);
    const bool enabled_;
    const WorldAdapterLimits limits_;
    WorldCommandQueue queue_;
    SpatialAoi world_;
    AoiDeliveryPool pool_;
    std::map<std::uint64_t, Session> sessions_;
    std::unordered_set<EntityKey, EntityKeyHash> entityIdentities_;
    WorldAdapterMetrics metrics_;
    std::atomic<bool> overflowRecovery_{false};
    bool bound_ = false;
    std::thread::id owner_;
};
}}
