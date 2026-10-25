# JX273 ? Goddess legacy character save ACK fail-closed gate (2026-10-25)

## Scope

**PARTIAL FIX** for P0 Issue #27. Never interpret an unsuccessful Goddess
ordinary save as success, transfer complete, or approval to disconnect.
The durable trade-fence/expected-revision protocol is NOT implemented here.
Issue #27 MUST REMAIN OPEN.

## Root cause

File gameserver/Sources/MultiServer/GameServer/KSOServer.cpp, case
s2c_roleserver_saverole_result, previously checked only the legacy
nDataLen marker 791030 and the player slot index. It logged
[Archive successfully] and proceeded with transfer/disconnect
handling even when pDataBuffer[0] was -1 (Goddess rejected the save).

Goddess ClientNode::_SaveRoleInfo already encodes save result as
pDataBuffer[0] = 1 for success, -1 for failure.

## Source fix

- Reject truncated save ACK before casting/reading TProcessData.
- Check pDataBuffer[0] == 1 as the only success condition.
- For NACK, preserve a new save request (SAVE_REQUEST) and exit the
  handler before logging success, SAVE_IDLE, transfer, or disconnect.
- Legacy wire packet ABI/opcodes remain unchanged.

## Verification

- Fresh x64 MSVC GameServer Rebuild PASS; exit 0; no C/LNK errors.
- Linked against isolated known-fresh CoreServer.lib from 2026-10-25.
- Binary in maintenance-builds/jx273_save_ack_gate_20261025/bin/gs001.exe.
- SHA256: F67C525B4DCFB81A5B152B81A01CE0D53F7F6C69D2CA726A4D51B5F4B31DEED9.
- Six source-contract assertions PASS for length guard, ACK success
  check, NACK retry and early exit ahead of success/idle actions.
- QA MSBuild project disabled copying outputs to the installed game
  binary directory; original GameServer.vcxproj unchanged.
- Tests here are build and source checks, NOT end-to-end runtime checks
  with an actual game client or Goddess NACK injection.

## Remaining release blockers

1. Propagate durable expected role snapshot version/fence from Goddess
   login via Bishop into GameServer, and back in normal save packets.
2. Compare atomically against trade_role_fence when saving SQLite and
   update the client-side token upon successful trade ACK.
3. Verify out-of-order ordinary saves, relog, restart, ACK loss, slot
   reuse, transfer and valid progression saves after normal trades.
4. Test gold + real items on disposable character data, and 2 GSs.
5. Prove TxID generator monotonic and collision resistant across restarts.

No production exe, player DB, assets or configuration replaced in this
step. User-installed previous x64 QA candidate remains unchanged.
