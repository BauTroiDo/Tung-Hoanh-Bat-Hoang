#pragma once
// Portable world-owner AOI admission and fair send scheduler.
// This is NOT a network implementation; handle IDs and ACKs must be resolved
// from authenticated server state. Never trust session IDs supplied by clients.
#include "mmo/AoiDeliveryBudget.h"
#include <cstddef>
#include <cstdint>
#include <map>
#include <memory>
#include <thread>
#include <vector>

namespace mmo { namespace portable {

struct AoiPoolLimits {
    std::size_t maxSessions = 0;
    std::size_t maxPendingBytes = 0;
    std::size_t maxBytesPerTick = 0;
    std::size_t maxAttemptsPerTick = 0;
    std::uint64_t minRetryNs = 0;
};

enum class AoiPoolStatus {
    Ok, Unbound, WrongThread, Invalid, Duplicate, NotFound,
    SessionLimit, PendingBytesLimit, FrameExceedsTickLimit,
    NoEligible, TickNotStarted, TickOrder, ClockRollback,
    TickBudgetExhausted
};

struct AoiPoolPrepare {
    AoiPoolStatus status = AoiPoolStatus::Invalid;
    ReplicationPrepareResult replication;
};
struct AoiPoolAdmission {
    AoiPoolStatus status = AoiPoolStatus::Invalid;
    AoiDeliveryStatus delivery = AoiDeliveryStatus::NotPending;
};
struct AoiPoolAck {
    AoiPoolStatus status = AoiPoolStatus::Invalid;
    ReplicationStatus replication = ReplicationStatus::NotPending;
};
struct AoiPoolDispatch {
    AoiPoolStatus status = AoiPoolStatus::NoEligible;
    std::uint64_t sessionId = 0;
    std::uint64_t sequence = 0;
    // Borrowed: valid only until the next state mutation/removal of this
    // session. Send synchronously on the owner thread; never retain pointer.
    const std::vector<std::uint8_t>* frame = nullptr;
};

// Idempotent cleanup of accounted bytes occurs on ACK, expiry, resync,
// scene change, connection change and removal.
class AoiDeliveryPool final {
public:
    explicit AoiDeliveryPool(AoiPoolLimits limits);
    AoiDeliveryPool(const AoiDeliveryPool&) = delete;
    AoiDeliveryPool& operator=(const AoiDeliveryPool&) = delete;
    bool bindOwnerToCurrentThread() noexcept;

    AoiPoolStatus add(std::uint64_t sessionId, EntityKey viewer,
        std::uint64_t connectionGeneration, std::uint64_t sceneEpoch,
        AoiDeliveryLimits deliveryLimits, std::size_t maxEvents = 256);
    AoiPoolStatus remove(std::uint64_t sessionId);
    AoiPoolPrepare prepare(std::uint64_t sessionId, SpatialAoi& world,
        std::int32_t x, std::int32_t y, std::uint32_t radius,
        std::uint64_t nowNs);
    AoiPoolAdmission admit(std::uint64_t sessionId, std::uint64_t sequence,
        std::vector<std::uint8_t> frame, std::uint64_t nowNs);
    AoiPoolAck acknowledge(std::uint64_t sessionId,
                           const AuthenticatedAoiAck& ack);
    AoiPoolStatus invalidateForResync(std::uint64_t sessionId);
    AoiPoolStatus advanceScene(std::uint64_t sessionId,
                               std::uint64_t epoch);
    AoiPoolStatus advanceConnection(std::uint64_t sessionId,
                                    std::uint64_t generation);

    // Begin exactly one strictly increasing world tick; resetting per-tick
    // quotas never discards pending frames.
    AoiPoolStatus beginTick(std::uint64_t tick);
    // Round robin by numeric trusted session ID. At most one attempt/session/tick
    // and maxAttempts/bytes aggregate; retry pacing uses monotonic nowNs.
    // Caller MUST NOT treat authorization as successful network delivery.
    AoiPoolDispatch nextSend(std::uint64_t nowNs);

    std::size_t sessionCount() const noexcept { return sessions_.size(); }
    std::size_t pendingBytes() const noexcept { return pendingBytes_; }
    std::size_t pendingSessions() const noexcept { return pendingSessions_; }
    std::size_t tickAttempts() const noexcept { return tickAttempts_; }
    std::size_t tickBytes() const noexcept { return tickBytes_; }
private:
    struct Slot {
        std::unique_ptr<AoiBudgetedSession> delivery;
        std::uint64_t lastSendAtNs = 0;
        std::uint64_t dispatchedTick = 0;
        std::size_t accountedBytes = 0;
        std::uint64_t maxPendingNs = 0;
    };
    AoiPoolStatus ownerStatus() const noexcept;
    void syncAccounting(Slot& slot) noexcept;
    void refreshAfterMutation(Slot& slot) noexcept;
    const AoiPoolLimits limits_;
    std::map<std::uint64_t, Slot> sessions_;
    std::size_t pendingBytes_ = 0;
    std::size_t pendingSessions_ = 0;
    std::size_t tickBytes_ = 0;
    std::size_t tickAttempts_ = 0;
    std::uint64_t tick_ = 0;
    std::uint64_t cursor_ = 0;
    std::uint64_t lastTimeNs_ = 0;
    bool haveTime_ = false;
    bool bound_ = false;
    std::thread::id owner_;
};

}} // namespace mmo::portable
