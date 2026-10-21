# Final scoped Bishop cleanup: malformed AccountServer response

In `CGamePlayer::VerifyAccount()`, a short or invalid AccountServer
verification response was previously popped from `CDataQueue::Attach()`,
released and returned as `enumError` without actively terminating the
failed login connection. This could leave the socket and its
process-local account reservation alive until the normal timeout.

The updated branch sends the existing `LOGIN_R_FAILED` response,
requests disconnect via `IServer::ShutdownClient`, and returns
`enumError`. The existing `Inactive()` path performs AccountServer
unlock and then releases the local account reservation in the correct
order. Do **not** release the local claim before AccountServer unlock,
which might allow a second login during a stale backend lock.

The data queue's `Attach` takes/removes its entry; `Detach` is a
no-op, so no extra queue cleanup call is necessary. No protocol
opcodes, framing, payload definitions or client compatibility changes.

## Verification and limits

- Scoped source delta: 7 lines in `GamePlayer.cpp`, original source
  byte encoding and CRLF preserved.
- Real VS18/MSVC Bishop `Release|x64 /t:Rebuild` completed with exit 0.
  Generated PE has x64 machine `0x8664`; **115 legacy warnings**.
- Existing four independent Bishop tests pass in Windows Release and
  Ubuntu ASan+UBSan. These exercise packet guard primitives, IP allowlist,
  duplicate-login gate and account claim registry.
- **The malformed AccountServer handler itself was not exercised using
  a real AccountServer and malformed network response.** Such handler
  integration testing remains a gate before deployment.
- This concludes the narrowly scoped Bishop static cleanup. The new
  JX1 3D client **does not use Bishop or original V9 client**. Continue
  the unfinished gameplay source/WorldRuntime integration instead of
  extending Bishop without a specific new failure.
- No remote service, client or database modified or restarted.
