# GAME-02.1 — Kingsoft entity source and ownership audit

**Master priority:** [JX3D_MASTER_ROADMAP.md](../JX3D_MASTER_ROADMAP.md) · M1/GAME-02 only
**Evidence baseline:** `main@c127c4bbb443285724833d257e7192138c2e5b1e` (PR #16 tick bridge, PR #17 master roadmap)
**Scope:** read-only source audit. This document is NOT a second roadmap, a gameplay bridge implementation, or an M1 acceptance claim.

## Proven source-field mapping

| Snapshot concern | Authoritative legacy source | Verified code location / interpretation |
| --- | --- | --- |
| Player object and slot | `Player[playerSlot]`; `KPlayer::m_nPlayerIndex`, `KPlayer::m_dwID` | `KPlayer.h:249,357,439-443`; `KPlayerSet.cpp:608-615`. Player slot is recycled; account/role lifetime is **not** proven by slot or `m_dwID` alone. |
| Player's visible world entity | `Player[playerSlot].m_nIndex -> Npc[npcSlot]` | `KPlayer.h:358`; `KPlayerSet.cpp:452-460`; `KNpc.h:330-339,496-497`. Verify reverse `Npc[npcSlot].m_nPlayerIdx == playerSlot` and kind is player before using. |
| NPC runtime identity | `Npc[npcSlot].m_dwID`; `Npc[npcSlot].m_Index` | `KNpc.h:330-338`; `KNpcSet.cpp:1087-1093`. `m_dwIDCreator` increments from 1 within process; `m_Index` is a reusable slot. Do not infer durable identity across restart/wrap. |
| Enumerating live NPCs | `NpcSet.GetNextIdx(current)` | `KNpcSet.cpp:1204-1209`; backed by used-slot list. Validate slot, `m_dwID != 0` and on-map state. Avoid scanning unallocated NPC slots. |
| Enumerating live players | `PlayerSet.GetFirstPlayer()/GetNextPlayer()` | `KPlayerSet.cpp:479-491`. These mutate an internal shared cursor; no nested/concurrent enumeration without owner exclusivity. |
| World/map identity | `Npc[npcSlot].m_SubWorldIndex` (slot), `SubWorld[slot].m_SubWorldID` (configured map ID), `SubWorld[slot].m_eMapState` | `KNpc.h:496-497`; `KSubWorld.h:34-56`; `KSubWorldSet.cpp:422-429,461-485`. The slot and external map ID are **different fields**; instance/scene epoch does not appear in this mapping and must be designed explicitly. |
| Region and position | `m_RegionIndex`, `m_MapX/Y`, `m_OffX/Y`, `m_Dir` on `KNpc` | `KNpc.h:496-497,702-706`; `KSubWorld.cpp:397-421`. `NewMap2Mps`: map region origin + cell coordinate times 32 + offset divided by 1024; result is legacy 2D map-pixel coordinates, **not meters or 3D world XYZ**. |
| Position helper safety | `KNpc::GetMpsPos(...)` | `KNpc.cpp:7034-7068`. Returns **SubWorld array slot** through `nMapid`, not `m_SubWorldID`; invalid region can lead to `(0,0)`. Validate world slot, map lifecycle, region range **before** conversion. |
| Remove and slot reuse | `KNpcSet::Remove` | `KNpcSet.cpp:989-1042`. Removal clears NPC and returns slot to free list; same index can later represent a different NPC. Presence diff must emit despawn, and generation must disambiguate reuse. |
| Owner tick notification | `CoreServerShell` → `g_SubWorldSet.MainLoop()` → `SubWorld[i].Activate()` → `Jx3dNotifyWorldTick` | `CoreServerShell.cpp:2263`; `KSubWorldSet.cpp:525-641`; `Jx3dLegacyWorldTickBridge.*`. Current map Activate loop is sequential, with hook **after** it, compiled only under `_SERVER && JX3D_ENABLE_LEGACY_WORLD_TICK_HOOK`. This proves call placement, **not exclusive ownership against all IO/worker writes**. |
| Incoming client/event path | `KSOServer` → `CoreServerShell::ProcessClientMessage` and `AddPlayerToWorld` | `KSOServer.cpp:3789,3798`. Inspect dispatch/threading to ensure no concurrent mutations while snapshotting. Do not register direct network-thread reads of `Player[]`/`Npc[]`. |
| Portable AOI identity | `mmo::portable::EntityKey` | `portable_core/include/mmo/SpatialAoi.h:13-25`: `runtimeId`, `generation`, `worldId`, `instanceId`. Generation and scene epoch must be set by trusted owner lifecycle, never client or slot-only mapping. |

## Blockers to resolve before implementing snapshot callback

1. **Thread ownership not fully proven.** Follow `CoreServerShell::Breathe` caller and `KSOServer` packet/IOCP dispatch; ensure all mutations serialize with the tick. If not, create owner-thread command scheduling/snapshot copy point; no foreign-thread global pointer reads.
2. **EntityKey generation and map epoch.** Assign bounded owner-owned generations for NPC-slot lifecycle and session/role lifecycle. Distinguish map ID from map slot; explicitly increment scene epoch on teleport/instance reset/map unload and handle `m_eMapState`.
3. **Despawn is not exposed in adapter.** `SpatialAoi::erase(key)` exists (`SpatialAoi.h`), but `WorldServerIntegrationAdapter.h/.cpp` publicly exposes `upsert` only. A reviewed owner-only erase path with correct quota/tombstone behavior is necessary for removal/logout/teleport.
4. **Position validation and units.** Reject invalid world/region, unsafely cast or overflowed map coordinates, and entries not actually present in world; snapshot retains 2D integer map pixels. Do not invent Z/yaw or implicit 3D scale.
5. **Bounded work per tick.** Cap `NpcSet` scan, copied snapshots, pending deltas, AOI writes and time budget; define recover/resync when quota is exceeded. Never retain `KPlayer*`, `KNpc*` or references to live world arrays.
6. **Lifecycle tests.** Required: spawn, move, remove, logout, disconnect, teleport, map unload/reload, NPC slot reuse/new generation, player slot reuse, frame/ID wrap, stale scene/session, wrong-thread access, quota overflow. Harness must use gameplay-sourced snapshots, not only synthetic positions.

## Gate and status

- **M1.1 source mapping:** partial evidence above; still BLOCKED on full dispatch/owner-thread proof and scene-generation policy.
- **M1.2 bridge:** NOT IMPLEMENTED by this audit. Feature must remain OFF in existing Windows server builds.
- **Win64 build, portable sanitizer, live gameplay E2E, runtime deployment:** NOT RUN by this documentation-only change.
- **Next action within M1:** finish ownership trace; add a bounded value-only read/copy seam on the proven owner thread; extend adapter with owner-only erase + lifecycle tests; then validate in an isolated server/harness.
- **Rollback:** documentation-only change; no gameplay source, binaries, database or running processes modified.
