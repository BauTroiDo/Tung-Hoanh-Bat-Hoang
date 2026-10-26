# CN02-ONLINE-RESUME-01 — Server reachable, controlled Map53 QA client launched

**Ngày:** 2026-10-26, khoảng 07:43–07:49 Asia/Ho_Chi_Minh. **User says the repaired server can connect.** User's previous SERVER_PAUSED_BY_USER hold is now **lifted for controlled read-only preflight and one new manual QA session only**. Do not assume GameServer database, character permission or original Map53 works merely because sockets connect.

## Verified live topology

- Client device `DESKTOP-TPG2RNF` IP `192.168.1.13`.
- Server device `WIN-33R6OP5GO1M` IP `192.168.1.69`.
- From client, TCP connect succeeded to Bishop `56722`, original GameServer `51189` and native gateway `14180`.
- On server: Bishop `Bishop_20261022.exe` PID **3212** listening `56722`, restarted `gs001.exe` PID **7444** listening `51189` (prior snapshot PID1608 obsolete), QA gateway `python.exe` PID **7600** listening `14180`, still started with `--native-gs-bridge --allow 192.168.1.13`.
- Verified Bishop PID3212 and GS PID7444 have an **Established** internal TCP pair (Bishop local 56732 ↔ GS local 62666). This proves connectivity between native processes, NOT game login/role permit.
- Server native_p0_readiness.evaluate(): `CONTROLLED_MAP53_PROTOCOL_TEST_READY`, `world_entry_enabled=true`, no stale source.
- Fresh client-side **read-only native TCP greeting**: exactly **34 bytes**, little-endian length34, prefix `22 00 20 00`. Sent no username/password/auth request. Server log registered corresponding short-lived greeting/close.

## Controlled client QA launch

- Original client 4016 original files retained unchanged: `Game.exe` SHA256 `1CAE3982C04BE9D6F8F7EA8CE272600400655D40FBF0671818EFF1D7A79750EE`; `GameAssembly.dll` `011357720B210AA1F392A8B62FF8DE090D3A895230A6E0135BCE707196871069`; `global-metadata.dat` `5586F416AA26EBE57EA7F6580D0A53E5E5F29C50E425E64A4D3F2A95D6069E08`.
- Existing localhost mirror `127.0.0.1:14186` PID **13100**, health `catalog_1.0.hash` HTTP **200 / 32 bytes**.
- Started **only one** original-client QA instance with existing `Client-3D-2/LAN-LOCAL/Start-M1-Map53-EarlyRoot.mjs` (memory-only Frida launcher; no EXE/metadata disk edit).
- `Game.exe` PID **19552**, launcher Node PID **26224**. Launcher stdout evidence under `E:\Games\VLTK3D\SwordOnline\Tool\_outputs\cn02-bulk\map53-resume-20261026-0745\launcher-stdout.log`; previous EarlyRoot JSON/log files were copied into this isolated QA directory before the new launch.
- Latest launcher stdout: `mountedBeforeFirstPakRead=5`, first read 572B, `lanEarlyRootArmed`, `lanRootBeforeFirstPakRead`, `lanRootAfterServerList`, `localServerReady=true`, `contentReady=true`, `loadReady=true`, `channelHasTabs=1`, `LAN_ONLY_ATTACHED_PID=19552`, `nativeLanLoginFormOpened accountSubmission=false originalSdkBlocked=true`.
- The native LAN login form is visible/active according to instrumented UI state; **no new real role-select or GS permit accepted yet**. Only the user should manually enter credentials and select a QA character. Never log/paste passwords/tickets. Launcher intentionally suppresses SDK auto-login.

## Gate disposition

- Server preflight/transport **PASS**.
- Client bootstrap to login panel **PASS**.
- Bishop genuine role permit after server restart **NOT YET OBSERVED**. Historical PermissionError at 05:59 belongs to the pre-restart snapshot.
- WorldSync, exact MapCopyID/Map53 resources/Unity SceneReady, avatar and original GS authoritative movement **NOT VERIFIED**.
- 6 active Map53 affine coefficients and 3+1 same-scene original MPS↔Unity landmarks **NOT CAPTURED**.
- **CN02-04 stays BLOCKED**, **CN03 stays CLOSED**. Offline BULK-01..04 remain accepted separately by review commit `3db49cc`.

## Next real action

User logs into the **already running Game.exe** with their own QA credentials and chooses one QA character. Then check NEW timestamps of native gateway/Bishop/GS and mirror/game logs for exactly one same-session chain: Bishop permit → original GS ACK/WORLD_SYNC actual SubWorld, MapCopyID, Region → resolved official PC bundles → SceneReady/avatar → user-clicked movement/GS authoritative ACK/client echo. Reject cross-session concatenation. Only then pursue six affine coefficients and 3+1 genuine landmarks.

**No Bishop/gs001/gateway restart, no DB or UnityProject changes occurred during this resume.** Do not kill these QA PIDs casually; verify actual process identities and close Game.exe normally after user finishes. Older SERVER_PAUSED_BY_USER headings in docs reflect historical status, now superseded by this confirmed-resume checkpoint.
