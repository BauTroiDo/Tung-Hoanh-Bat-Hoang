#include "mmo/AoiDeliveryBudget.h"
#include <stdexcept>
#include <utility>

namespace mmo { namespace portable {

AoiBudgetedSession::AoiBudgetedSession(
    EntityKey viewer, std::uint64_t connectionGeneration,
    std::uint64_t sceneEpoch, AoiDeliveryLimits limits,
    std::size_t maxEvents)
    : bridge_(viewer, connectionGeneration, sceneEpoch, maxEvents),
      limits_(limits) {
    if (limits_.maxFrameBytes == 0 || limits_.maxAttempts == 0 ||
        limits_.maxPendingNs == 0)
        throw std::invalid_argument("AOI delivery requires positive frame, attempts and age limits");
}

bool AoiBudgetedSession::bindOwnerToCurrentThread() noexcept {
    return bridge_.bindOwnerToCurrentThread();
}

ReplicationPrepareResult AoiBudgetedSession::prepare(
    SpatialAoi& world, std::int32_t x, std::int32_t y,
    std::uint32_t radius, std::uint64_t nowNs) {
    return bridge_.prepare(world, x, y, radius, nowNs);
}

void AoiBudgetedSession::clearFrame() noexcept {
    // Release retained capacity, not just logical length. Otherwise global
    // admission counters could show 0 while thousands of sessions retain
    // max-frame-sized heap allocations after ACK/resync.
    std::vector<std::uint8_t>().swap(frame_);
    admittedSequence_ = 0;
    admittedAtNs_ = 0;
    attempts_ = 0;
}

bool AoiBudgetedSession::resetPending() {
    if (!bridge_.invalidateForResync()) return false;
    clearFrame();
    ++counters_.resyncs;
    return true;
}

AoiDeliveryStatus AoiBudgetedSession::admit(
    std::uint64_t sequence, std::vector<std::uint8_t> exactFrame,
    std::uint64_t nowNs) {
    const auto access = bridge_.accessStatus();
    if (access == ReplicationStatus::Unbound) return AoiDeliveryStatus::Unbound;
    if (access != ReplicationStatus::Prepared) return AoiDeliveryStatus::WrongThread;
    if (!bridge_.hasInFlight()) return AoiDeliveryStatus::NotPending;
    if (admittedSequence_ != 0) return AoiDeliveryStatus::AlreadyAdmitted;
    const auto pending = bridge_.retry();
    if (pending.status != ReplicationStatus::Prepared)
        return AoiDeliveryStatus::NotPending;
    if (sequence == 0 || sequence != pending.batch.sequence)
        return AoiDeliveryStatus::WrongSequence;
    if (exactFrame.empty()) {
        ++counters_.rejectedFrame;
        resetPending(); // fail closed: uncertain encoding requires full RESET
        return AoiDeliveryStatus::EmptyFrame;
    }
    if (exactFrame.size() > limits_.maxFrameBytes) {
        ++counters_.rejectedFrame;
        resetPending();
        return AoiDeliveryStatus::FrameTooLarge;
    }
    frame_ = std::move(exactFrame);
    admittedSequence_ = sequence;
    admittedAtNs_ = nowNs;
    ++counters_.admitted;
    return AoiDeliveryStatus::Accepted;
}

AoiDeliveryStatus AoiBudgetedSession::authorizeSendAttempt(
    std::uint64_t sequence, std::uint64_t nowNs) {
    const auto access = bridge_.accessStatus();
    if (access == ReplicationStatus::Unbound) return AoiDeliveryStatus::Unbound;
    if (access != ReplicationStatus::Prepared) return AoiDeliveryStatus::WrongThread;
    if (admittedSequence_ == 0) return AoiDeliveryStatus::NotAdmitted;
    if (sequence != admittedSequence_) return AoiDeliveryStatus::WrongSequence;
    if (nowNs < admittedAtNs_) {
        resetPending();
        return AoiDeliveryStatus::InvalidClock;
    }
    if (nowNs - admittedAtNs_ >= limits_.maxPendingNs) {
        ++counters_.timeouts;
        resetPending();
        return AoiDeliveryStatus::Expired;
    }
    if (attempts_ >= limits_.maxAttempts) {
        ++counters_.retryLimit;
        resetPending();
        return AoiDeliveryStatus::RetryLimit;
    }
    ++attempts_;
    ++counters_.attempts;
    return AoiDeliveryStatus::Accepted;
}

ReplicationStatus AoiBudgetedSession::acknowledge(
    const AuthenticatedAoiAck& ack) {
    const auto access = bridge_.accessStatus();
    if (access != ReplicationStatus::Prepared) return access;
    if (admittedSequence_ == 0 || attempts_ == 0)
        return ReplicationStatus::NotPending;
    const auto status = bridge_.acknowledge(ack);
    if (status == ReplicationStatus::Prepared) {
        clearFrame();
        ++counters_.acknowledged;
    } else {
        ++counters_.rejectedAck;
    }
    return status;
}

bool AoiBudgetedSession::invalidateForResync() {
    if (bridge_.accessStatus() != ReplicationStatus::Prepared) return false;
    return resetPending();
}

bool AoiBudgetedSession::advanceScene(std::uint64_t newEpoch) {
    if (!bridge_.advanceScene(newEpoch)) return false;
    clearFrame();
    ++counters_.resyncs;
    return true;
}

bool AoiBudgetedSession::advanceConnection(std::uint64_t newGeneration) {
    if (!bridge_.advanceConnection(newGeneration)) return false;
    clearFrame();
    ++counters_.resyncs;
    return true;
}

}} // namespace mmo::portable
