#pragma once
// Typed owner-thread capture from actual legacy KNpc/KSubWorld-shaped objects.
// This is only built when included by an authorized legacy integration unit.
// The legacy arrays must refer to the exact WorldServer process being observed.
#include "mmo/LegacyJxAoiLifecycleBridge.h"
#include <cstdint>

namespace mmo { namespace portable {

enum class LegacyJxCaptureStatus {
    Ok, WrongThread, MissingArrays, InvalidNpcSlot, InactiveNpc,
    InvalidSubWorld, UnloadedWorld, InvalidRegion, MpsMapIndexMismatch,
    InvalidActualMapId
};

// This is an opt-in *capture adapter*, not a background poller. NEVER call
// from a network/packet worker. It accesses NPC + SubWorld state synchronously
// on the same thread that updates KSubWorld::Activate.
//
// NPC-shaped object must provide m_dwID, m_SubWorldIndex, m_RegionIndex,
// GetMpsPos(int*,int*,int*). SubWorld-shaped object must provide
// m_Region, m_nTotalRegion, m_SubWorldID.
// For player characters pass Player[p].m_nIndex as slot and capture the
// corresponding Npc[slot] -- never use Player[p].m_dwID for AOI.
template<class NpcT, class WorldT>
LegacyJxCaptureStatus captureLegacyJxNpcOnOwner(
    const LegacyJxAoiLifecycleBridge& bridge,
    NpcT* npcs, std::int32_t npcCapacity,
    WorldT* worlds, std::int32_t worldCapacity,
    std::int32_t slot, std::uint32_t instanceId, bool visible,
    LegacyJxNpcSnapshot& out) {
    // Order is deliberate: no reads of raw game arrays on a foreign thread.
    if (!bridge.isOwnerThread()) return LegacyJxCaptureStatus::WrongThread;
    if (!npcs || !worlds || npcCapacity<=1 || worldCapacity<=0)
        return LegacyJxCaptureStatus::MissingArrays;
    if (slot<=0 || slot>=npcCapacity) return LegacyJxCaptureStatus::InvalidNpcSlot;
    NpcT& npc=npcs[slot];
    if (!npc.m_dwID) return LegacyJxCaptureStatus::InactiveNpc;
    const auto subWorld=static_cast<std::int32_t>(npc.m_SubWorldIndex);
    if (subWorld<0 || subWorld>=worldCapacity)
        return LegacyJxCaptureStatus::InvalidSubWorld;
    WorldT& world=worlds[subWorld];
    if (!world.m_Region) return LegacyJxCaptureStatus::UnloadedWorld;
    if (npc.m_RegionIndex<0 || npc.m_RegionIndex>=world.m_nTotalRegion)
        return LegacyJxCaptureStatus::InvalidRegion;
    if (world.m_SubWorldID<=0)
        return LegacyJxCaptureStatus::InvalidActualMapId;
    // KNpc::GetMpsPos's third parameter is m_SubWorldIndex, NOT map ID.
    int x=0,y=0,reportedIndex=-1;
    npc.GetMpsPos(&x,&y,&reportedIndex);
    if (reportedIndex!=subWorld)
        return LegacyJxCaptureStatus::MpsMapIndexMismatch;
    LegacyJxNpcSnapshot s;
    s.npcArrayIndex=slot;
    s.npcDwId=npc.m_dwID;
    s.subWorldArrayIndex=subWorld;
    s.getMpsMapIndex=reportedIndex;
    s.mapId=world.m_SubWorldID;
    s.instanceId=instanceId;
    s.regionIndex=npc.m_RegionIndex;
    s.totalRegions=world.m_nTotalRegion;
    s.mpsX=x;
    s.mpsY=y;
    s.visible=visible;
    // Lifecycle bridge owns spawnGeneration and revision, and assigns them
    // only on accepted world mutation.
    out=s;
    return LegacyJxCaptureStatus::Ok;
}

}} // namespace mmo::portable
