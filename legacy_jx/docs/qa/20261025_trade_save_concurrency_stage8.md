# JX273 Issue #27 — Stage 8 QA: concurrent trade + ordinary-save CAS (2026-10-25)

**Result: native transaction/inventory conservation checks PASS. P0 OPEN; production not approved.**

## Test
`tests/native/role_trade_save_race_qa.cpp` compiles natively with the actual Goddess `RoleSaveVersionedReceipt.h` and `RoleSaveRevisionCAS.h` against the same x64 SQLite library used by QA. The fixture has two synthetic 16-byte role blobs: carried gold, bank gold, unique item owner ID, and progression tick.

Three independent SQLite connections operate in WAL mode, with two concurrent threads. `BEGIN IMMEDIATE` trade transactions atomically update both blobs, advance both persisted revisions using `JXRoleSaveRevision::Advance`, and advance the trade fence.

Scenarios:
- Trade commits on connection A while a stale ordinary save from connection B is waiting: the stale save rejects with -17 after commit; old snapshot cannot overwrite trade.
- Valid progress save after trade advances the version and receipt; replay with the same nonce on another connection is idempotent (no duplicate receipt).
- Second trade with opposite item ownership commits atomically; replay of an older saved receipt returns -17, never reviving the stale token.
- Two independent ordinary saves race with the same expected revision but different nonce/blob: exactly one CAS succeeds, the other is rejected as stale.
- Both character snapshots, carrying gold, bank gold, unique item ownership and three committed save receipts are checked after a cold reopen.

**Native x64 MSVC: 37/37 PASS × four independent fresh databases**, exit 0 each time. SQLite integrity checks PASS. Gold carry total 300, bank total 50, distinct unique item IDs 111 and 222 remain conserved.

Evidence:
`C:/Server-Game/SwordOnline/maintenance-builds/jx273_role_protocol_stage7_20261025/trade_save_race_run*.log`.

## Limits of this test

This is a transaction/receipt concurrency QA fixture, **not** a live JX1 character archive, actual inventory serializer, Bishop session/login, two GameServers, or full Rainbow MMORPG exchange. The explicit item ownership and gold fields are synthetic. General Phase3 CoreServer item tests (27/27) and Stage7 Goddess/Rainbow save network tests (5/5+2/2) were run separately.

**P0 remains OPEN.** Before production: run two real QA players through login, item+gold exchange, ordinary saves/retry, server transfer, disconnect, restart/relog and verify real role inventory; validate two-GS and stale ACK on recycled slot; implement controlled resync/user-facing recovery after retry exhaustion; handle old fenced roles lacking revision; audit intentional character deletion permissions and cleanup. Do not deploy or merge Draft PR #29.
