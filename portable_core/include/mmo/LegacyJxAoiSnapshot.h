#pragma once
// Strict legacy JX owner-thread snapshot -> portable SpatialAoi entity conversion.
// No dependency on the legacy DLL, no direct process globals, no sockets.
#include "mmo/SpatialAoi.h"
#include <cstdint>
#include <limits>

namespace mmo { namespace portable {

// Captured synchronously on the authoritative KSubWorld activation/owner thread.
// For players, the authoritative visible entity is Npc[Player[n].m_nIndex],
// NOT Player[n].m_dwID. Never accept this struct from an untrusted client.
struct LegacyJxNpcSnapshot {
    std::int32_t npcArrayIndex = -1;   // 1 <= slot < MAX_NPC; not a stable identity
    std::uint32_t npcDwId = 0;        // Npc[slot].m_dwID, not player or template ID
    std::uint32_t spawnGeneration = 0; // server-owned lifecycle counter
    std::int32_t subWorldArrayIndex = -1;
    std::int32_t getMpsMapIndex = -1; // KNpc::GetMpsPos outputs SubWorld array index
    std::int64_t mapId = -1;         // SubWorld[subWorldArrayIndex].m_SubWorldID
    std::uint32_t instanceId = 0;  // server-assigned; 0 for non-instanced maps
    std::int32_t regionIndex = -1;
    std::int32_t totalRegions = 0;
    std::int64_t mpsX = 0;
    std::int64_t mpsY = 0;
    std::uint64_t revision = 0; // monotonic, assigned on WorldServer owner thread
    bool visible = true;
};

enum class LegacyJxSnapshotStatus {
    Ok, InvalidNpcArraySlot, InvalidNpcId, MissingSpawnGeneration,
    InvalidSubWorldIndex, MapIndexMismatch, InvalidMapId,
    InvalidRegion, PositionOutOfBounds, MissingRevision
};

// Fail closed: the caller provides the legacy array capacities from the
// compiled server, avoiding hard-coded MAX_NPC/MAX_SUBWORLD assumptions.
// No coordinate rescaling: KNpc::GetMpsPos already yields MPS units.
inline LegacyJxSnapshotStatus convertLegacyJxNpcSnapshot(
    const LegacyJxNpcSnapshot& s, std::int32_t maxNpcs,
    std::int32_t maxSubWorlds, AoiEntity& result) noexcept {
    if (s.npcArrayIndex <= 0 || s.npcArrayIndex >= maxNpcs)
        return LegacyJxSnapshotStatus::InvalidNpcArraySlot;
    if (s.npcDwId == 0) return LegacyJxSnapshotStatus::InvalidNpcId;
    if (s.spawnGeneration == 0)
        return LegacyJxSnapshotStatus::MissingSpawnGeneration;
    if (s.subWorldArrayIndex < 0 || s.subWorldArrayIndex >= maxSubWorlds)
        return LegacyJxSnapshotStatus::InvalidSubWorldIndex;
    if (s.getMpsMapIndex != s.subWorldArrayIndex)
        return LegacyJxSnapshotStatus::MapIndexMismatch;
    if (s.mapId <= 0 ||
        s.mapId > static_cast<std::int64_t>(std::numeric_limits<std::uint32_t>::max()))
        return LegacyJxSnapshotStatus::InvalidMapId;
    if (s.regionIndex < 0 || s.regionIndex >= s.totalRegions)
        return LegacyJxSnapshotStatus::InvalidRegion;
    if (s.mpsX < std::numeric_limits<std::int32_t>::min() ||
        s.mpsX > std::numeric_limits<std::int32_t>::max() ||
        s.mpsY < std::numeric_limits<std::int32_t>::min() ||
        s.mpsY > std::numeric_limits<std::int32_t>::max())
        return LegacyJxSnapshotStatus::PositionOutOfBounds;
    if (s.revision == 0) return LegacyJxSnapshotStatus::MissingRevision;
    AoiEntity next;
    next.key.runtimeId = s.npcDwId;
    next.key.generation = s.spawnGeneration;
    next.key.worldId = static_cast<std::uint32_t>(s.mapId);
    next.key.instanceId = s.instanceId;
    next.x = static_cast<std::int32_t>(s.mpsX);
    next.y = static_cast<std::int32_t>(s.mpsY);
    next.revision = s.revision;
    next.visible = s.visible;
    result = next; // one final store; invalid input never modifies output
    return LegacyJxSnapshotStatus::Ok;
}

}} // namespace mmo::portable
