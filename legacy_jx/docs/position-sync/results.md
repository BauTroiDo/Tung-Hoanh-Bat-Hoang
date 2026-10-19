# JX1 Position Synchronization — Implementation & Verification Results (`results.md`)

**Date:** 2026-10-17  
**Build Target:** Visual Studio 2022 BuildTools MSBuild x64 (`Server Release` / `Release`)  
**Verification Status:** `SOURCE_REVIEWED` + `BUILD_VERIFIED` + `UNIT_TEST_VERIFIED`

---

## 1. Modified Files & Backups

### Server (`C:\Server-Game\SwordOnline\jxOnline` & synced to `C:\Server-Game\SwordOnline\jxOnline-r78`)
1. [`gameserver/Sources/Core/Src/KNpcFindPath.cpp`](file:///c:/Server-Game/SwordOnline/jxOnline-r78/gameserver/Sources/Core/Src/KNpcFindPath.cpp) (Backup: `KNpcFindPath.cpp.pos_sync.bak`)
   - Re-enabled `m_nFindTimes > 1` and `m_nFindTimer >= m_nMaxTimeLong` obstacle pathfinding limits behind `g_bPositionSyncFixEnabled` to match `Client/Classes/gamecore/KNpcFindPath.cpp` 1:1.
2. [`gameserver/Sources/Core/Src/KNpc.cpp`](file:///c:/Server-Game/SwordOnline/jxOnline-r78/gameserver/Sources/Core/Src/KNpc.cpp) (Backup: `KNpc.cpp.pos_sync.bak`)
   - Defined `BOOL g_bPositionSyncFixEnabled = TRUE;`.
   - Re-enabled `s2c_npcstand` (`NPC_PLAYER_TYPE_NORMAL_SYNC`) broadcast in `KNpc::DoStand()` with strict region bounds checks (`0 <= nConRegion < m_nTotalRegion`).
   - Added early barrier-stop detection (`bStoppedByBarrier`) in `KNpc::ServeMove()` that broadcasts authoritative `s2c_npcsetpos` (`NPC_PLAYER_TYPE_NORMAL_SYNC`) when an entity stops before reaching `(m_DesX, m_DesY)`.
   - Added 3-frame move command buffering in `KNpc::ProcCommand(0)` for players finishing skill/attack/jump animations, and authoritative `s2c_npcsetpos` reconciliation when moves are rejected during immobilization.
   - Fixed all 6 occurrences of `nConRegion > SubWorld[m_SubWorldIndex].m_nTotalRegion` to `>=`, added upper-bound checks in `DoHurt()`, and added region guards to `RunTo()` and `Goto()`.
   - Fortified `KNpc::SetPos()` to zero-initialize `sSync`, send `s2c_npcsetpos` directly to self player (`PackDataToClient`) as well as `SendDataToNearRegion`, and broadcast `s2c_npcsetpos` for non-player NPCs.
3. [`gameserver/Sources/Core/Src/KProtocolProcess.cpp`](file:///c:/Server-Game/SwordOnline/jxOnline-r78/gameserver/Sources/Core/Src/KProtocolProcess.cpp) (Backup: `KProtocolProcess.cpp.pos_sync.bak`)
   - Fortified `NpcWalkCommand` and `NpcRunCommand` with player/NPC index bounds checks and immediate authoritative `s2c_npcsetpos` reconciliation when a player sends walk/run commands while stunned (`m_StunState.nTime > 0`), frozen (`m_FreezeState.nTime & 1`), confused (`m_randmove.nTime > 0`), or in `do_hurt`.
4. [`gameserver/Sources/Core/Src/KCore.cpp`](file:///c:/Server-Game/SwordOnline/jxOnline-r78/gameserver/Sources/Core/Src/KCore.cpp) (Backup: `KCore.cpp.pos_sync.bak`)
   - Added runtime INI configuration toggle `g_GameSetTing.GetInteger("Gameconfig", "PositionSyncFix", 1, &g_bPositionSyncFixEnabled);` (defaults to `1` = enabled; set `PositionSyncFix=0` under `[Gameconfig]` in `gamesetting.ini` to disable without recompiling).

### Client (`C:\Server-Game\SwordOnline\Client\Classes\gamecore`)
5. [`Client/Classes/gamecore/KProtocolProcess.cpp`](file:///c:/Server-Game/SwordOnline/Client/Classes/gamecore/KProtocolProcess.cpp) (Backup: `KProtocolProcess.cpp.pos_sync.bak`)
   - Fixed `NetCommandStand` (`s2c_npcstand`): changed `if (nRegion > 0)` to `if (nRegion >= 0)` (fixing Region 0), guarded `NpcChangeRegion` so it only runs when `nOldRegion != nRegion` with the valid source `m_RegionID`, and added coordinate reconciliation for non-self entities when drifted `>= 64` MPS.
   - Fixed `SyncNpcMinPlayer` (`s2c_syncnpcminplayer`): added same-grid (`nRegion >= 0 && m_RegionIndex >= 0`) drift reconciliation when distance exceeds `160` MPS (standing/sitting) or `384` MPS (moving).

---

## 2. Build & Test Verification Results

| Check | Command / Target | Result | Classification |
|---|---|---|---|
| Server Core DLL Build | `MSBuild.exe Core.vcxproj /p:Configuration="Server Release" /p:Platform=x64` | **0 Error(s)** (`CoreServer.dll`, `12,128,256` bytes) | `BUILD_VERIFIED` |
| GameServer EXE Build | `MSBuild.exe GameServer.vcxproj /p:Configuration="Release" /p:Platform=x64` | **0 Error(s)** (`gs001.exe`, `1,363,456` bytes) | `BUILD_VERIFIED` |
| Deterministic Test Suite | `python tests/position_sync_test.py` (7 test cases) | **7 / 7 PASS** (`0.009s`) | `UNIT_TEST_VERIFIED` |

---

## 3. Rollback Procedure

1. **Runtime Config Rollback (No Rebuild Required):**
   - Set `PositionSyncFix=0` under `[Gameconfig]` in `gamesetting.ini` and restart `gs001.exe`.
2. **Source & Binary Rollback:**
   - Restore `.pos_sync.bak` files in `gameserver/Sources/Core/Src/` (`KNpc.cpp.pos_sync.bak`, `KNpcFindPath.cpp.pos_sync.bak`, `KProtocolProcess.cpp.pos_sync.bak`, `KCore.cpp.pos_sync.bak`) and `Client/Classes/gamecore/KProtocolProcess.cpp.pos_sync.bak`, then rebuild `Core.vcxproj`.
