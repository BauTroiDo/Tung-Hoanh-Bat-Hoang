# JX273 Issue #27 — Stage 3: Goddess login revision propagation to CoreServer (2026-10-25)

**State: STAGED / DRAFT. P0 NOT FIXED. NO PRODUCTION DEPLOYMENT.**

## Scope
Stage 1 opt-in atomic SQLite CAS and Stage 2 revision codec/snapshot were previously tested. This stage joins the **login/read side** of the actual Goddess -> Bishop -> GameServer -> KPlayer pipeline and prepares a session-bound revision token.

## Source changes (10 files)

- gateway/Sources/MultiServer/Goddess/sqlitel_db.h/.cpp:
  Add SelectBlobDataWithRevision() which converts the role name using the existing G2U/UTF-8 DB key, obtains StudentBlob+revision using LoadSnapshot() in one SQLite transaction, validates the capacity and only then copies the blob.
- gateway/Sources/MultiServer/Goddess/IDBRoleServer.h/.cpp:
  GetRoleInfo() gets an optional revision output pointer. The SQLite backend uses the snapshot loader when requested, while old Berkeley DB callers remain unchanged. Account and server validations are retained.
- gateway/Sources/MultiServer/Goddess/ClientNode.cpp:
  For a successful SQLite role load, reserve 24 trailer bytes before fetching the BLOB and append the JXREV001 role-bound revision extension to the return packet. Existing Bishop continues to forward its dataLength without interpreting the trailer.
- gameserver/Sources/MultiServer/GameServer/KSOServer.cpp:
  Bound-check the fully reassembled Bishop packet, verify TRoleData length and JXREV001 trailer, reject invalid/foreign trailers and bind validated revisions after AddCharacter.
- gameserver/Sources/Core/Src/KPlayer.h/.cpp:
  Hold the revision and loaded flag in the concrete KPlayer session, reset both in Release() to avoid stale numeric slot reuse.
- gameserver/Sources/Core/Src/CoreServerShell.h/.cpp:
  Expose session-aware set/get/confirm revision methods. Only after a WAL-approved, successful trade ACK may in-memory revisions for both verified participants advance. Duplicate/stale ACK does not advance the in-memory token.

## Build evidence
All four binaries built **from the Stage 3 QA source tree**, isolated under:
C:/Server-Game/SwordOnline/maintenance-builds/jx273_role_protocol_stage3_20261025/

- Goddess.JX273Stage3QA.exe (x64) — MSVC exit 0, 0 compile/link errors. Rebuilt after raw-encoding cleanup.
- Engine.dll and Engine.lib (x64) — MSVC exit 0, 0 errors; fresh Engine.lib exports __imp_?Utf8ToGbk@@YAPEBDPEBDPEADH@Z.
- CoreServer.dll and CoreServer.lib (x64) — MSVC exit 0, 0 errors after linking the fresh Engine.lib. Old, restored Engine.lib failed with LNK2019 because it lacked Utf8ToGbk; fixing the actual Engine dependency avoids stubbing Unicode conversion.
- gs001.exe (x64) — MSVC exit 0, 0 errors, linked against the fresh CoreServer.lib.
- Legacy warnings about unrelated code pages, runtime-library combinations and unavailable older PDBs remain.
- No post-build copy hooks were allowed. All temporary projects were archived under project-snapshots outside the source tree; original .vcxproj files unchanged.

## Regression tests
All three test EXEs were freshly compiled against this QA worktree and linked to the new CoreServer/Engine libraries; runtime copies used those exact newly built DLLs.

- TestPhase3FinalRuntime: 60/60 PASS, exit 0.
- TestPhase3PersistenceE2E: 27/27 PASS, exit 0.
- TestPhase3RuntimeVerified: 60/60 PASS, exit 0.
- TOTAL 147/147 PASS.
- Evidence: QA_results.json and QA/<suite>/run.log under the isolated output.
- Prior Stage 2 native codec/SQLite checks: 118/118 PASS (not a substitute for a full client login test).

## Blockers / DO NOT MERGE INTO A PRODUCTION DEPLOY

1. Goddess -> Bishop -> GameServer new revision trailer is compiled but **has not yet been exercised as a live network login with real serialized role data**. The old Bishop path forwards the payload but still needs real packet-level login/relog/transfer validation.
2. KSwordOnLineSever::SavePlayerData STILL sends the unversioned legacy role-save packet. Goddess ClientNode::_SaveRoleInfo STILL uses SaveRoleInfo/updateBlobData_n without CAS. A delayed legacy save CAN STILL overwrite a successful trade.
3. Implement versioned ordinary-save request+response with transaction-side CAS, an expected revision bound to role and session, in-flight save serialisation, and ACK token update only after successful commit. Legacy save must fail safely when trade-fenced without breaking legitimate progress on new clients.
4. Stale ACK, player index reuse, duplicate ACK and overlapping save/trade timing require end-to-end test. The new in-memory trade revision update follows WAL-accepted ACK but this stage has no real-client runtime acceptance for it.
5. Existing legacy-fenced roles with no revision row currently fail closed when using the new snapshot loader; define a safe migration/resynchronization path before enabling on production data.
6. Multi-GS TxID ordering, items/gold preservation, packet size ceiling and Berkeley DB crash recovery with real test roles remain open.
7. KPlayer and CoreServerShell layout/vtable changed. If eventually deployed, GameServer and CoreServer MUST be upgraded together, and the fresh compatible Engine.dll/import library must be included. Never replace only one binary.

**Disposition:** Keep GitHub PR #29 Draft and Issue #27 OPEN. No production engine/core/gameserver/goddess binaries installed, no player DB/config modified, no service restarted. Source files containing legacy GBK comments were preserved byte-wise to avoid unrelated encoding damage.
