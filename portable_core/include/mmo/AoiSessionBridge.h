#pragma once
// Test-harness bridge between authenticated transport state and AOI replication.
// There is NO network authentication here: caller must supply trusted, server-side
// connection/scene generations; NEVER forward IDs taken from a client packet.
#include "mmo/AoiReplication.h"
#include <cstddef>
#include <cstdint>

namespace mmo { namespace portable {

struct AuthenticatedAoiAck {
    EntityKey viewer;
    std::uint64_t connectionGeneration = 0;
    std::uint64_t sceneEpoch = 0;
    std::uint64_t sequence = 0;
};

// Owned by the authoritative world thread. A new connection or scene MUST
// invalidate unacknowledged deltas before newer work can be prepared.
// Generations monotonically increase and may not wrap or be reused.
class AoiSessionBridge final {
public:
    AoiSessionBridge(EntityKey viewer, std::uint64_t connectionGeneration,
                     std::uint64_t sceneEpoch, std::size_t maxEvents = 256);
    bool bindOwnerToCurrentThread() noexcept;

    ReplicationPrepareResult prepare(SpatialAoi& world, std::int32_t x,
        std::int32_t y, std::uint32_t radius, std::uint64_t nowNs);
    ReplicationPrepareResult retry() const;
    ReplicationStatus acknowledge(const AuthenticatedAoiAck& ack);

    // Both changes reset known-view and discard in-flight ACK expectations.
    // Fail closed on zero, reused, lower, or overflowing generation values.
    bool advanceScene(std::uint64_t newSceneEpoch);
    bool advanceConnection(std::uint64_t newConnectionGeneration);

    bool timedOut(std::uint64_t nowNs, std::uint64_t timeoutNs) const noexcept;
    std::uint64_t connectionGeneration() const noexcept { return connectionGeneration_; }
    std::uint64_t sceneEpoch() const noexcept { return sceneEpoch_; }
    std::size_t committedVisibleCount() const noexcept;
    std::uint64_t lastAcknowledgedSequence() const noexcept;
    bool hasInFlight() const noexcept;
private:
    AoiReplicationSession replication_;
    std::uint64_t connectionGeneration_;
    std::uint64_t sceneEpoch_;
};

}} // namespace mmo::portable
