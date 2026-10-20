#include "mmo/AoiSessionBridge.h"
#include <stdexcept>

namespace mmo { namespace portable {

AoiSessionBridge::AoiSessionBridge(EntityKey viewer,
    std::uint64_t connectionGeneration, std::uint64_t sceneEpoch,
    std::size_t maxEvents)
    : replication_(viewer, maxEvents),
      connectionGeneration_(connectionGeneration), sceneEpoch_(sceneEpoch) {
    if (connectionGeneration == 0 || sceneEpoch == 0)
        throw std::invalid_argument("AOI bridge requires nonzero server generations");
}

bool AoiSessionBridge::bindOwnerToCurrentThread() noexcept {
    return replication_.bindOwnerToCurrentThread();
}

ReplicationPrepareResult AoiSessionBridge::prepare(SpatialAoi& world,
    std::int32_t x, std::int32_t y, std::uint32_t radius, std::uint64_t nowNs) {
    const auto access = replication_.accessStatus();
    if (access != ReplicationStatus::Prepared) {
        ReplicationPrepareResult result;
        result.status = access;
        return result;
    }
    return replication_.prepare(world, x, y, radius, sceneEpoch_, nowNs);
}

ReplicationPrepareResult AoiSessionBridge::retry() const {
    return replication_.retry();
}

ReplicationStatus AoiSessionBridge::acknowledge(const AuthenticatedAoiAck& ack) {
    // Avoid reading mutable bridge generations from a foreign thread.
    const auto access = replication_.accessStatus();
    if (access != ReplicationStatus::Prepared) return access;
    // These values must come from authenticated server-side transport context.
    // The bridge is NOT a substitute for cryptographic authentication.
    if (!(ack.viewer == replication_.viewer()))
        return ReplicationStatus::WrongViewer;
    if (ack.connectionGeneration != connectionGeneration_)
        return ReplicationStatus::WrongConnection;
    if (ack.sceneEpoch != sceneEpoch_)
        return ReplicationStatus::WrongEpoch;
    return replication_.acknowledge(ack.sequence);
}

bool AoiSessionBridge::advanceScene(std::uint64_t newSceneEpoch) {
    if (replication_.accessStatus() != ReplicationStatus::Prepared) return false;
    if (newSceneEpoch <= sceneEpoch_) return false;
    if (!replication_.invalidateForResync()) return false;
    sceneEpoch_ = newSceneEpoch;
    return true;
}

bool AoiSessionBridge::advanceConnection(std::uint64_t newConnectionGeneration) {
    if (replication_.accessStatus() != ReplicationStatus::Prepared) return false;
    if (newConnectionGeneration <= connectionGeneration_) return false;
    if (!replication_.invalidateForResync()) return false;
    connectionGeneration_ = newConnectionGeneration;
    return true;
}

bool AoiSessionBridge::timedOut(std::uint64_t nowNs,
    std::uint64_t timeoutNs) const noexcept {
    return replication_.timedOut(nowNs, timeoutNs);
}

std::size_t AoiSessionBridge::committedVisibleCount() const noexcept {
    return replication_.committedVisibleCount();
}
std::uint64_t AoiSessionBridge::lastAcknowledgedSequence() const noexcept {
    return replication_.lastAcknowledgedSequence();
}
bool AoiSessionBridge::hasInFlight() const noexcept {
    return replication_.hasInFlight();
}

}} // namespace mmo::portable
