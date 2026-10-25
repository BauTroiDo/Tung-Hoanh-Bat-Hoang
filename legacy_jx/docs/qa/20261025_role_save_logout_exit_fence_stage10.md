# Issue #27 — Stage 10: prevent unlock/logout with unresolved versioned save (2026-10-25)

**Result: isolated source QA build and regressions PASS. NOT production-ready; keep PR #29 DRAFT.**

## Finding

Stage 7's retry-exhaustion branch sets `SAVE_REQUEST` after the save ACK cannot be established. In the legacy `KSwordOnLineSever::PlayerLogoutGateway()` loop, `SAVE_REQUEST` is also the condition accepted by the quitting branch before it calls `RemoveQuitingPlayer()` and unlocks the account/role. The login-timeout branch may similarly unlock a role without consulting an unresolved revisioned save.

Therefore `SAVE_REQUEST` **does not mean the latest versioned save was durably acknowledged**. Releasing/recycling the role after retry exhaustion or a trade/save collision before authoritative reconciliation risks losing the user's in-memory progress. The Stage 7 fail-closed claim did not cover this legacy exit path.

## Fix

- Added `gameserver/Headers/RoleSaveExitFence.h`, a small pure policy: `BlocksRelease(versionedSession, revisionLoaded, pendingSave)`. It blocks release of a versioned session whenever an ACK is still outstanding or the revision token has been invalidated by failure/trade-save race.
- Added `ICoreServerShell::IsRoleSaveReleaseBlocked(slot)` and the CoreServer implementation using the live `KPlayer` protocol flag, revision-loaded flag and pending-save state. Does not expose or regenerate a nonce.
- `KSwordOnLineSever::PlayerLogoutGateway()` now checks this release fence **at the top of its per-slot loop**, before both login-timeout and character-quit branches; unresolved versioned saves do not unlock a role, send leave notifications or invoke RemoveQuitingPlayer. Legacy sessions and versioned sessions whose ACK/revision were confirmed retain original behavior.
- Source modifications were applied as raw byte patches to preserve existing CP936/GBK legacy comments. Original source byte snapshots and generated binaries remain only in QA maintenance-builds.

## Evidence

- New native x64 policy test `tests/native/role_save_exit_fence_qa.cpp`: **13/13 PASS**, including pending save, negative ACK, trade/save collision, retry exhaustion, success ACK, authoritative re-login and legacy behavior.
- `CoreServer.dll` MSVC Release x64 Rebuild: **PASS; zero C/LNK errors**.
- `gs001.exe` MSVC Release x64 Rebuild: **PASS; zero C/LNK errors**. Only inherited missing-PDB LNK4099 warnings from ms64.lib.
- Linked against previously verified Stage3 Engine.lib; postbuild auto-copy deployment disabled; both new binaries are under `maintenance-builds/jx273_role_protocol_stage10_20261025`.
- Fresh Stage10 Core test binaries + Stage10 rebuilt CoreServer.dll: FinalRuntime **60/60 PASS**, PersistenceE2E **27/27 PASS**, RuntimeVerified **60/60 PASS**, total **147/147**.
- `git -c core.whitespace=cr-at-eol diff --check` PASS.

## Still blocking Issue #27

This is a **quarantine/fail-closed safety fix, not a recovery implementation**. The unresolved KPlayer remains blocked in memory until a separately verified authoritative reconciliation/relogin. User-facing resync, stale session cleanup, and a real two-client, two-GS login-save-trade-disconnect-restart cycle have **not** been accepted. All character data/production binaries remain untouched. Do not merge/deploy PR #29 or mark P0 closed on the basis of this QA alone.
