# FROM ANTIGRAVITY — JX1 POSITION SYNC REPORT (`FROM-ANTIGRAVITY-POSITION-SYNC-001`)

**Date:** 2026-10-17  
**In Response To:** `TO-ANTIGRAVITY-JX1-POSITION-SYNC-001.md` & `TO-ANTIGRAVITY-JX1-POSITION-SYNC-RESEARCH-002.md`  
**Status:** COMPLETED (Server-first fixes applied, compiled with 0 errors, synced to `jxOnline-r78`, Client handlers patched, 7/7 deterministic tests passing)

---

## 1. Summary of Root Causes Found & Fixed

1. **Server/Client Pathfinding Divergence (`KNpcFindPath.cpp:144-149, 227-231`):**
   - `m_nFindTimes > 1` (corner-turn limit) and `m_nFindTimer >= m_nMaxTimeLong` (`30`-frame obstacle timer) were enabled in `Client/Classes/gamecore/KNpcFindPath.cpp` (`lines 167-172, 246-250`) but **commented out** in `Server/KNpcFindPath.cpp`! Re-enabled on Server behind `g_bPositionSyncFixEnabled` so Server and Client obstacle pathfinding terminate at the exact same tick.
2. **Server `KNpc::DoStand()` Broadcast Commented Out (`KNpc.cpp:3304-3335`):**
   - Re-enabled `s2c_npcstand` (`NPC_PLAYER_TYPE_NORMAL_SYNC`, 25 bytes) broadcast in `KNpc::DoStand()` with strict `0 <= nConRegion < SubWorld[m_SubWorldIndex].m_nTotalRegion` bounds checking.
3. **Server `KNpc::ServeMove()` Barrier Stop Reconciliation (`KNpc.cpp:6373-6377`):**
   - When `m_PathFinder.GetDir()` returns `!= 1` due to a barrier before reaching `(m_DesX, m_DesY)`, `ServeMove()` now calls `DoStand()` **and** sends an authoritative `s2c_npcsetpos` (`NPC_PLAYER_TYPE_NORMAL_SYNC`) to nearby regions and the player (`PackDataToClient`), which reconciles even unmodified Clients across all regions `0..8` (bypassing the Client's `NetCommandStand` Region 0 bug).
4. **Server `KNpc::ProcCommand(0)` & `NpcWalkCommand`/`NpcRunCommand` (`KNpc.cpp:1371-1406`, `KProtocolProcess.cpp:5436-5483`):**
   - Buffered `do_walk`/`do_run` commands arriving during the final 3 frames of a skill/attack/jump animation (`(m_Frames.nTotalFrame - m_Frames.nCurrentFrame) <= 3`) instead of wiping `m_Command.CmdKind = do_none` before `ProcStatus()` sets `m_ProcessAI = 1`.
   - Added immediate `s2c_npcsetpos` authoritative reconciliation when a player sends `c2s_npcwalk` or `c2s_npcrun` while stunned (`m_StunState`), frozen (`m_FreezeState`), confused (`m_randmove`), or in `do_hurt`.
5. **Server Region Broadcast Bounds & `SetPos` (`KNpc.cpp:3052, 4201, 4252, 7926, 8049, 8241, 8346, 14898`):**
   - Fixed all 6 occurrences of `nConRegion > m_nTotalRegion` off-by-one bugs to `>=`, added upper-bound checks in `DoHurt()`, guarded `RunTo()`/`Goto()`, and fortified `SetPos()` to send `s2c_npcsetpos` directly to the self player (`PackDataToClient`) as well as nearby regions for both players and NPCs.
6. **Client Position Sync Handlers (`Client/Classes/gamecore/KProtocolProcess.cpp:678-723, 2637-2716`):**
   - Fixed `if (nRegion > 0)` -> `if (nRegion >= 0)` and `NpcChangeRegion` in `NetCommandStand`, plus same-grid drift reconciliation (`160` MPS standing / `384` MPS moving) in `SyncNpcMinPlayer`.

---

## 2. Deliverables Index

- Audit Report: [`docs/position-sync/audit.md`](file:///c:/Server-Game/SwordOnline/jxOnline-r78/docs/position-sync/audit.md)
- Wire Protocol Matrix: [`docs/position-sync/protocol-matrix.md`](file:///c:/Server-Game/SwordOnline/jxOnline-r78/docs/position-sync/protocol-matrix.md)
- Deep Audit & Fork Analysis: [`docs/position-sync/DEEP_AUDIT.md`](file:///c:/Server-Game/SwordOnline/jxOnline-r78/docs/position-sync/DEEP_AUDIT.md)
- Build & Verification Results: [`docs/position-sync/results.md`](file:///c:/Server-Game/SwordOnline/jxOnline-r78/docs/position-sync/results.md)
- Deterministic Test Suite: [`tests/position_sync_test.py`](file:///c:/Server-Game/SwordOnline/jxOnline-r78/tests/position_sync_test.py)
