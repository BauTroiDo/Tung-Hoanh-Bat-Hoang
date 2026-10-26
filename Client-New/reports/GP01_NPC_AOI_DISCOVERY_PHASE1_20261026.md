# GP-01 Phase 1 — NPC/AOI native4016 ↔ original GS
**2026-10-26. Result: code + unit tests PASS; NOT enabled in running gateway; NPC rendering NOT accepted.**

## Protocol evidence
- Native client c2s_requestnpc = opcode 92, 5B: 1B opcode + uint32 ID.
- Legacy GS c2s_requestnpc = opcode 73, NPC_REQUEST_COMMAND 69B: 1B opcode + uint32 ID + 64B reserved name field. Source: KProtocol.h, KProtocol.cpp, KProtocolProcess.cpp::NpcRequestCommand → NpcSet.SyncNpc.
- GS authoritative NPC_NORMAL_SYNC=77, NPC_SYNC=76, NPC_REQUEST_FAIL=155, NPC_REMOVE_SYNC=84. Native NPC_MIN=84, NPC_SYNC=98 per 4016 protocol table.
- Existing gs_npc_status_merge.py and gs_world_map53_name_preview.py explicitly mark native full NPC projection production_ready=False, due to missing fields. NO speculative native NPC spawn emitted.

## New DEV source, guarded off by default
- C:\Server-Game\SwordOnline\Client3D-LAN\gs-bridge-m1\native_npc_aoi.py
- C:\Server-Game\SwordOnline\Client3D-LAN\gs-bridge-m1\test_native_npc_aoi.py
- Modified C:\Server-Game\SwordOnline\Client3D-LAN\gs-bridge-m1\native_session.py with opt-in environment variable GP01_NPC_DISCOVERY_ENABLED=1 (default 0).
- Backup: gs-bridge-m1\native_session.py.bak_pre_gp01_20261026; SHA256 of original 52EA5CDC218477BCF0B9087B112827ADA5010ADC7E7F696A29990ED4EA1AC971.
- Bounded GS-NPC discovery: only real GS-sourced nearby NPC IDs, distance threshold 3072 legacy units, max 48 per session, 4 requests/second, dedup and removal; full GS metadata collected with valid IDs and packet framing. Native client requests are accepted only if GS previously announced that ID. Unknown/own/zero/malformed/out-of-range ID rejected. No fabricated NPCs, fake positions, unverified native packets, or database modifications.
- Already-running gateway PID 3828 not restarted / not hot reloaded, thus behavior is unchanged in current session. GS PID 5036 was not restarted.

## Tests actually run
- py_compile three GP01 files: exit 0.
- python -m unittest discover -s gs-bridge-m1 -p 'test_*.py' -q from Client3D-LAN: **171/171 PASS, exit 0** (14 new GP01 tests).

## Next blocking gate
1. Verify exact native4016 NPC_MIN(84), NPC_SYNC(98), NPC_REMOVE and interaction consumption layouts through real IL2CPP/GameServer packet evidence; resolve missing required fields. Do not change original 4016 binary.
2. Implement structurally complete GS→native NPC create/update/remove converter, cross-region AOI lifecycle, reconnect, map ownership, malformed packet and replay tests.
3. Isolated canary with explicit GP01 discovery opt-in; validate real server NPC IDs, model mapping, click and server dialogue, no regressions movement/collision; backup and rollback before DEV activation.
4. Only then enable the feature in shared DEV gateway and claim GP01 PASS. Current status **GP01 PARTIAL / rendering BLOCKED**, CN02-04 BLOCKED, CN03 CLOSED; global collision still not accepted.
