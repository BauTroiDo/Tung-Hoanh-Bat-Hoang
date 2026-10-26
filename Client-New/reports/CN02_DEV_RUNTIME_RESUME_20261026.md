# CN02 DEV continuation — ping provenance logging and missing Lua source

Date: 2026-10-26, Asia/Bangkok (UTC+7). Branch: `cg/cn02-anchor-evidence`.
User explicitly authorized DEV edits, builds and service restarts. No character DB, Bishop, GS executable, UnityProject or reference client binary was changed.

## Gateway deployed state

The newer checkpoint `c7304c9` already recorded the NativePingPair repair. It was present on DEV at session start: native_session.py SHA256 `715208d04f731cbd12ddf1c20f60c348d80cd34c57fe876af895844a9ba3cd34`, gateway PID6324, GS4168 and Bishop3212. Do not redeploy the older structural candidate from d3866c7.

Re-read original `KSOServer.cpp::PingClient/ProcessPingReply`: GS143 contains `m_nGameLoop`, saved as `nSendPingTime`; GS requires a 9-byte C2S122 with exactly that server value, then sends S2C174 containing the received client-time sample. Existing NativePingPair preserves that provenance and consumes one pending token per response. Native82 is a client-time sample, not proof of the GS token.

Added only numeric INFO logs to native_session.py:

- GS143 receipt: `server_tick` from the actual GS packet.
- C2S122 after backend.send: actual byte length, entity, server_tick and native82 client_tick.
- GS174/native131 after send_native: actual echo client_tick, entity and byte length.

New source SHA256: `5dbbb751cc67873db9fcc9ae12a48157dee5f2e5f5e6739c04a1a5f9721cac17`.
Readiness correctly refused the stale native_session hash. After **117/117 bridge tests**, refreshed only its source hash; readiness returned `CONTROLLED_MAP53_PROTOCOL_TEST_READY`, `world_entry_enabled=true`, **m1_p0_pass=false**.

Restarted only gateway after checking **zero Established sessions on 14180** and verifying old process command. New PID **4088**, native bridge and QA allowlist unchanged. New log: `C:\Server-Game\SwordOnline\Client3D-LAN\qa_runs\cn02-dev-runtime-trace-20261026\gateway-trace.log`. At 12:44:26 listening; client probe at 12:44:35 returned exactly **34 bytes**, prefix **22002000**, then closed normally. This is greeting proof, not authenticated ping proof. No real login appeared in the new log during this session.

Backup/rollback directory on DEV: `Client3D-LAN\qa_runs\cn02-dev-runtime-trace-20261026`. Contains native_session.py.before, native-p0-readiness.json.before, deployed copies, tests.log (`bridge-tests.log`), deployment-preflight.json, previous-command.txt, gateway-pid.txt and ping-trace.patch. With no active QA sessions, stop the verified current gateway, restore **both** .before files, verify readiness, then relaunch with the saved command's native bridge/allowlist and a fresh log path. This restores the previous repaired ping behavior, removing only added trace logging. No GS/Bishop restart needed.

## Lua — stronger evidence, different error branches

Added stdlib read-only `tools/reference4016/lua_mount_audit.py` and ten regression tests. It uses Win32 uint32 overflow, the ASCII path hash from KPakList.cpp, and PACK header/index layout from XPackFile.h. Validates index/data bounds and rejects duplicate IDs. It does not decode, execute, replace or create gameplay scripts.

Executed against actual DEV GS root `ServerNew\_bin_v2_\gs`. **All seven archives parsed without errors**: maps.pak 95,366 entries, maps01.pak706, script.pak425, skill.pak499, jingjichang.mps35, qianchonglou.mps4, yanwuchang.mps26. package.ini lists maps.pak and three MPS archives; script.pak is not configured for mounting.

| Target | Win32 lookup ID | Loose source | Candidate in any of seven archives |
|---|---|---|---|
| script/system/online.lua | e19c8de5 | absent | absent |
| script/system/player/playerlogin.lua | f511cc08 | absent | absent |
| script/system/player/playerlogout.lua | 54f1a1d6 | absent | absent |

`script/system` currently contains only serverscript.lua; recursive filename search under SwordOnline found no copies of the three targets. `script/script.txt` is also absent. Merely mounting script.pak will not supply these missing IDs.

