# JX1 Position Synchronization — Root-Cause Audit (`audit.md`)

**Date:** 2026-10-17  
**Scope:** Server-first C++ Core (`jxOnline` / `jxOnline-r78` `gameserver/Sources/Core/Src/`) + Client C++ Core (`SwordOnline/Client/Classes/gamecore/`)  
**Verification Classification:** `SOURCE_REVIEWED` + `BUILD_VERIFIED` + `UNIT_TEST_VERIFIED`

---

## 1. Executive Summary

Our end-to-end audit of the JX1 (SwordOnline) Server and Client movement pipeline identified **6 concrete, deterministic code-level root causes** of position desynchronization ("lag vị trí", ghost-walking through obstacles, rubber-banding, and hit-and-run stop desync):

1. **Server/Client Pathfinding Turn & Timer Divergence (`KNpcFindPath.cpp`):**
   - **Client (`Client/Classes/gamecore/KNpcFindPath.cpp:167-172, 246-250`):** Stops pathfinding around obstacles when `m_nFindTimes > 1` (second corner turn) or `m_nFindTimer >= m_nMaxTimeLong` (`30` ticks), returning `0` -> `DoStand()`.
   - **Server (`gameserver/Sources/Core/Src/KNpcFindPath.cpp:144-149, 227-231`):** Both `m_nFindTimes > 1` and `m_nFindTimer >= m_nMaxTimeLong` were **commented out**! Whenever an entity hit a barrier and entered `m_nFindState == 1`, the Client stopped at the barrier while the Server continued walking around the obstacle indefinitely.
2. **Server `KNpc::DoStand()` Broadcast Completely Commented Out (`KNpc.cpp:3304-3335`):**
   - In `gameserver/Sources/Core/Src/KNpc.cpp:3304-3335`, the `#ifdef _SERVER` block that constructs and broadcasts `s2c_npcstand` (`NPC_PLAYER_TYPE_NORMAL_SYNC`, 25 bytes) to `CURREGION` and 8 `CONREGION` neighbors was commented out (`/* ... */`).
   - Whenever `KNpc::ServeMove()` stopped an entity (`GetDir != 1`), or when `OnHurt()` / `OnSkill()` finished and called `DoStand()`, the Server never notified nearby clients of the authoritative stop position.
3. **Server `KNpc::ServeMove()` Early Barrier Stop Without Authoritative Snap (`KNpc.cpp:6373-6377`):**
   - When `m_PathFinder.GetDir()` returned `!= 1` because of an obstacle before reaching `(m_DesX, m_DesY)`, `ServeMove()` called `DoStand()` and returned without broadcasting `s2c_npcsetpos` (`NPC_PLAYER_TYPE_NORMAL_SYNC`).
   - Because an unmodified Client's `NetCommandStand` (`s2c_npcstand`) ignores coordinates for non-self entities (`ConformIdx(nIdx)`) and has a Region 0 bug (`if (nRegion > 0)`), whereas `s2cnpcsetpos` (`s2c_npcsetpos`) reconciles all entities across all regions `0..8`, sending `s2c_npcsetpos` on barrier-induced early stops fixes even unmodified clients immediately.
4. **Server `KNpc::ProcCommand(0)` Drops Walk/Run Commands on Final Cast Frames & Rejects Immobilized Moves Silently (`KNpc.cpp:1000-1003, 1371-1406` & `KProtocolProcess.cpp:5436-5483`):**
   - In `KNpc::Activate()`, `ProcCommand(m_ProcessAI)` runs **before** `ProcStatus()`. When a player clicks to move as a skill/attack animation finishes, `c2s_npcrun` / `c2s_npcwalk` often arrives 1–3 frames before `OnSkill()` sets `m_ProcessAI = 1`. `ProcCommand(0)` wiped `m_Command.CmdKind = do_none` at line `1406`, destroying the move command while the Client ran across the map.
   - Conversely, when a player sent `c2s_npcwalk` / `c2s_npcrun` while genuinely immobilized on the Server (`m_StunState.nTime > 0`, `m_FreezeState.nTime & 1`, `m_randmove.nTime > 0`, or `m_Doing == do_hurt`), the Server ignored the move without sending an authoritative `s2c_npcsetpos` correction back to the client.
5. **Server Region Broadcast Upper-Bound Off-By-One Bug (`>` instead of `>=` in `KNpc.cpp`):**
   - `KNpc.cpp` lines `3524`, `4368`, `7926`, `8049`, `8241`, `8346` checked `nConRegion > SubWorld[m_SubWorldIndex].m_nTotalRegion` instead of `>=`, and `DoHurt()` (`line 3052`) / `DoStand()` (`line 3330`) only checked `CONREGIONIDX(i) == -1` without checking `>= m_nTotalRegion`.
6. **Client `NetCommandStand` Region 0 Bug & `SyncNpcMinPlayer` Same-Grid Blindness (`Client/Classes/gamecore/KProtocolProcess.cpp:703, 711, 2637-2716`):**
   - `NetCommandStand` (`line 703`) checked `if (nRegion > 0)` instead of `if (nRegion >= 0)` (skipping Region 0, the top-left active region) and passed `-1` to `NpcChangeRegion(-1, ...)` (`line 711`).
   - `SyncNpcMinPlayer` (`lines 2637-2716`) ignored `pSync->m_dwMapX / m_dwMapY` whenever `m_RegionIndex >= 0 && nRegion >= 0` (same 3x3 active grid).

---

## 2. File & Line Evidence Table

| ID | Component | File | Line(s) | Root Cause | Fix Applied |
|---|---|---|---|---|---|
| RC-1 | Server PathFinder | `gameserver/Sources/Core/Src/KNpcFindPath.cpp` | `144-149`, `227-231` | `m_nFindTimes > 1` & `m_nFindTimer >= m_nMaxTimeLong` commented out on Server but enabled on Client | Re-enabled behind `g_bPositionSyncFixEnabled` |
| RC-2 | Server Stop Sync | `gameserver/Sources/Core/Src/KNpc.cpp` | `3304-3335` | `s2c_npcstand` broadcast in `KNpc::DoStand()` commented out | Re-enabled with `0 <= nConRegion < m_nTotalRegion` bounds check |
| RC-3 | Server Barrier Stop | `gameserver/Sources/Core/Src/KNpc.cpp` | `6373-6377` | `ServeMove()` early barrier stop only called `DoStand()` | Added `bStoppedByBarrier` check + authoritative `s2c_npcsetpos` sync |
| RC-4 | Server Cmd Queue & Immobilize | `gameserver/Sources/Core/Src/KNpc.cpp`, `KProtocolProcess.cpp` | `1371-1406`, `5436-5483` | `ProcCommand(0)` wiped `do_walk`/`do_run` in final cast frames; immobilized move rejection sent no correction | Preserve `do_walk`/`do_run` in final 3 cast frames; send `s2c_npcsetpos` when rejected |
| RC-5 | Server Region Bounds | `gameserver/Sources/Core/Src/KNpc.cpp` | `3052`, `4201`, `4252`, `7926`, `8049`, `8241`, `8346`, `14898` | `nConRegion > m_nTotalRegion` off-by-one; `SetPos` missing direct self packet | Fixed all to `>= m_nTotalRegion`; fortified `SetPos` |
| RC-6 | Client Reconcile | `Client/Classes/gamecore/KProtocolProcess.cpp` | `678-723`, `2637-2716` | `if (nRegion > 0)` skips Region 0; `SyncNpcMinPlayer` ignores same-grid drift | Fixed `nRegion >= 0`, `NpcChangeRegion`, and drift threshold (`160`/`384` MPS) |
