# JX1 Position Synchronization — Wire Protocol Matrix (`protocol-matrix.md`)

**Date:** 2026-10-17  
**Packing:** `#pragma pack(push, 1)` across `gameserver/Headers/KProtocol.h` and `Client/Classes/gamecore/KProtocol.h`  
**Verification Status:** `SOURCE_REVIEWED` + `UNIT_TEST_VERIFIED`

---

## 1. Client-to-Server (C2S) Movement Packets

| Protocol Enum | Struct Name | Size (Bytes) | Wire Fields (`#pragma pack(1)`) | Coordinate Semantics | Server Handler |
|---|---|---|---|---|---|
| `c2s_npcwalk` | `NPC_WALK_COMMAND` | `9` | `BYTE ProtocolType` (1)<br>`int nMpsX` (4)<br>`int nMpsY` (4) | **Destination intent** `(nMpsX, nMpsY)` in Map Pixel Space (MPS). Does **not** carry current position. | `KProtocolProcess::NpcWalkCommand` (`KProtocolProcess.cpp:5436`) |
| `c2s_npcrun` | `NPC_RUN_COMMAND` | `13` | `BYTE ProtocolType` (1)<br>`int nMpsX` (4)<br>`int nMpsY` (4)<br>`int nMapID` (4) | **Destination intent** `(nMpsX, nMpsY)` in MPS + `nMapID`. Does **not** carry current position. | `KProtocolProcess::NpcRunCommand` (`KProtocolProcess.cpp:5452`) |
| `c2s_npcsunyi` | `NPC_SUNYI_COMMAND` | `21` | `BYTE ProtocolType` (1)<br>`int nMap` (4)<br>`int nMpsX` (4)<br>`int nMpsY` (4)<br>`int nMapID` (4)<br>`DWORD nDwid` (4) | Instant teleport request `(nMap, nMpsX, nMpsY)`. | `KProtocolProcess::NpcSunyiCommand` (`KProtocolProcess.cpp:5485`) |

---

## 2. Server-to-Client (S2C) Movement & Position Reconciliation Packets

