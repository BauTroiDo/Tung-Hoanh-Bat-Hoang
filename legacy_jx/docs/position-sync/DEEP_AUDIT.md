# JX1 Position Synchronization — Deep Audit & Fork Comparison (`DEEP_AUDIT.md`)

**Date:** 2026-10-17  
**Scope:** Critical evaluation of community fork workarounds (`AutoFixXY`, `HurtAutoMove`, `NetCommandStand`) vs. our authoritative Server-first implementation.  
**Verification Status:** `SOURCE_REVIEWED` + `BUILD_VERIFIED` + `UNIT_TEST_VERIFIED`

---

## 1. Why Blindly Copying Reference Fork Code (`SwordOnline-1226-Update2` / `tuyen-tl/jx-3`) Is Unsafe

In `TO-ANTIGRAVITY-JX1-POSITION-SYNC-RESEARCH-002.md`, several community fork patches (`AutoFixXY()`, `HurtAutoMove()`, and `NetCommandStand()`) were flagged as **reference-only hypotheses** requiring deep verification before adoption. Our code audit confirmed that **all three community fork snippets contain severe bugs**:

### 1.1 `AutoFixXY()` (`nRegion == -1` Out-of-Bounds Array Access)
- In the community fork's `KNpc::AutoFixXY()`:
  ```cpp
  if (nRegion == -1) { ... }
  else if (nOldRegion != nRegion || nRegion < 0) { ... }
  // Followed unconditionally by:
  SubWorld[m_SubWorldIndex].m_Region[nRegion].AddRef(m_MapX, m_MapY, obj_npc);
  ```
- **Defect:** When `nRegion == -1` (entity moved outside the client's 3x3 active region grid), `m_Region[-1].AddRef(...)` indexes `-1` before the start of `SubWorld[0].m_Region[9]`, corrupting heap/BSS memory!
- **Decision:** **Rejected.** We did not port `AutoFixXY()`.

### 1.2 `NetCommandStand()` (`if (nRegion > 0)` Region 0 Bug & `NpcChangeRegion(-1, ...)` List Corruption)
- Both the reference fork and the existing `Client/Classes/gamecore/KProtocolProcess.cpp:703-711` had:
  ```cpp
  if (nRegion > 0)
  {
      SubWorld[0].m_Region[Npc[nIdx].m_RegionIndex].DecRef(Npc[nIdx].m_MapX, Npc[nIdx].m_MapY, obj_npc);
      Npc[nIdx].m_RegionIndex = nRegion;
      Npc[nIdx].m_dwRegionID = SubWorld[0].m_Region[nRegion].m_RegionID;
      SubWorld[0].NpcChangeRegion(-1, SubWorld[0].m_Region[nRegion].m_RegionID, nIdx);
  ```
- **Defect 1:** On the client, `SubWorld[0].m_Region[0..8]` uses indices `0..8`. `nRegion == 0` is a 100% valid active region (top-left of the 3x3 grid). `if (nRegion > 0)` silently drops all stand corrections in Region 0!
- **Defect 2:** Calling `NpcChangeRegion(-1, ...)` unconditionally (even when `nOldRegion == nRegion`) executes `m_Region[nDest].AddNpc(nIdx)` without removing `nIdx` from `nOldRegion`, corrupting `m_NpcList` (`KIndexNode` doubly-linked list)!
- **Decision:** **Fixed properly** in `Client/Classes/gamecore/KProtocolProcess.cpp` (`if (nRegion >= 0)` and only calling `NpcChangeRegion(nSrcRegionID, ...)` when `nOldRegion != nRegion`).

### 1.3 Why `s2c_npcsetpos` (`s2cnpcsetpos`) on the Server Solves Even Unmodified Clients
- Our audit of `Client/Classes/gamecore/KProtocolProcess.cpp:2311-2365` (`KProtocolProcess::s2cnpcsetpos`) revealed that `s2c_npcsetpos` uses the **exact same 25-byte `NPC_PLAYER_TYPE_NORMAL_SYNC` packet** as `s2c_npcstand`, and its existing handler in the compiled client binary:
  1. Works for **all** `nIdx > 0` (local player, other players, normal NPCs, partners),
  2. Properly checks `if (Npc[nIdx].m_RegionIndex != nRegion || nRegion < 0)` and passes the true old `m_RegionID` to `NpcChangeRegion`,
  3. Works in **all** regions including `nRegion == 0`,
  4. Updates `m_MapX`, `m_MapY`, `m_OffX`, `m_OffY`, `AddRef`, and transitions the entity to `do_stand` (`SendSerCommand(do_stand)`).
- Therefore, by having the Server emit `s2c_npcstand` on normal stops (`KNpc::DoStand`) **and** `s2c_npcsetpos` on abnormal/divergent stops (early barrier collision in `ServeMove`, rejected walk/run while stunned/frozen/hurt/casting in `NpcWalkCommand`/`NpcRunCommand`/`ProcCommand(0)`, and `SetPos`), **even an existing unmodified Client binary immediately reconciles position without needing a client rebuild first**.

---

## 2. Deep Discovery: Why Hit-and-Run (PK Kiting) Caused Severe Desync

A critical root cause not previously identified in community forks was uncovered in `KNpc::Activate()` (`KNpc.cpp:1000-1003`) and `KNpc::ProcCommand(int nAI)` (`KNpc.cpp:1296-1407`):

1. On every server frame (`55.5ms` at 18 FPS), `KNpc::Activate()` calls:
   ```cpp
   ProcCommand(m_ProcessAI); // line 1000
   ProcStatus();             // line 1003
   ```
2. During a skill cast (`do_attack`, `do_magic`, `do_skill`, `do_special1..3`) or jump (`do_jump`), `m_ProcessAI` is `0` until `WaitForFrame()` completes inside `ProcStatus() -> OnSkill()`.
3. On the Client, when `OnSkill()` finishes its last frame, the player clicks to run (`c2s_npcrun`). Due to network jitter or 1-frame attack-speed rounding differences, `c2s_npcrun` arrives at the Server 1–3 frames before `OnSkill()` sets `m_ProcessAI = 1` (or on the very frame `OnSkill()` finishes, because `ProcCommand(0)` runs 3 lines *before* `ProcStatus()`!).
4. Previously, `ProcCommand(0)` hit `default: break;` and executed `m_Command.CmdKind = do_none;` (`line 1406`), **permanently deleting the player's `do_run` / `do_walk` command**!
5. As a result, the Client ran across the map while the Server stood still at the skill cast spot.
6. **Our Fix (`KNpc.cpp:1371-1415`):**
   - If `m_Command.CmdKind == do_walk || m_Command.CmdKind == do_run` when `nAI == 0` (`m_ProcessAI == 0`), and the player is in the **last 3 frames** of a skill/attack/jump animation (`(m_Frames.nTotalFrame - m_Frames.nCurrentFrame) <= 3`), `ProcCommand(0)` returns early **without clearing `m_Command.CmdKind`**, preserving the buffered move command so `ProcCommand(1)` executes it as soon as `OnSkill()` finishes!
   - If the move arrives when the player has `> 3` frames remaining or is immobilized (`do_hurt`, `m_StunState`, `m_FreezeState`), the Server rejects the move and immediately sends `s2c_npcsetpos` (`NPC_PLAYER_TYPE_NORMAL_SYNC`) to `Player[m_nPlayerIdx].m_nNetConnectIdx` so the client snaps back to the authoritative server coordinate.
