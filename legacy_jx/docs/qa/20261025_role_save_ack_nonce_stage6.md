# JX273 Issue #27 — Stage 6: revisioned GameServer saves and nonce-bound ACK (2026-10-25)

**Status: ISOLATED QA BUILD/REGRESSION PASS. NOT production-ready.** Keep Draft PR #29; Issue #27 P0 OPEN.

## Changes
- KPlayer.h/KPlayer.cpp: persist role revision, protocol-seen flag, pending nonce, expected revision and bound pending role name per live KPlayer. All reset on Release().
- CoreServerShell.h/CoreServerShell.cpp: add IsRoleSaveRevisionRequired(), BeginRoleSave(), FinishRoleSave(). Generate randomized 64-bit nonce via CoCreateGuid(), only one outstanding versioned save at a time. Finish accepts an ACK only for the matching expected revision, nonce, pending role name and live session. Duplicate/mismatched ACK is ignored; NACK or trade/save collision forces resync without silent fallback to legacy.
- KSOServer.cpp: append the existing Stage 4 JXREV001 36-byte save extension to opcode 10 on revision-capable sessions, and recognize Goddess opcode 56 marker 791031 with status, expected revision, committed revision and nonce. Only accepted ACK is permitted to trigger transfer/logout/SAVE_IDLE. A legacy ACK marker 791030 is refused for versioned sessions.
- tests/native/legacy_role_save_guard_qa.cpp: explicitly set WAL in the two-connection QA test to avoid rollback journal shared-lock interference with the first COMMIT. This is test-only; not a production journal mode change.

## Source safety
Legacy source uses CP936/GBK comments. Editing through ordinary text APIs initially recoded some unrelated lines. The five altered source files were restored byte-for-byte from HEAD, then only ASCII patches were reapplied through a byte-preserving PowerShell procedure. This restored the original encoding and produced a small diff. The prior edited files are archived in maintenance-builds, outside Git.

## Builds and tests
- Isolated MSVC x64 CoreServer.dll Rebuild PASS, zero compiler/link errors.
- Isolated MSVC x64 gs001.exe Rebuild PASS, zero compiler/link errors.
- Fresh binaries compiled from this QA source; CoreServer test runtime used the newly built CoreServer.dll and validated Engine.dll.
- Phase3 FinalRuntime 60/60 PASS; PersistenceE2E 27/27 PASS; RuntimeVerified 60/60 PASS. Total 147/147.
- Native Stage 5 receipt tests: 34/34 PASS.
- Save wire native codec Gateway 19/19 PASS, GameServer 19/19 PASS, 10,000 round trips each.
- SQLite legacy guard two-connection WAL fixture 38/38 PASS in three fresh independent runs. The first regression attempt, before fixture WAL normalization, returned SQLITE_BUSY at COMMIT in the rollback-journal lock race; this failure was recorded and not attributed to the Stage 6 implementation.
- Goddess Stage 5 network QA from the earlier checkpoint was 5/5 versioned saves and 2/2 replay/restart; it has not been rerun against live integrated GameServer in Stage 6.

QA output: C:\Server-Game\SwordOnline\maintenance-builds\jx273_role_protocol_stage6_20261025

## Remaining P0 blockers — DO NOT DEPLOY
1. The newly wired CoreServer/GameServer save/ACK path has not been run end-to-end with a real logged-in player and Goddess. Builds plus general Core regressions do not validate that entire flow.
2. A lost ACK leaves the per-role save pending as a fail-closed protection. Immutable packet replay using the exact same nonce and serialized bytes, bounded retry, and fallback resync/relog still need implementation.
3. Simultaneous trade/save, transfer/disconnect, player slot reuse and two-GS scenarios still require runtime acceptance. Trade ACK during a pending save invalidates the in-memory token rather than silently incrementing it.
4. Audit BDB→SQLite migration writer updateBlobData(), controlled migration for old fenced roles lacking a revision token, maximum inventory packet capacity and BDB crash recovery.
5. End-to-end two-client item+gold conservation and durable trading still unaccepted.

**Release decision:** Keep Draft PR #29, Issue #27 OPEN; no production binary/config/database changes, no service restart or merge.
