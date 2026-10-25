# Goddess real Rainbow network: synthetic positive atomic trades (2026-10-25)

## Outcome and scope

**PASS: native Rainbow.dll -> isolated Goddess x64 -> disposable SQLite positive trade,
replay and negative ACK tests. NOT full product GameServer/client integration acceptance.**

Base Git HEAD at start: c6596e2 on branch checkpoint/jxonline-273-triage-20261024.
This work added QA artifacts and reports; no server source changes were necessary.

## Test setup

- QA server binary: Goddess.JX273LoopbackFix.exe (x64), QA source bind:
  OpenService(INADDR_LOOPBACK,15001). Actual listener confirmed at
  **127.0.0.1:15001** (not all interfaces).
- All runtime and SQLite files are outside production:
  C:/Server-Game/SwordOnline/maintenance-builds/jxonline273_network_qa_20261025/.
- Test client is compiled native x64 and loads Rainbow.dll to exercise the
  real transport. It uses the byte-identical SendTradePairLargePacket helper
  embedded from gameserver/Sources/Core/Src/KPlayerTradeJournal.cpp. It is
  not the complete live GameServer process.
- The synthetic serialized role blobs were produced by a native C++ fixture
  using the genuine S3DBInterface.h TRoleData/TRoleBaseInfo definitions:
  sizeof(TRoleBaseInfo)=528, sizeof(TRoleData)=568,
  offsetof money=432, bank=428, item count=536.
- QA_A initial pocket gold=100, bank=20, items=0.
  QA_B initial pocket gold=200, bank=30, items=0.
  Legacy ekey values QA_A=5743 and QA_B=5744.
- Synthetic SQLite started with exactly those two role_Blob rows;
  no character/account data from production was copied.

## Native real-network ACK test ? 8/8

Via Rainbow SendPackToServer, Goddess CPackager/ClientNode and SQLite:
1. POSITIVE_1 TxID 4000000000010000: expected/actual ACK 1, success.
   Pocket gold QA_A 100->80; QA_B 200->220.
2. DUPLICATE_SAME_TX: expected/actual 1, idempotent.
3. DUPLICATE_CRC_COLLISION: expected/actual -13, CRC/TxID collision.
4. STALE_TX_REJECTED: expected/actual -16, stale distinct TxID.
5. POSITIVE_2 TxID 4000000000010002: expected/actual 1, success.
   Pocket gold QA_A 80->70; QA_B 220->230.
6. MISSING_ROLE_REJECTED: expected/actual -14. Request and embedded
   role data both name QA_MISSING, but role_Blob has no such row.
7. CORRUPT_CRC_REJECTED: expected/actual -11.
8. SAME_ROLE_REJECTED: expected/actual -15.
All ACKs were 85 bytes with opcode 57 and matching TxIDs.
Native test process exit 0 (see positive-network-test-v2.log).

An earlier QA iteration intentionally used QA_MISSING in the outer request
with QA_B still inside the blob. Goddess correctly rejected it with -12
before looking up the database; this was a **fixture mismatch**, corrected
before obtaining the 8/8 PASS above.

## SQLite post-commit verification

- Exactly two committed trade_receipts rows, TxIDs
  4000000000010000 and 4000000000010002.
- Exactly two trade_role_fence rows, each with last_tx_id
  4000000000010002.
- Stored BLOBs match post2_A.bin and post2_B.bin **byte-for-byte**:
  QA_A money 70, bank 20, item count 0, ekey 5743;
  QA_B money 230, bank 30, item count 0, ekey 5744.
- Pocket-gold total remains 300; bank-gold total remains 50.
- No duplicate receipt or extra role row was created by replay/NACK.

## Reconnection and process restart

- A new Rainbow client process connected to the original Goddess QA:
  identical TxID ACK 1, stale TxID ACK -16, client exit 0.
- QA Goddess was deliberately terminated only after the two transactions
  completed, to test persisted data. A **same-folder restart** then exited
  while enabling the service. QA had initialized its legacy Berkeley DB
  environment, and the same-folder issue was **not resolved or fully
  diagnosed**. Do not claim crash-safe full runtime restart.
- For a narrower durable-SQLite check, only the committed role.db was copied
  byte-identically into a fresh QA runtime folder; all BDB files were
  deliberately absent. This Goddess instance started and listened on
  127.0.0.1:15001. A new Rainbow client replayed the same TxID (ACK 1)
  and old TxID (ACK -16); exit 0.
- Post-restart read-only SQLite equality comparison showed exact equality
  of all saved role BLOB hashes, gold/bank/item counts, ekeys, both receipt
  rows and both per-role fences. SQLite PRAGMA integrity_check returned ok.
  Evidence: positive-before-restart.json and positive-after-restart.json.

## Open release gates

1. Live GameServer application runtime -> separate QA Goddess -> real game
   client, role login/relog and trade UI; not yet exercised.
2. Synthetic fixtures had **zero items**, so the test proves money totals
   and opaque serialized BLOB persistence, NOT real item transfer correctness.
3. Test dropped ACK, interrupted SQLite commit, same-folder Berkeley DB
   recovery, actual reconnect/timeout and end-to-end character unlock.
4. Reconcile asynchronous legacy single-role saves with new trade_role_fence;
   these saves still bypass the fence. Harden durable TxID allocation across
   clocks, restarts and multiple GameServers.
5. Do not deploy or merge source to production before full integration checks.
   Git repository still has no configured remote; no push.

## Operational alert (observed near 08:41 +07 on 2026-10-25)

A read-only process inspection found that production Goddess.exe, gs001.exe,
Bishop and S3Relay processes previously observed under ServerNew/_bin_v2_
were **no longer running**. The host had not rebooted (last boot reported
2026-10-20). Application Error/Hang events and Security 4689 process-exit
events yielded no matching evidence in the inspected window. No root cause
or time of production shutdown could be established from those checks.

**QA commands stopped only PIDs whose full ExecutablePath was under
maintenance-builds, and never issued a stop/restart/deployment command
to production. Do not infer causation from the timing.**

No production process was restarted or modified. When this document was
prepared, the QA listener on 15001 was stopped as well.
