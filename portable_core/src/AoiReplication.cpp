#include "mmo/AoiReplication.h"
#include <limits>
#include <stdexcept>
#include <utility>

namespace mmo { namespace portable {

AoiReplicationSession::AoiReplicationSession(EntityKey viewer,
    std::size_t maxEvents)
    : viewer_(viewer), maxEvents_(maxEvents),
      committed_(new AoiObserverView(viewer)) {
    if (viewer.runtimeId == 0 || viewer.generation == 0 || maxEvents == 0)
        throw std::invalid_argument("AOI session requires valid viewer and budget");
}

bool AoiReplicationSession::bindOwnerToCurrentThread() noexcept {
    if (!ownerBound_) {
        owner_ = std::this_thread::get_id();
        ownerBound_ = true;
    }
    return owner_ == std::this_thread::get_id();
}

ReplicationStatus AoiReplicationSession::ownerStatus() const noexcept {
    if (!ownerBound_) return ReplicationStatus::Unbound;
    if (owner_ != std::this_thread::get_id())
        return ReplicationStatus::WrongThread;
    return ReplicationStatus::Prepared; // owner accepted
}

std::size_t AoiReplicationSession::committedVisibleCount() const noexcept {
    return committed_->knownCount();
}
std::uint64_t AoiReplicationSession::committedEpoch() const noexcept {
    return committed_->lastEpoch();
}

ReplicationPrepareResult AoiReplicationSession::prepare(SpatialAoi& world,
    std::int32_t x, std::int32_t y, std::uint32_t radius,
    std::uint64_t epoch, std::uint64_t nowNs) {
    ReplicationPrepareResult result;
    result.status = ownerStatus();
    if (result.status != ReplicationStatus::Prepared) return result;
    if (staged_) {
        result.status = ReplicationStatus::InFlight;
        return result;
    }
    if (nextSequence_ == 0) {
        result.status = ReplicationStatus::SequenceExhausted;
        return result;
    }

    // Never advance committed client knowledge during planning. Memory
    // allocation errors propagate to caller; the committed view remains intact.
    auto tentative = std::unique_ptr<AoiObserverView>(
        new AoiObserverView(*committed_));
    const auto plan = world.plan(*tentative, x, y, radius, epoch, maxEvents_);
    result.plannerStatus = plan.status;
    if (plan.status != AoiStatus::Ok) {
        result.status = ReplicationStatus::PlannerRejected;
        return result;
    }
    if (plan.events.empty()) {
        result.status = ReplicationStatus::NoChanges;
        return result;
    }

    ReplicationBatch next;
    next.sequence = nextSequence_;
    next.sceneEpoch = epoch;
    next.events = plan.events;
    // Commit outstanding state only after all allocations/copies succeed.
    result.batch = next;  // Possible allocation failure cannot advance state.
    pending_ = std::move(next);
    staged_ = std::move(tentative);
    preparedAtNs_ = nowNs;
    ++nextSequence_; // UINT64_MAX advances to zero -> permanently exhausted
    result.status = ReplicationStatus::Prepared;
    return result;
}

ReplicationPrepareResult AoiReplicationSession::retry() const {
    ReplicationPrepareResult result;
    result.status = ownerStatus();
    if (result.status != ReplicationStatus::Prepared) return result;
    if (!staged_) {
        result.status = ReplicationStatus::NotPending;
        return result;
    }
    result.batch = pending_; // caller may re-encode/re-send exact same sequence
    return result;
}

ReplicationStatus AoiReplicationSession::acknowledge(std::uint64_t sequence) {
    const auto access = ownerStatus();
    if (access != ReplicationStatus::Prepared) return access;
    if (!staged_) return ReplicationStatus::NotPending;
    if (sequence != pending_.sequence) return ReplicationStatus::WrongSequence;
    committed_.swap(staged_);
    staged_.reset();
    acknowledged_ = sequence;
    pending_ = {};
    return ReplicationStatus::Prepared;
}

bool AoiReplicationSession::invalidateForResync() {
    if (ownerStatus() != ReplicationStatus::Prepared) return false;
    // Construct before modifying existing state, preserving strong safety.
    std::unique_ptr<AoiObserverView> fresh(new AoiObserverView(viewer_));
    committed_.swap(fresh);
    staged_.reset();
    pending_ = {};
    return true;
}

bool AoiReplicationSession::timedOut(std::uint64_t nowNs,
    std::uint64_t timeoutNs) const noexcept {
    return ownerStatus() == ReplicationStatus::Prepared && staged_ &&
        timeoutNs > 0 && nowNs >= preparedAtNs_ &&
        nowNs - preparedAtNs_ >= timeoutNs;
}

}} // namespace mmo::portable
