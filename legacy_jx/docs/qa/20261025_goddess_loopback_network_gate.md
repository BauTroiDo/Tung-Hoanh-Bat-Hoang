# Goddess x64 — isolated loopback network QA gate, 2026-10-25

## Goal and scope

Advance the previous Phase 3 checkpoint (HEAD `603a4eb`) from native SQLite QA to the real Goddess ↔ GameServer network ACK path, without touching production. **Gate NOT PASSED**: native Goddess process did not complete initialization. No packet or ACK test was attempted against a live service.

## Prepared

- Isolated build folder: `C:\Server-Game\SwordOnline\maintenance-builds\jxonline273_network_qa_20261025`.
- Copied `Goddess.cpp` to QA-only source and replaced just `OpenService(INADDR_ANY,g_nServerPort)` with `OpenService(inet_addr("127.0.0.1"),g_nServerPort)` in that QA copy.
- QA .vcxproj selected the QA-only source, redirected outputs outside repo, and disabled PostBuildEvent copying into game binaries.
- Native MSVC x64 build succeeded without C/LNK errors; `bin\Goddess.JX273Loopback.exe` remains available.
- Runtime folder has copied dependency DLLs, role-name filter data, an explicit `Goddess.cfg` with **port 15001**, `dataOpenModel=1`, and an empty QA-only `database` folder. Nothing copied to `ServerNew`.
- No real account, character, production config, or database was copied into QA.

## Startup probes and results

- Started only the isolated executable from its QA runtime working directory (QA PIDs 9812, 6776, 9112, 8256 in separate attempts).
- No main application window, listener on TCP 15001, or `runtime\database\role.db` appeared; all QA processes were explicitly stopped after verifying their executable path.
- Direct `LoadLibrary` checks in a separate PowerShell process succeeded for QA copies of `FilterText.dll` and `Heaven.dll`.
- A QA-only instrumented build with logging at entry to `WinMain` and before/after initialization produced no trace file.
- Additional QA-only global static initializer trace probes also produced no trace file. This is **consistent with**, but does not conclusively prove, a pre-C-runtime/static-initialization loader stall; logging itself or environment may still be involved.
- The QA variant was a source-only copy; no original Goddess source or .vcxproj changed. The QA files were saved as `qa-loopback-before-startup-probes.cpp.raw`, `qa-loopback-instrumented.cpp.raw`, `qa-loopback-project.vcxproj.raw` in the maintenance-build folder, and the temporary .cpp/.vcxproj in repo were cleaned.
- Build logs: `build-loopback.log`, `startup-probe-build.log`, `startup-static-probe-build.log`.

## Safety verification

- Live `Goddess.exe` PID 6592, `gs001.exe` PID 6960, `Bishop_20261022.exe` PID 8712, `S3Relay.exe` PID 10076 and `S3RelayServer.exe` PID 10092 remained under `ServerNew\_bin_v2_`.
- No production listener, binary, database, characters, or existing Git sources were modified.
- QA TCP port 15001 has no listener; QA role.db was never created.
- No merge, push, or restart was performed.

## Carry-forward acceptance gate

1. Debug and resolve the startup blocker in a genuinely interactive/isolated loader context, using process trace or debugger; inspect why neither QA global static probe nor `WinMain` probe emits data. Do not debug by launching the unmodified Goddess with the production port.
2. Once QA listener is confirmed **127.0.0.1:15001** (not 0.0.0.0), seed only synthetic role rows in disposable SQLite, with valid serialized `TRoleData` blobs.
3. Use the real Rainbow client transport and CPackager chunk framing to send `c2s_roleserver_savetradepair` and receive `s2c_roleserver_savetradepair_result`. Compare ACK, receipt, per-role `trade_role_fence`, and two blobs.
4. Prove success, NACK, timeout, duplicate TxID replay, CRC collision, stale TxID and crash/relog scenarios. Build/start a separate GameServer only when every dependency/port/database target is demonstrably QA-only.
5. **Still open:** ordinary legacy single-role saves can bypass `trade_role_fence`, and Snowflake-style clock/process TxID uniqueness must be validated across restarts/multiple GameServers. These remain production deployment blockers.

## Previous verified QA, unchanged

- Goddess source-native SQLite 53/53 PASS and CoreServer QA 147/147 PASS with latest source. These are **not** proof of network ACK or complete production safety.
