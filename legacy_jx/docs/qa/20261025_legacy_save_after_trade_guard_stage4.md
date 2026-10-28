# JX273 Stage 4 — Fail-closed legacy saves after trade (2026-10-25)

**Disposition:** QA PASS for the scoped legacy-write protection; **P0 Issue #27 remains BLOCKED**. This change must NOT be deployed alone: a player whose role has been trade-fenced needs a fully implemented versioned ordinary-save + ACK path before further progress can be persisted.

## Scope and source

- `gateway/Sources/MultiServer/Goddess/LegacyRoleSaveGuardSql.h`: single parameterized SQLite UPDATE shared byte-for-byte by runtime and native test. The legacy update is permitted only if the legacy ekey condition passes, there is **no** committed `trade_role_fence` for that role, and there is **no** `role_save_revision` entry. Both predicates execute inside the same atomic SQLite UPDATE as the blob write; they are not preflight checks vulnerable to a trade/save race.
- `gateway/Sources/MultiServer/Goddess/sqlitel_db.cpp`: `updateBlobData_n()` initializes required schemas or fails closed; uses the guarded SQL statement. Existing versioned CAS / trade-pair database transaction code remains unchanged.
- `gateway/Headers/RoleSaveRevisionWire.h` and `gameserver/Headers/RoleSaveRevisionWire.h`: identical opt-in `EncodeSave/DecodeSave` extension to the existing 24-byte login trailer, now 36 bytes for a future ordinary save with bounded revision, nonzero 64-bit request nonce and integrity digest. The **runtime save opcode/ACK is not yet wired** to this codec.
- `tests/native/legacy_role_save_guard_qa.cpp` and `tests/native/role_revision_save_wire_qa.cpp` are native regression fixtures.

## Independent QA: Native SQLite

- Native C++ x64 compiled with the same `LegacyRoleSaveGuardSql.h` used by Goddess: **37/37 PASS**, exit 0.
- Covered allowed pre-trade legacy progress, ekey rejection, fenced-role refusal even with very high ekey, migrated CAS-role refusal, missing-role no-op, a second SQLite connection blocked behind `BEGIN IMMEDIATE` while a trade committed, restart/cold reopen persistence. Late save after trade returned zero affected rows; the committed blob did not change.

## Versioned save-wire codec

- Native C++ x64: Gateway **19/19 PASS**, GameServer **19/19 PASS** (identical header SHA), including 10,000 revision+nonce round trips per side, absent legacy token rejection, wrong name, short/extra trailer, overflow, zero nonce and tampering.
- This is a standalone codec, **not a negotiated save/ACK protocol**.

## Real Rainbow network tests with freshly built Goddess

- Full Goddess x64 source rebuilt in isolated maintenance-builds with forced **loopback-only** bind 127.0.0.1:15001 and post-build deployment hooks disabled: exit 0, **zero compiler/link errors**. Remaining warnings C4267 in unrelated legacy code.
- Native Rainbow client against new QA Goddess + two disposable SQLite roles:
  - **8/8 ACK cases PASS**: two successful atomic trade commits, duplicate replay, CRC collision, stale TxID, missing role, invalid CRC, same-role reject.
  - **Legacy save after committed trade:** sent an actual opcode 10 packet with a **pre-trade role BLOB**; Goddess returned opcode 56, marker 791030, **NACK -1**, request slot ID 7, exit 0.
  - Verified role BLOBs after rejected legacy save are **byte-identical** to committed `post2_A.bin` and `post2_B.bin`; carried gold QA_A=70, QA_B=230; bank gold 20/30; ekeys 5743/5744 unchanged; exactly two receipts, both revisions=2, both fences share last TxID 4000000000010002; `PRAGMA integrity_check = ok`.
  - Same runtime directory restart: committed TxID replay ACK 1, stale TxID NACK -16, exit 0.
- All QA Goddess processes stopped by verified executable path, loopback port closed.
- Evidence: `C:/Server-Game/SwordOnline/maintenance-builds/jx273_role_protocol_stage4_20261025`: native test logs, `Goddess/build.log`, `cas_network_trade.log`, `legacy_guard_network.log`, `stage4_network_db_integrity.json`.

## Outstanding P0 gates — NOT PRODUCTION READY

1. **Legacy ordinary save now fails after a trade (correct for preventing silent overwrite), but legitimate post-trade character progress still cannot be persisted until Goddess and GameServer negotiate versioned ordinary save requests, CAS commit, nonce-bound ACK, retry/resync and player-session identity. Do not deploy this stop-gap by itself.**
2. Stage 3 role login trailer has been compiled, but live login/relog across Bishop and GameServer with real serialized player roles and transfer has not been verified.
3. ACK loss, duplicate save ACK, slot reuse, overlapping trade+ordinary save, two GameServers and version migration of older fenced roles all need reliable fail-closed recovery.
4. `sqlitel_db::updateBlobData()` is an **alternate destructive delete+insert writer** used by `copyDBdataToSql()` in the BDB→SQLite migration path; this Stage 4 guard covers `updateBlobData_n()` used by ordinary player saves, **not** all offline maintenance/migration writers. Audit and fence migration operations before running them on live data.
5. Test real serialized item+gold trade and maximum save packet size, SQL busy/failure, rollback, BDB recovery, and full native/client gameplay.

**Release status:** Stage 4 evidence and source only; keep Draft PR #29 and Issue #27 OPEN. Do not merge or install Goddess without the save/ACK revision protocol. Production `ServerNew/_bin_v2_`, character DB and live service state were not changed.
