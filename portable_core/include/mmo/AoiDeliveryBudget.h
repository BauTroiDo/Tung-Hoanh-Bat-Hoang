#pragma once
// Bounded, owner-thread-only AOI send admission. Caller supplies EXACT encoded
// frame bytes from an authenticated transport codec (not event-size guesses).
// This library does not authenticate a socket or send any bytes.
#include "mmo/AoiSessionBridge.h"
#include <cstddef>
#include <cstdint>
#include <vector>

namespace mmo { namespace portable {

struct AoiDeliveryLimits {
    std::size_t maxFrameBytes = 0;
    std::uint32_t maxAttempts = 0;
    std::uint64_t maxPendingNs = 0;
};
enum class AoiDeliveryStatus {
    Accepted, Unbound, WrongThread, NotPending, WrongSequence,
    AlreadyAdmitted, EmptyFrame, FrameTooLarge, NotAdmitted,
    RetryLimit, Expired, InvalidClock
};
struct AoiDeliveryCounters {
    std::uint64_t admitted = 0;
    std::uint64_t attempts = 0;
    std::uint64_t acknowledged = 0;
    std::uint64_t rejectedFrame = 0;
    std::uint64_t rejectedAck = 0;
    std::uint64_t retryLimit = 0;
    std::uint64_t timeouts = 0;
    std::uint64_t resyncs = 0;
};

// One object per trusted session, on the world-owner thread. The caller
// performs encoding with an independent bounded codec, then passes its exact
// frame. Admission stores the owned bytes for byte-identical resend.
// authorizeSendAttempt() is a budget gate, NOT evidence of client receipt.
// On uncertain transmission, call invalidateForResync(); only authenticated,
// application-level ACK may call acknowledge().
class AoiBudgetedSession final {
public:
    AoiBudgetedSession(EntityKey viewer, std::uint64_t connectionGeneration,
                       std::uint64_t sceneEpoch, AoiDeliveryLimits limits,
                       std::size_t maxEvents = 256);
    bool bindOwnerToCurrentThread() noexcept;
    ReplicationPrepareResult prepare(SpatialAoi& world, std::int32_t x,
        std::int32_t y, std::uint32_t radius, std::uint64_t nowNs);
    AoiDeliveryStatus admit(std::uint64_t sequence,
                            std::vector<std::uint8_t> exactFrame,
                            std::uint64_t nowNs);
    AoiDeliveryStatus authorizeSendAttempt(std::uint64_t sequence,
                                           std::uint64_t nowNs);
    // Delivery is successful only after trusted, verified application ACK.
    ReplicationStatus acknowledge(const AuthenticatedAoiAck& ack);
    bool invalidateForResync();
    bool advanceScene(std::uint64_t newEpoch);
    bool advanceConnection(std::uint64_t newGeneration);

    const std::vector<std::uint8_t>& admittedFrame() const noexcept { return frame_; }
    bool hasAdmittedFrame() const noexcept { return admittedSequence_ != 0; }
    std::uint64_t admittedSequence() const noexcept { return admittedSequence_; }
    std::uint64_t admittedAtNs() const noexcept { return admittedAtNs_; }
    std::uint32_t attemptCount() const noexcept { return attempts_; }
    std::size_t admittedBytes() const noexcept { return frame_.size(); }
    std::size_t committedVisibleCount() const noexcept {
        return bridge_.committedVisibleCount();
    }
    std::uint64_t lastAcknowledgedSequence() const noexcept {
        return bridge_.lastAcknowledgedSequence();
    }
    AoiDeliveryCounters counters() const noexcept { return counters_; }
private:
    void clearFrame() noexcept;
    bool resetPending();
    AoiSessionBridge bridge_;
    const AoiDeliveryLimits limits_;
    std::vector<std::uint8_t> frame_;
    std::uint64_t admittedSequence_ = 0;
    std::uint64_t admittedAtNs_ = 0;
    std::uint32_t attempts_ = 0;
    AoiDeliveryCounters counters_;
};

}} // namespace mmo::portable
