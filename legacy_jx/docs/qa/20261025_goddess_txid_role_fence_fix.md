# Goddess x64 ? P0 trade replay/relog fix (QA checkpoint 2026-10-25)

## Summary

**QA source implementation completed; NOT YET production accepted or deployed.**
The previous P0 analysis discovered that SQLite role_Blob.ekey is a legacy
FileNameDB(roleName) key written by InsertBlobData/updateBlobData, not a
durable player-trade generation. Seeding KPlayer::m_uSaveGeneration from ekey
would have corrupted old data semantics and can wrap 32-bit counters.

Instead, transactional ordering uses the journal's durable Snowflake-style
64-bit TxID, with a separate SQLite trade_role_fence table per role.
Legacy role_Blob.ekey is never modified by this new atomic trade path.

## Changes

1. gateway/Sources/MultiServer/Goddess/sqlitel_db.cpp
   - ensureTradeReceiptTable now also creates trade_role_fence(role_name,last_tx_id).
   - A new atomic trade checks BOTH roles' last_tx_id before changing either
     character. Older *distinct* TxIDs return -16, with SQLite rollback.
   - For a role without a fence row (pre-upgrade receipts), consult the
     committed trade_receipts history and reject older TxIDs; this occurs only
     when that role has not yet received a new fence row.
   - Update two StudentBlob values without rewriting legacy ekey, record
     trade_receipts and both per-role fences under one IMMEDIATE transaction.
   - Existing same-TxID matching receipt returns idempotent success;
     different CRC/roles with same ID returns collision error -13.
   - Reject invalid/zero TxIDs and generation 0 before SQLite writes (-15).
   - Do not ACK an idempotent replay as successful if SQL COMMIT fails.
2. gateway/Sources/MultiServer/Goddess/ClientNode.cpp
   - Reject malformed/empty/non-terminated/same role names and
     zero-generation/zero-TxID atomic trade requests (-15).
3. gameserver/Sources/Core/Src/KPlayer.cpp
   - Initialize in-memory m_uSaveGeneration=0 in Release() to remove UB.
     This counter is no longer the authoritative database trade version.
4. gameserver/Sources/Core/Src/KPlayerTradeJournal.cpp
   - Initialize reconstructed legacy WAL entry generations to 1 so replay
     never transmits uninitialized memory.
   - The durable WAL TxID, not those counters, is the ordering authority.
   - This file also contained earlier uncommitted journal length-window
     hardening; checkpointing it includes those existing changes.

The sqlitel_db.cpp file likewise already contained the uncommitted atomic
transaction source from the preceding Phase 3 work. This checkpoint
preserves and integrates that earlier implementation rather than assuming
its entire diff was written in this one turn. Raw source snapshots are
under maintenance-backups/jxonline-273-audit-20261024.

## Build outputs (isolated, never deployed)

- CoreServer x64 Rebuild: exit=0; no compiler/link errors.
  - C:/Server-Game/SwordOnline/maintenance-builds/jxonline273_fullcore_p0_20261025/bin/CoreServer.dll
  - SHA256 050BCE06B28558CC4C18A3A3208723903FE5DBD5F03611E417DE7A14FC95A921
- Goddess x64 Rebuild then Build after final source edit: exit=0; no compiler/link errors.
  - C:/Server-Game/SwordOnline/maintenance-builds/jxonline273_goddess_p0_20261025/bin/Goddess.JX273GBKAudit.exe
  - SHA256 8078439EC9FFA29B51C86D1049F3522D67883F1A5A82FED1C2D47C0C036976BC
- Temporary QA .vcxproj files disable PostBuildEvent copying into the repo
  and use external OutDir/IntDir/PCH. Source project files are unchanged.

## Executed tests

- **Goddess native SQLite, 53/53 PASS**, exit=0:
  - Compiled exact, byte-identical source of 6 Goddess SQLite methods and
    the newly added ordering helpers, linked to real sqlite3.dll in x64.
  - Tests commit, missing-role rollback, CRC mismatch/TxID collision,
    matching receipt replay, older delayed transactions, reopen/restart,
    relog after legacy ekey values, zero generation, same role, migration
    from receipts without fence, and a third participant cross-role fence.
  - Fixture source: maintenance-builds/jxonline273_goddess_p0_20261025/native_sqlite/goddess_txid_fence_gbk_qa.cpp
  - Evidence: maintenance-builds/jxonline273_goddess_p0_20261025/native_sqlite/gbk-native-test.log
- **Fresh CoreServer QA 147/147 PASS**, all exit=0:
  - FinalRuntime 60/60
  - PersistenceE2E 27/27
  - RuntimeVerified 60/60
  - All 3 suites relinked using new CoreServer.lib, and each test process
    loaded the SHA256-verified freshly compiled CoreServer.dll.
  - Evidence: maintenance-builds/jxonline273_fullcore_p0_20261025/qa/core-p0-final-results.json
- Total 200/200 case assertions passed across separate QA suites. They are
  neither one integrated executable nor a true network packet/ACK test.
- The previous P0 failing scenario (legacy ekey=5, new generation=1 -> -14)
  is addressed at the SQLite-method level by no longer comparing trade
  generation to ekey. Tests preserve large legacy ekey values exactly.

## Remaining release gates / risks

1. Run REAL isolated network GameServer ? Goddess processes on QA ports,
   with disposable role SQLite DB and no connections to production Bishop,
   Goddess, account store or gs001. Check success/negative ACK/timeout/replay.
2. Verify end-to-end login, trade, relog and reconnect with two QA characters;
   inspect actual gold/items counts, scene synchronization, client unlock
   and SQLite receipt/fence values.
3. Snowflake TxID ordering is time based with process-identity bits. Across
   clock rollback, non-monotonic multi-server timestamps or ID collisions,
   DB deliberately fails closed (-16/-13), which may block valid trades.
   Define a durable allocator or clock/instance discipline before release.
4. Audit non-trade legacy saves: updateBlobData still deletes/reinserts the
   role blob with legacy ekey and is not fenced by trade_role_fence. Ensure
   old asynchronous saves cannot overwrite newer committed trades.
5. Native SQL fixture now tests both ASCII and GBK CP936 role names with
   UTF-8 role_Blob keys and legacy raw-byte receipts. Real Goddess Windows
   ACP must still be verified in an isolated executable/network test.
6. Existing binary WIP and unrelated Git dirty files remain; no
   deployment or merge until full integration is proven.

**Live system preserved:** no service restart, no production DB writes, no
runtime executable replacement, no master merge and no Git push.


## Follow-up: GBK/UTF-8 migration correctness

The role_Blob key and trade_role_fence key are stored in UTF-8 via G2U,
while trade_receipts stores protocol names as original raw (GBK/CP936)
bytes. The receipt-history fallback must compare raw protocol names, not
the converted UTF-8 key. The new helpers also free G2U owned buffers
rather than leaking allocations on each trade.

Native x64 QA with genuine SQLite and a CP936-to-UTF8 test converter
verified 53/53 assertions (exit 0). This includes a GBK-named character,
old receipts, missing-fence migration and replay rejection, plus unchanged
legacy ekey. The last Goddess x64 rebuild used the encoding-corrected
source and returned exit code 0 without C/LNK errors.