Verified actual source branches:

- KSortScript.cpp `g_GetScript` looks only in the preloaded tree/map and returns NULL when absent. LoadAllScript enumerates loose files and optional script.txt; KPakFile tries the configured package list and loose fallback.
- KPlayer.cpp `ExecuteScriptC` writes the observed **online.lua Script error** when `pScript == NULL`, before calling OnLine. That message does **not** establish a nil callee inside an existing OnLine function.
- KPlayer.cpp `ExecuteScript(path, main/OnLogout, ...)` emits the observed **Error Execute** immediately when its script lookup is NULL. The nearby standalone `attempt to call a nil value` still lacks a line/callee trace; log adjacency cannot assign it to one of these missing functions.

Conclusion: **missing hook source is established for this on-disk installation/configuration**. Actual live cache contents and exact standalone nil call remain unverified. Genuine matching Lua sources/backups are required before repairing hook behavior; no no-op hooks or guessed login/logout logic were installed. No character save/DB changes.

## Map53, NPC/AOI and movement

Launched one QA Game.exe PID **31732** using the existing EarlyRoot/LAN adapter. Parent launcher PID29308; mirror PID13100 retained. Rechecked original Game.exe, GameAssembly.dll and metadata hashes against checkpoint: all match. Launcher mounted five PAKs and reported localServerReady/contentReady/loadReady. It did **not** report a submitted login or nativeLanLoginFormOpened during observation. Do not call the login form visually verified.

Windows capture failed repeatedly with timeout, including after activation; no usable screenshot was obtained. The client process reports Responding=True. A **28-second passive Frida observer** on metadata/SQLite-verified world and scene entries recorded installation and **1,500 main-loop callbacks**, but **no WORLD_SYNC or scene loader events**. Observer unloaded/detached in finally. No game methods were called, returns changed, packets sent or original files patched by this observer. This is idle-client evidence only; not proof of a scene failure or success. Raw observer and launcher output are under `.tmp/cn02-dev-resume-20261026` outside Git.

Further source mapping, not a deployed AOI converter:

- Legacy75 = s2c_syncplayermin / PLAYER_NORMAL_SYNC; legacy80 = s2c_syncobjstate / OBJ_SYNC_STATE. They must not be assumed to be full NPC spawn packets.
- Native92 5-byte NPC request remains deliberately omitted. Legacy c2s_requestnpc is enum73; its NPC_REQUEST_COMMAND contains protocol, UINT ID **and nDeathName[NAME_LENGHT]**, so direct five-byte relabeling is wrong. KProtocol.cpp uses sizeof(NPC_REQUEST_COMMAND); NpcRequestCommand calls NpcSet.SyncNpc, which sends either real NPC sync or request-failure.
- Native initial converter sends only the selected actor; runtime movement path ignores non-local NPC entities, and NPC template indices are not verified Unity model IDs. Presence/update/despawn, request-failure layout, map/region scope and asset-template mapping must be implemented together before enabling requests. No fake NPCs were generated.

## Verification and next gate

- New Lua audit: **10/10 PASS**, after initial test-first run failed because the module was not yet implemented.
- Client Python discovery: **251/251 PASS**, exit0, one preexisting SQLite ResourceWarning.
- Server bridge discovery: **117/117 PASS**, exit0. Initial invocation from inside gs-bridge-m1 failed seven imports due missing parent PYTHONPATH; rerun from Client3D-LAN with PYTHONPATH=root passed, then repeated after log edits passed.
- Gateway startup/greeting verified after restart; source hashes and original client hashes checked.
- No Unity build required: Unity source and gameplay behavior were not edited.

**PENDING**: one user-authenticated QA login, then correlate GS143 → native82 → C2S122(9B) → GS174/native131 in the new log and fresh GS console; only that can demonstrate runtime timestamp acceptance and absence of size/time warnings. Then capture actual scene load key/callback/GET and visible terrain, followed by NPC/AOI and one user click with GS-authoritative position/Unity echo. Existing 90-second/4096-frame/8MiB QA limits remain.

**CN02-04 BLOCKED; CN03 CLOSED.** Ping repair remains deployed but not end-to-end certified; Lua missing source OPEN; scene/NPC/manual movement not accepted.
