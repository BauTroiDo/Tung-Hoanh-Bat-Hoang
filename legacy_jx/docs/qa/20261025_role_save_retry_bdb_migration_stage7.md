# JX273 Issue #27 — Stage 7: immutable save-frame retry + safe BDB import (2026-10-25)

**Disposition: QA IMPLEMENTED and TESTED; production release NOT approved. Keep Draft PR #29; Issue #27 remains P0 OPEN.**

## Implementation

1. gameserver/Headers/RoleSaveRetryCache.h (new): deep-copies the exact emitted Rainbow transport frames, role identity, expected revision, random nonce and send tick into a bounded per-player cache. Enforces a 256 KiB maximum and per-frame ceiling; comparison binds slot, role, revision and nonce. Handles tick wrap and duplicate/stale ACKs.
2. gameserver/Sources/Core/Src/CoreServerShell.h/.cpp: GetPendingRoleSave() exposes only the current, live KPlayer pending identity/token to its owning GS. It does not create new tokens.
3. gameserver/Sources/MultiServer/GameServer/KSOServer.h/.cpp: initial versioned save now caches framed bytes before transmission; the ACK path verifies both the GS frame cache and live Core KPlayer session before accepting a Goddess revisioned ACK. RetryPendingRoleSaves() retransmits **the original frame bytes** after 5 seconds without ACK, up to 3 times. After exhaustion it fail-closes the session for authoritative relog/resync, never generates another nonce or downgrades to legacy save. It drops cached data on logout/slot reuse and on matching ACK.
4. gateway/Sources/MultiServer/Goddess/SafeBdbRoleImport.h (new): create-only, BEGIN IMMEDIATE / transactional SQLite BDB import with parameterized names and guards against existing characters, trade fences and revisions. No deletion or overwriting of existing SQLite role_Blob rows.
5. gateway/Sources/MultiServer/Goddess/sqlitel_db.cpp: replaces the destructive delete/insert updateBlobData() maintenance writer with guarded create-only import; return values now indicate actual insert versus skipped/refused. Gameplay ordinary saves still use Stage 4/5 CAS and receipts.
6. gateway/Sources/MultiServer/Goddess/IDBRoleServer.cpp: BDB copy workflow reports skipped records rather than reporting a false successful replacement; returns partial-failure status when skips occur.
7. tests/native/role_save_retry_cache_qa.cpp and tests/native/safe_bdb_import_qa.cpp: native C++ QA tests.

All legacy CP936/GBK files patched using raw-byte replacement, keeping unrelated bytes unchanged.

## Verification with isolated x64 binaries

- CoreServer.dll: MSVC x64 Rebuild PASS, zero compiler/link errors.
- gs001.exe: MSVC x64 Build PASS, zero compiler/link errors. First attempt detected an escaped-newline C2001 log typo, corrected byte-safely before final successful build.
- Goddess.JX273Stage7Loopback.exe: MSVC x64 Rebuild PASS, zero compiler/link errors, loopback bind only to 127.0.0.1:15001; postbuild deployment disabled.
- Core Phase3 FinalRuntime 60/60, PersistenceE2E 27/27, RuntimeVerified 60/60. **147/147 PASS** against newly rebuilt CoreServer.dll and verified Engine.dll.
- Native immutable retry cache: **41/41 PASS** (wrong role/slot/nonce, byte-exact deep copy, three retries, tick wrap and memory caps).
- Native SQLite BDB create-only migration: **43/43 PASS**, including SQL quoting, blob with embedded null, existing ID unchanged, orphan fence/revision, two-connection race with fresh transaction, restart.
- Rainbow against new Goddess QA and disposable synthetic character roles: versioned save **5/5 PASS** (post-trade progress, nonce replay, collision, stale revision, valid next save) plus **2/2 PASS** after Goddess restart.
- Final SQLite integrity_check = ok, QA_A revision 4 (gold 72, 2 save receipts), QA_B revision 2 (gold 230); no duplicated receipts or asset changes.
- Evidence root: C:/Server-Game/SwordOnline/maintenance-builds/jx273_role_protocol_stage7_20261025. QA database, binaries, and disposable .vcxproj are NOT committed to Git.

## P0 release blockers — do not merge/deploy yet

- The new GS/CS↔Goddess retry path has not been exercised in a *fully integrated* login/save/ACK lifecycle with a real loaded character/inventory. A standalone retry-cache test and direct Goddess Rainbow test do not prove the complete chain.
- On retry exhaustion the KPlayer token becomes unusable and an authoritative relog/resync is **required**, but an automatic user-facing relog/resynchronization handshake has not been implemented. The safe behavior is to block further versioned saves, not silently continue.
- Test with two simultaneous game-server processes, GS crash at every save/trade phase, delayed/out-of-order ACK after slot reuse, transfers/disconnects, persisted equipped/item/gold invariants.
- Legacy fenced characters with absent revision row still fail closed and need a controlled migration path. DO NOT invent their revision as zero or reset player data.
- The separate intentional role-deletion API uses deleteBlobData() and still needs authorization/receipt/fence lifecycle acceptance. It is NOT used by the new BDB copy workflow. Other admin/destructive maintenance commands require review before production use.
- This Stage 7 is a source checkpoint only; no live player DB, production config or binary has been modified, and no production service was started.

## Prior checkpoints

Stage 1–6 source and tests are on Draft PR #29. Stage 6 CoreServer/GS save protocol implemented; Stage 5 Goddess durable CAS+nonce receipt implemented; Stage 4 legacy normal save guarded after committed trade. The current stage closes the in-memory immutable retry and migration-overwrite gaps, but **not the total P0 acceptance**.
