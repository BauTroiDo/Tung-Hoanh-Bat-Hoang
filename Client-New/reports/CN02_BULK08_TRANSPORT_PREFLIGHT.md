# CN02-BULK-08 — Map53 isolated transport preflight and gateway diagnosis

Date: 2026-10-26, Asia/Ho_Chi_Minh. Client: DESKTOP-TPG2RNF. Server: WIN-33R6OP5GO1M. Starting checkpoint: 5cd3b69.

**Status: OFFLINE/ISOLATED CHECKS PASS; LIVE GAMEPLAY BLOCKED.** CN02-04 BLOCKED, CN03 CLOSED.

## 1. Client-side isolated HTTP check

New tools/reference4016/map53_qa_preflight.py reads the original and staged metadata with pinned SHA256:
- Installed original and backup: 5586F416AA26EBE57EA7F6580D0A53E5E5F29C50E425E64A4D3F2A95D6069E08.
- Staged loopback variant: C677837DEC676A3F03A33333A9B94620ED373962E844704D6B9D1E451BFE1987.

It imports but does not change the original LAN-LOCAL/m1_map53_asset_server.py, redirects its LOG in-memory outside Git, opens a 127.0.0.1 ephemeral test port, tests eight HTTP requests, then closes the isolated listener. No original game executable, server packet, PAK/cache or UnityProject is changed.

| Endpoint request | Result |
|---|---|
| catalog hash HEAD | 200, 32 bytes |
| catalog hash GET | 200, 32 bytes |
| Addressables settings GET | 200, 3,630 bytes, StandaloneWindows64 |
| official PC BaLangHuyen assets HEAD | 200, 31,592,781 bytes |
| PC assets bytes=0-6 | 206, UnityFS, Content-Range correct |
| official PC BaLangHuyen scenes HEAD | 200, 9,738,990 bytes |
| PC scenes bytes=0-6 | 206, UnityFS, Content-Range correct |
| bogus resource GET | 404 |

**8/8 synthetic loopback HTTP checks PASS**; the mirror enumerates 1,127 local sources, 7 official staged and 1,120 cached UnityFS files (the latter have not all been fully hashed). Raw eight request records are outside Git at Tool/_outputs/cn02-bulk/map53-runtime-preflight/isolated-v2/mirror-http-events.jsonl.

The verified native-field projection remains MapCopyID -> nTemplateId, SubWorld -> nMapId, Region -> nRegion. In the *specific historical Map53 example*, values (53,53,6488114) project to LoadMap(53,53,6488114,0,0). A hypothetical MapCopyID=0 produces template ID 0, not 53. These IDs must not be treated as universally identical.

## 2. Fresh read-only inspection of original server

Authorized Desktop Commander device WIN-33R6OP5GO1M was inspected **read-only**. No configuration changes, service restarts, server source edits, game client launches or character/DB changes.

At server inspection:
- Bishop_20261022.exe PID 3212 listening on 56722.
- gs001.exe PID 1608 listening on 51189.
- Gateway C:\Server-Game\SwordOnline\Client3D-LAN\gateway.py NOT listening on 14180.

At the later client connectivity test, TCP 56722 and 51189 were reachable; 14180 was not. These checks reflect only instantaneous port reachability, not completed Bishop auth or in-game ACK.

**The historical MapCopyID=0 diagnosis is stale as a statement about current source.** Current server file C:\Server-Game\SwordOnline\Client3D-LAN\gs-bridge-m1\gs_world_map53_name_preview.py already implements MapCopyID=53 at native offset 14 via struct.pack_into('<i', data, 14, old_map_id). The native_initial_packets.py conversion uses that helper and obtains packed Region from real actor coordinates. The helper still reports production_ready=False, since complete runtime parity is unverified.

Ran **28/28 read-only server Python tests PASS**, exit 0:
- test_gs_world_map53_name_preview
- test_native_map_region
- test_gs_world_scalar_projection

First invocation had missing PYTHONPATH and one import error; rerun with C:\Server-Game\SwordOnline\Client3D-LAN on PYTHONPATH passed all 28. The server native_p0_readiness.evaluate() returned status CONTROLLED_MAP53_PROTOCOL_TEST_READY, world_entry_enabled=True, stale_source=none. This indicates source integrity/readiness for a *controlled test*, not actual player or Map53 runtime success.

**Gateway activation still needs an explicit controlled action.** gateway.py defaults --native-gs-bridge OFF. The existing Manage-Gateway.ps1 start action omits the --native-gs-bridge option, so simply starting it in default mode does not enable native GS relay. The --npc-map53-preview and --npc-name-preview flags affect NPC preview and are not substitutes for the native GS flag. The old gateway.pid file is not evidence that the process is alive.

No gateway was started, modified or restarted. No server process/DB was touched.

## 3. Checkpoint, tests and limitations

Files created/changed in the Client-New ChatGPT-owned lane:
- tools/reference4016/map53_qa_preflight.py
- tools/reference4016/verify_map53_qa_preflight.py
- reports/evidence/cn02-4016-il2cpp/map53-qa-preflight-summary.json
- reports/CN02_BULK08_TRANSPORT_PREFLIGHT.md
- docs/CHECKPOINT.md, docs/NEXT_SESSION_HANDOFF.md, TASK_BOARD.md

New isolated preflight suite: **20/20 PASS**. Existing CN02 suites at the previous checkpoint: 256/256. Rerun all before final commit (expected 276/276 client static/isolated tests). Server 28/28 are independent and **must not** be added to that client test count.

The test proves only: metadata unchanged, local mirror responds correctly for the exact official Map53 PC bundles, prior native selector mapping, and observed backend port status. HTTP mock traffic is NOT a real Unity scene load. Historical event documents cannot be spliced into a new authenticated session. No live client/avatar/movement/affine matrix has been captured.

## 4. Remaining real QA steps

When an authorized operator is present:
1. Verify gateway bind/allowlist, currently listening services and the precise native-GS activation path; keep Bishop and original gs001 untouched. Do not launch default Manage-Gateway and claim the native path is enabled.
2. Ensure one original client QA session with sanitized pseudonymous session/actor; original metadata hash and optional localhost mirror state must be tracked with rollback.
3. Capture the actual Bishop permit, GS sync/ACK, WORLD_SYNC SubWorld/MapCopyID/Region, LoadMap arguments, exact selected catalog scene key, verified loaded Unity scene and visibly spawned same avatar.
4. Observe user-initiated movement AFTER Map53 scene-ready, original GS-authoritative ACK and client position echo.
5. Record six active Map53 affine values and 3+1 genuine same-scene original MPS / Unity landmark pairs; independently review holdout error.
6. Apply tools/reference4016/check_single_session.py and review raw source logs/screenshot separately.

**CN02-04 BLOCKED, CN03 CLOSED; no gameplay acceptance claimed.** Eight pre-existing UnityProject material edits owned by Antigravity are preserved.