| Protocol Enum | Struct Name | Size (Bytes) | Wire Fields (`#pragma pack(1)`) | Coordinate Semantics | Server Sender | Client Handler |
|---|---|---|---|---|---|---|
| `s2c_npcwalk` | `NPC_WALK_SYNC` | `13` | `BYTE ProtocolType` (1)<br>`UINT ID` (4)<br>`int nMpsX` (4)<br>`int nMpsY` (4) | **Destination** `(nMpsX, nMpsY)` in MPS. Client pathfinds locally from its current coordinate toward `(nMpsX, nMpsY)`. | `KNpc::Goto` (`KNpc.cpp:4275`) | `KProtocolProcess::NetCommandWalk` (`KProtocolProcess.cpp:881`) |
| `s2c_npcrun` | `NPC_RUN_SYNC` | `13` | `BYTE ProtocolType` (1)<br>`UINT ID` (4)<br>`int nMpsX` (4)<br>`int nMpsY` (4) | **Destination** `(nMpsX, nMpsY)` in MPS. Client pathfinds locally toward `(nMpsX, nMpsY)`. | `KNpc::RunTo` (`KNpc.cpp:4224`) | `KProtocolProcess::NetCommandRun` (`KProtocolProcess.cpp:645`) |
| `s2c_npcjump` | `NPC_JUMP_SYNC` | `13` | `BYTE ProtocolType` (1)<br>`UINT ID` (4)<br>`int nMpsX` (4)<br>`int nMpsY` (4) | Jump **destination** `(nMpsX, nMpsY)` in MPS. | `KNpc::DoJump` (`KNpc.cpp:6812`) | `KProtocolProcess::NetCommandJump` (`KProtocolProcess.cpp:598`) |
| `s2c_npchurt` | `NPC_HURT_SYNC` | `17` | `BYTE ProtocolType` (1)<br>`UINT ID` (4)<br>`int nFrames` (4)<br>`int nX` (4)<br>`int nY` (4) | **Authoritative current MPS position** `(nX, nY)` at start of hurt + `nFrames` duration. | `KNpc::DoHurt` (`KNpc.cpp:3028`) | `KProtocolProcess::NetCommandHurt` (`KProtocolProcess.cpp:610`) |
| `s2c_npcstand` | `NPC_PLAYER_TYPE_NORMAL_SYNC` | `25` | `BYTE ProtocolType` (1)<br>`UINT m_dwNpcID` (4)<br>`int m_dwMapX` (4)<br>`int m_dwMapY` (4)<br>`int m_wOffX` (4)<br>`int m_wOffY` (4)<br>`UINT m_GameLeftTiem` (4) | **Authoritative stop position**: `m_dwMapX/Y` = MPS `(nMpsX, nMpsY)`, `m_wOffX/Y` = Q10 sub-cell offsets `(0..32767)`. | `KNpc::DoStand` (`KNpc.cpp:3306`) | `KProtocolProcess::NetCommandStand` (`KProtocolProcess.cpp:678`) |
| `s2c_npcsetpos` | `NPC_PLAYER_TYPE_NORMAL_SYNC` | `25` | Same 25-byte layout as above | **Authoritative hard snap + stand**: `m_dwMapX/Y` = MPS `(nMpsX, nMpsY)`, `m_wOffX/Y` = Q10 sub-cell offsets. Works for self, other players, and NPCs across all regions `0..8`. | `KNpc::SetPos`, `KNpc::ServeMove` (barrier stop), `KNpc::ProcCommand(0)`, `NpcWalk/RunCommand` | `KProtocolProcess::s2cnpcsetpos` (`KProtocolProcess.cpp:2311`) |
| `s2c_syncnpcminplayer` | `NPC_PLAYER_TYPE_NORMAL_SYNC` | `25` | Same 25-byte layout as above | **Authoritative periodic self-player snapshot** sent in `KNpc::NormalSync()`. | `KNpc::NormalSync` (`KNpc.cpp:8247`) | `KProtocolProcess::SyncNpcMinPlayer` (`KProtocolProcess.cpp:2637`) |
| `s2c_syncnpcmin` | `NPC_NORMAL_SYNC` | Fixed packed | Includes `ID`, `MapX` (MPS X), `MapY` (MPS Y), `m_wOffX`, `m_wOffY`, `Doing`, speeds, states | **Authoritative periodic NPC / other-player snapshot**. | `KNpc::NormalSync` (`KNpc.cpp:7900`) | `KProtocolProcess::SyncNpcMin` (`KProtocolProcess.cpp:2415`) |

---

## 3. Coordinate Representation Invariants

1. **Grid Hierarchy:**
   - Cell dimensions: `m_nCellWidth = 32`, `m_nCellHeight = 32` MPS units.
   - Region dimensions: `m_nRegionWidth = 16` cells (`512` MPS), `m_nRegionHeight = 32` cells (`1024` MPS).
   - Sub-cell fixed-point offset: `CELLWIDTH = 32 << 10 = 32768`, `CELLHEIGHT = 32 << 10 = 32768`. `m_OffX` and `m_OffY` are stored in Q10 fixed-point (`0 .. 32767`).
2. **Conversion Formulas (`KSubWorld::NewMap2Mps` & `Mps2Map`):**
   - `MpsX = RegionOriginX + m_MapX * 32 + (m_OffX >> 10)`
   - `MpsY = RegionOriginY + m_MapY * 32 + (m_OffY >> 10)`
   - Notice that `NPC_PLAYER_TYPE_NORMAL_SYNC` carries **both** `m_dwMapX / m_dwMapY` (integer MPS coordinates) **and** `m_wOffX / m_wOffY` (exact Q10 sub-cell offsets).
