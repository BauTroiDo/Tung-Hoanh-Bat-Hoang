# Bishop P0.4 — in-process account admission before AccountServer

## Problem

P0.3 (PR #10) stopped repeated initial login packets **within the same
socket**, but two distinct Bishop connections could each forward login
credentials for the same account concurrently. The original AccountServer
already has `E_ACCOUNT_EXIST`; however Bishop had no corresponding local
in-flight reservation or owner-only cleanup.

## Scoped implementation

- Each connection's `CGamePlayer` has `BishopAccountClaimSession`.
  `Active()` creates a fresh **monotonically unique ownership token**.
  `Inactive()` releases its reservation **after** the existing legacy
  AccountServer unlock logic, then clears the session state.
  The destructor is an additional cleanup fallback.
- A process-wide `BishopAccountClaimRegistry` uses `std::mutex` and a
  canonicalized account key (ASCII case folded without truncation) to
  admit only one active owner for an account.
  A different connection fails without forwarding its credentials to
  AccountServer; the client receives existing
  `LOGIN_A_LOGIN | LOGIN_R_ACCOUNT_EXIST`.
- Repeated `QueryAccPwd()` within **the same connection** is permitted
  when the account key matches: the original V9 role-deletion flow also
  invokes this method for password re-verification.
- `VerifyAccount()` releases the local claim on explicit failure.
  On success the claim remains until disconnect; if the AccountServer
  never responds, the existing Bishop login timeout closes the connection,
  and `Inactive()` cleans the local claim.
- Ownership checks prevent a stale or foreign token from releasing a
  different connection's claim. `Begin` / `End` / `TryReserve` on a
  connection serialize with a per-session mutex; registry state uses
  its own mutex. No wire protocol structs or opcodes changed.
- Account passwords are not put in the registry; only an account key and
  connection lifetime token are retained.

## Independent verification

- New unit test `test_bishop_account_claim.cpp` checks:
  * empty/malformed account rejection and ASCII case folding;
  * owner re-verification, denial of a different account on one session;
  * owner-only release, token generation and clean reuse;
  * 64 concurrent connections racing for one account, repeated 20 times;
  * cleanup racing with reservation and successful subsequent reuse.
- The prior P0.1 packet guard, P0.2 GS IP peer guard and P0.3 login-once
  regression tests remain enabled (four CTest programs total).
- Ubuntu GCC CTest **4/4 PASS**.
- MSVC Windows Release|x64 CTest **4/4 PASS**.
- Ubuntu ASan+UBSan CTest **4/4 PASS**.
- Actual Bishop VS18 MSBuild `Release|x64 /t:Rebuild` **EXIT 0**,
  using `PostBuildEventUseInBuild=false` to disable legacy postbuild
  copying. PE x64 `0x8664`, 1,569,280 bytes, SHA256
  `06FB6DD0E4A65F50EE096F9402409FFF4364521C159DEDCD6E0F53C4A677F7B7`.
  Source has **115 legacy warnings**. DLL/EXE outputs were written only
  to `Document-ThietKe/_build/bishop_account_win64_20261022`.
- The original `resource.hm`, `lock.ico`, `common.lib`,
  `engine.lib` build prerequisites were temporarily SHA-matched from
  the local reference checkout into the isolated worktree for compilation,
  then removed before committing. They are not repository source.
- Chinese legacy source comments preserved byte-for-byte and CRLF,
  `git -c core.whitespace=cr-at-eol diff --check` PASS.

## Limitations and rollout HOLD

1. This is **single-process/local** admission control. It does **not**
   enforce uniqueness across several Bishop instances, AccountServers
   or world realms; the existing AccountServer remains authoritative.
2. It does not provide a cryptographic proof or ticket, or protect
   cross-process/cross-connection replay of captured credentials. A
   modern JX1 3D `AuthService` should issue short-lived, signed,
   one-use session tickets bound to an authenticated connection and
   consumed by an authoritative WorldServer.
3. Bishop P0.2 now requires a valid
   `Bishop.cfg [Network] TrustedGameServerIPs` for startup; the existing
   live server config does not contain this setting. **Do not deploy**
   a newly linked Bishop.exe without prepared config, firewall, verified
   allowed GS addresses, real JX V9 PC login and GS E2E testing.
4. Some JX V9 client vs Bishop greeting frames differ (34 vs 42 bytes).
   No genuine PC V9 E2E login/role selection has passed. The future
   AuthService/WorldServer architecture is not yet implemented.
5. Legacy logs, other role create/select/delete races, rate limiting and
   distributed session expiration remain separate audit tasks.
6. No actual Bishop/GameServer process was changed or restarted, no
   player/account database mutated, and no Graviti 349 UNKNOWN_HELPER
   source touched.
