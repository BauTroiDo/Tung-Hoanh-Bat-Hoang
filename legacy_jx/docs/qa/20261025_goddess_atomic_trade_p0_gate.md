# Goddess / GameServer atomic-trade gate ? 2026-10-25

## Isolated builds and safety
- Branch: checkpoint/jxonline-273-triage-20261024; parent HEAD 6bd068b.
- Fresh CoreServer.dll SHA256: EE581FCA6CE470D4BE1353FBF2A17EA07B6D660DB3A0719610D1845CB1CDCA9E.
- Goddess x64 QA executable SHA256: 23592E181957F61A81E00F0D91C916D6511A9B6D099858E80A45E2A9A304A9ED.
- gs001 x64 QA executable SHA256: D2083C3521B7E3567EBF6CD6498CAAB7986C5D591B3DFB09DBD7A933B2A3EC6A.
- gs001 QA binary linked to freshly built CoreServer.lib instead of old source-library copy.
- Outputs isolated under C:/Server-Game/SwordOnline/maintenance-builds; production binaries and character databases untouched.
- Original project files unchanged; temporary QA projects removed; no restart, merge, push, or live network interaction.

## Native request / result ABI
- Gateway/GameServer definitions are byte-identical:
  TTradePairSaveRequest packed size 106, payload offset 105;
  TTradePairSaveResult packed size 85.
- Sender: KPlayerTradeJournal::DispatchTradePairToGoddess.
- Goddess: CClientNode::_SaveTradePair -> SaveTradePairAtomic -> sqlitel_db::updateBlobData_atomic_dual_tx -> SendTradePairResult.
- Receiver: KSOServer s2c_roleserver_savetradepair_result -> CoreServerShell::OnTradePairSaveResult.
- Evidence: maintenance-builds/jxonline273_goddess_20261025/packet-abi-parity.json.

## Native QA results
- Fresh-CoreServer test doubles: 60/60 FinalRuntime, 27/27 PersistenceE2E, 60/60 RuntimeVerified; total 147/147 PASS.
- MSVC x64 compiled the exact, unmodified 240-line extract of six Goddess C++ SQLite methods with a minimal fixture class, ASCII-only role name stub, and real SQLite library.
- Atomic dual save, rollback, TxID duplicate/mismatch CRC checks, stale nonzero generation, reopen/replay: 18/18 PASS.
- Separate reproduction: 22/22 behavioral assertions PASS. These include CONFIRMING the P0 failure, not a green release gate:
  (1) Existing ekey=5; a new transaction with generation=1 returns -14 and cannot save two roles.
  (2) Existing ekey=5; a transaction with generation=0 commits successfully and overwrites both blobs and emits receipt.
- Native harness source and logs: maintenance-builds/jxonline273_goddess_20261025/native_sqlite.
- Neither QA suite exercised a live network pair.

## P0: durable role generation is missing across re-login
- KPlayer::m_uSaveGeneration declared in KPlayer.h is incremented in KPlayer.cpp on trade.
- Source audit found no initialization or hydration from the stored SQLite role_Blob.ekey. KPlayer constructor calls Release(), but Release() does not assign this field.
- Sender substitutes generation 0 with 1. Thus a player returning after an earlier trade with ekey>1 may receive -14.
- Goddess updateBlobData_n uses (? = 0 OR ekey IS NULL OR ekey <= ?); zero bypasses the stale-write fence if a zero-generation packet arrives.
- Do NOT disable ekey validation or send generation 0 as a workaround. That could permit old WAL snapshots to overwrite newer character inventory/gold.

## Must-pass gates before production
1. Define persistent, monotonic role generation owned by authoritative database and load it into every player session. Initialize the in-memory field safely without resetting persistent sequence.
2. Audit compatibility requirements for zero-generation legacy saves and enforce nonzero versioning for the trade packet path after proper migration.
3. Cross-relog tests after ekey=5: next trade must commit with newer generation; replay of older trade must reject, retain both player blobs and receipt consistency.
4. Test success/negative ACK/timeouts/crash replay, no duplication of gold and items.
5. Run real local GameServer/Goddess executables only with separate QA ports/config and disposable DB, then validate wire ACK and packet framing.
6. Keep master and ServerNew/_bin_v2_ unchanged until all gates pass.

## Warnings / limitations
- Compiler warnings remain in legacy sources.
- Packet ABI and method-level SQL tests are not equivalent to native live TCP end-to-end.
- No source changes were made to the production generation logic.
