# Goddess x64 Berkeley DB startup/recovery hardening QA ? 2026-10-25

## Scope
Source-only change, NOT a production deployment. The production binary
installed in the previous session was not replaced/restarted.

## Source change
File: gateway/Sources/MultiServer/Goddess/DBTable.cpp (GBK/CRLF preserved).

- Add DB_INIT_LOCK to DB transaction/locking environment initialization.
- ZDBTable::open() succeeds only if index == index_number, instead of
  index <= index_number which incorrectly accepted failed index initialization.
- Close opened secondary handles before the primary on failure.
- Initialize gdbenv=NULL and copy _getcwd using strcpy_s, freeing the
  allocated path instead of treating it as a printf format string.

## Tests
- Loopback-only Goddess x64 compiled from source: MSVC Rebuild EXIT 0,
  zero C/LNK errors.
- Isolated runtime:
  C:/Server-Game/SwordOnline/maintenance-builds/jx273_bdb_recovery_20261025/runtime
- Service bound only to 127.0.0.1:15001, not LAN.
- Forced QA process termination, then restart in the SAME directory
  with pre-existing Berkeley DB files: both runs READY=True,
  verified listener PID/address/port and both QA processes stopped.
- Evidence: same-dir-abrupt-restart.log.
- This does not test power loss, corrupt BDB logs or real character BDB
  recovery. Those remain outstanding acceptance gates.

## Remaining P0 (NOT RESOLVED)
The legacy per-character save packet c2s_rolekfjhu_saveroleinfo does not
carry the per-role trade TxID required by trade_role_fence. A delayed
legacy save can therefore overwrite an atomic two-role commit even though
the trade commit and replay are safe.

Rejecting every subsequent ordinary save of a traded role would also
block legitimate character progression. That destructive workaround is
NOT added. A versioned save protocol with a persisted expected fence
token is required, plus tests for relog, out-of-order saves and restart.

## Delivery
Publish the code and tested QA documents on a separate GitHub branch
under legacy_jx; no force-push/main overwrite and no database/secrets.
