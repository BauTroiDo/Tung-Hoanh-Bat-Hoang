#pragma once
// Portable, stop-and-wait AOI replication session. This class does not
// serialize or send packets. Only the trusted world thread may mutate it.
// Network transport must provide trusted ordered delivery confirmation.
// For uncertain send/disconnect/timeout call invalidateForResync().
#include "mmo/SpatialAoi.h"
#include <cstddef>
#include <cstdint>
#include <memory>
#include <thread>
#include <vector>

namespace mmo { namespace portable {

enum class ReplicationStatus {
    Prepared, NoChanges, InFlight, Invalid, WrongThread, Unbound,
    PlannerRejected, NotPending, WrongSequence, SequenceExhausted,
    WrongViewer, WrongConnection, WrongEpoch
};

struct ReplicationBatch {
    std::uint64_t sequence = 0;
    std::uint64_t sceneEpoch = 0;
    std::vector<AoiDelta> events;
};

struct ReplicationPrepareResult {
    ReplicationStatus status = ReplicationStatus::Invalid;
    AoiStatus plannerStatus = AoiStatus::Ok;
    ReplicationBatch batch;
};

// Stop-and-wait: at most ONE prepared batch/session. The committed
// AoiObserverView changes only after acknowledge() confirms delivery.
class AoiReplicationSession final {
public:
    explicit AoiReplicationSession(EntityKey viewer, std::size_t maxEvents = 256);
    AoiReplicationSession(const AoiReplicationSession&) = delete;
    AoiReplicationSession& operator=(const AoiReplicationSession&) = delete;

    bool bindOwnerToCurrentThread() noexcept;
    ReplicationStatus accessStatus() const noexcept { return ownerStatus(); }
    const EntityKey& viewer() const noexcept { return viewer_; }
    bool hasInFlight() const noexcept { return static_cast<bool>(staged_); }
    std::size_t committedVisibleCount() const noexcept;
    std::uint64_t committedEpoch() const noexcept;
    std::uint64_t lastAcknowledgedSequence() const noexcept { return acknowledged_; }

    // Call only on bound world thread. No pending transmission is allowed.
    // nowNs must come from the same monotonic clock as timedOut().
    ReplicationPrepareResult prepare(SpatialAoi& world,
        std::int32_t centerX, std::int32_t centerY, std::uint32_t radius,
        std::uint64_t sceneEpoch, std::uint64_t nowNs);
    // Re-send identical bytes generated from this batch. Caller must ensure
    // reliable transport sequence ordering and authenticated acknowledgement.
    ReplicationPrepareResult retry() const;
    ReplicationStatus acknowledge(std::uint64_t sequence);
    // False for wrong owner/unbound. On success forcibly resets committed
    // scene so the next successful prepare starts with a full Reset.
    bool invalidateForResync();
    // True only if a pending batch is older than timeoutNs.
    bool timedOut(std::uint64_t nowNs, std::uint64_t timeoutNs) const noexcept;
private:
    ReplicationStatus ownerStatus() const noexcept;
    const EntityKey viewer_;
    const std::size_t maxEvents_;
    bool ownerBound_ = false;
    std::thread::id owner_;
    std::uint64_t nextSequence_ = 1;
    std::uint64_t acknowledged_ = 0;
    std::uint64_t preparedAtNs_ = 0;
    std::unique_ptr<AoiObserverView> committed_;
    std::unique_ptr<AoiObserverView> staged_;
    ReplicationBatch pending_;
};

}} // namespace mmo::portable
