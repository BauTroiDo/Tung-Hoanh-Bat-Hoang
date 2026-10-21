# Bishop P0.3: at-most-one initial login request per TCP connection

## Security / correctness issue

`Intercessor::AnalyzePlayerRequire` previously accepted every login packet,
called `AppendData` and then unconditionally invoked
`CGamePlayer::DispatchTask(enumLogin)`. The latter reset all task processors
even when an earlier login, role selection or role-server operation was in
progress. The queue `CDataQueue::AddData` also **replaces** a previous
buffer under the same opcode key; duplicate login packets could overwrite the
first credentials before the task processor reads them.

## Fix in this isolated source-slice

- Each `CGamePlayer` owns `BishopLoginAttemptGate`, a
  `std::atomic<bool>` with a compare-exchange winner. It is reset on
  `Active` and `Inactive`, so a reused network slot can accept one login
  in each new connection.
- `AppendData(enumOwnerPlayer)` checks the minimum actual login packet
  length and claims the gate **before** the queue can overwrite the earlier
  packet. Duplicate login requests are rejected. The gate is rolled back if
  queue insertion fails.
- `DispatchTask(enumLogin)` additionally refuses to reset an in-flight
  task; `Intercessor` dispatches only when the original packet was
  successfully queued.
- `DispatchTaskForPlayer` now propagates the real queue insertion result
  rather than always saying success.
- No existing opcode/packet struct/wire size or new login response is added.
  Source-byte preserving edits retained the original Chinese ANSI comments
  and CRLF.

## Tested

- Portable concurrency test runs **64 concurrent login attempts** against
  the same gate, 20 iterations, requires exactly one success; also checks
  single-thread duplicate rejection, reset and re-accept.
- Existing P0 packet guard and P0.2 trusted GS peer tests remain active.
- Ubuntu GCC CTest **3/3 PASS**.
- Windows MSVC Release x64 CTest **3/3 PASS**.
- Ubuntu ASan+UBSan CTest **3/3 PASS**.
- Windows VS18 MSBuild Bishop `Release|x64 /t:Rebuild` EXIT 0, PE x64
  machine `0x8664`; hash/size in final acceptance checkpoint. The build
  still contains 115 pre-existing warnings.
- `git -c core.whitespace=cr-at-eol diff --check` PASS.

## Limits, cautions and future work

This prevents **duplicate initial login commands on a single active socket**.
It is NOT cross-connection cryptographic replay prevention, distributed
single-account session enforcement or a complete race audit of the entire
login/role-select state machine.

Other select/create/delete role opcodes are still handled through legacy
per-opcode queues and need original V9 packet fixtures before imposing strict
new ordering. Remaining major gates: authenticated GameServer handshake
(beyond P0.2 source IP allowlist), original client V9 handshake mismatch
and authorized GS + role-list + account login E2E testing.

New Bishop builds must not be deployed until the P0.2 mandatory config
`[Network] TrustedGameServerIPs` is populated with actual authorized peers,
and runtime acceptance passes. Runtime Bishop and GameServer remain
untouched.
