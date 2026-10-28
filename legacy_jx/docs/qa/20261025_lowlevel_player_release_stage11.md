# JX273 Issue #27 — Stage 11: enforce save-exit fence in low-level KPlayerSet (2026-10-25)

**Status: Source QA PASS. NOT production-ready; keep Draft PR #29 and Issue #27 P0 OPEN.**

## Finding

Stage 10 protected GameServer.PlayerLogoutGateway() and the public CoreServerShell wrappers ClientDisconnect(), RemoveQuitingPlayer(), and mandatoryShutDownClient(). The audit then found **direct KPlayerSet.PrepareRemove calls from ScriptFuns.cpp LuaTickOutPlayerInServer()**, bypassing those CoreServerShell wrappers. Direct KPlayerSet removal could clear the inventory and reset a recycled player slot while the final versioned save ACK remained unresolved.

## Changes

In gameserver/Sources/Core/Src/KPlayerSet.cpp, add raw-byte-preserving ASCII patches:
- __PrepareRemove(): guard after the slot bound check and before CheckMaps() / WaitForRemove().
- PrepareRemove(): guard after the slot bound check and before CheckMaps() / any logout/unlock side effect. This catches LuaTickOutPlayerInServer() calls.
- RemoveQuiting(): add explicit slot bounds and guard before destroying inventory, releasing the KPlayer, and reusing the numeric slot.
- gm_PrepareRemove(): guard immediately after identifying a live role, before GM-directed leave/unlock packets.
- Use the existing immutable RoleSaveExitFence.h policy and the KPlayer protocolSeen/revisionLoaded/savePending flags. All new guards are under _SERVER to preserve client-only compilation.

This is an **in-memory quarantine**, not an automatic reconciliation or retry policy. It does not solve missing client-visible recovery; never reset the revision or silently unlock a quarantined character.

## Verification

- MSVC CoreServer.dll x64 incremental QA build PASS, zero C/LNK errors; inherited C4566 codepage warning from an older source line. Rechecked gs001.exe x64 QA Build PASS, zero C/LNK errors and no production copy/postbuild action.
- Native RoleSaveExitFence policy: **13/13 PASS** from Stage 10 (unchanged).
- New tests/native/role_save_release_entrypoints_qa.py checks the actual source-wired guards before mutation in all three CoreServerShell wrappers, GameServer quit/timeout, and four KPlayerSet pathways including Lua/GM; **ALL PASS (18 assertions)**.
- Freshly rebuilt CoreServer.dll copied byte-for-byte to isolated test runtimes; actual MSVC regression executables all PASS: FinalRuntime 60/60; PersistenceE2E 27/27; RuntimeVerified 60/60, total **147/147**.
- Source original CP936/GBK bytes preserved; git whitespace check PASS.
- QA evidence/binaries isolated under C:/Server-Game/SwordOnline/maintenance-builds/jx273_role_protocol_stage11_20261025.

## Still blocking P0 release

A versioned role that exhausts retry remains protected/quarantined; **authoritative resync/relog with user-facing feedback has not been implemented/accepted**. No fully integrated gs001.exe + Bishop + Goddess + two real QA clients item/gold save-trade/disconnect/relog/restart test. Other deep player deletion entrypoints should be audited during that acceptance, alongside separate GM-delete authorization. Do not merge or deploy production until acceptance is complete.
