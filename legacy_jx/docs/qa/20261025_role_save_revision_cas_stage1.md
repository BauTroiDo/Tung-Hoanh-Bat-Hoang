# JX273 – Role-save revision CAS, stage 1 QA (2026-10-25)

**Disposition:** SQLite/native + Rainbow/Goddess QA PASS. **P0 Issue #27 is NOT closed. This is an opt-in API and is NOT activated for legacy ordinary character saves.** Do not install this QA Goddess binary on real player data.

## Changes

- `gateway/Sources/MultiServer/Goddess/RoleSaveRevisionCAS.h`: create `role_save_revision(role_name TEXT PRIMARY KEY, revision INTEGER)`; read revision, return `-17` for a pre-existing trade-fenced role with no revision record; CAS save requires `expectedRevision` and atomically advances version with `StudentBlob` within `BEGIN IMMEDIATE`; missing or stale save cannot overwrite stored data.
- `sqlitel_db.{h,cpp}`: add opt-in `updateBlobData_versioned()` API. `ensureTradeReceiptTable()` also initializes the revision ledger. On a **new successful** atomic two-role transaction both role revisions advance before COMMIT, with receipt and trade fence. Committed TxID replay returns before revision advance. Legacy `ekey` is not overwritten.
- `tests/native/goddess_role_revision_cas_qa.cpp`: native C++ SQLite fixture, no production data.

## Independent native test

- Compiled MSVC x64 against the existing x64 SQLite import library and source header.
- **32/32 PASS, executable exit 0**.
- Scenarios: revision=0 new role; versioned save; reject stale snapshot; reject old trade-fenced role lacking migration token; atomic pair version advance; post-trade progress save; missing role rollback; close/reopen durable revision; stale replay after restart; successful fresh save after restart.
- Evidence: `C:/Server-Game/SwordOnline/maintenance-builds/jx273_save_ack_gate_20261025/native_role_save_cas_qa.log`.

## Goddess native loopback regression

- Full Goddess x64 Rebuild **PASS, 0 C/LNK errors** from the isolated source worktree.
- Separate loopback-only Goddess binary compiled from the **same source** with bind forced to `127.0.0.1:15001`, not `INADDR_ANY`. All output in maintenance-builds, postbuild live copies disabled.
- Native Rainbow client → QA Goddess → SQLite: **8/8 expected ACK cases PASS**, including two successful new transactions, identical TxID replay, CRC collision, stale TxID, missing role, corrupt CRC and same-role rejection.
- Read-only SQLite verification after transactions:
  - QA_A: revision 2, money 70, bank 20, `ekey` 5743.
  - QA_B: revision 2, money 230, bank 30, `ekey` 5744.
  - Exactly two committed `trade_receipts` and two `trade_role_fence` rows.
  - Total carry gold 300, total bank gold 50, zero items; no extra receipts from replay.
  - `PRAGMA integrity_check` = `ok`.
- Force-terminated QA Goddess after the test; restarted **in the same disposable runtime folder**; native Rainbow new client replayed committed TxID (ACK 1) and rejected stale TxID (ACK -16). Both persisted revisions remained 2 after restart. All QA processes were stopped; port 15001 no longer listening.
- Evidence: `cas_network_trade.log`, `revision_network_verified.json`, `revision-goddess-build/build.log` under `C:/Server-Game/SwordOnline/maintenance-builds/jx273_save_ack_gate_20261025/`.

## Critical outstanding protocol/P0 gates

1. **Legacy ordinary save still calls `updateBlobData_n` directly** and does not supply `expectedRevision`; it can still overwrite a committed trade. Stage 1 **does not protect real accounts against that route**.
2. Version token must be sent on login Goddess → Bishop → GameServer and preserved with the player session, including relog and role-slot reuse; the wire protocol must be negotiated to avoid breaking old servers.
3. Every ordinary save must submit its expected revision and accept returned next revision on ACK. Only the new versioned save path should write trade-fenced roles; legacy migration must fail safely without preventing legitimate progress.
4. Test out-of-order saves, interruption before/after commit, two GameServers, item + gold transfer, ACK loss, retry, player unlock, and runtime recovery with real serialized items in disposable QA.
5. Review compatibility with async legacy BDB and account rename; these tests used ASCII synthetic roles and no real items.
6. TxID clock/instance collision risk remains outside this change.

**Safe release status:** staged database implementation only, **Draft PR for review**, no merge or production deployment. This worktree is isolated from `C:/Server-Game/SwordOnline/ServerNew/_bin_v2_`, which remains untouched.
