# CN02 Map53 movement rubber-band — DEV partial hardening (2026-10-26)

## Status
**Partially mitigated; rubber-band NOT ACCEPTED. CN02-04 BLOCKED / CN03 CLOSED.** The fixes below are in the Python native GS gateway only. Original GS executable, Bishop, character database and 4016 binary were NOT modified. User authorized DEV restarts.

## Evidence before patch
- Active QA: Game.exe PID21188 on DESKTOP-TPG2RNF; GS PID4020, Bishop PID3212 on WIN-33R6OP5GO1M.
- Four genuine GS-forcedPosition (native opcode120 / legacy206) at 14:46:57.826 x102528,y107639 twice and 14:46:58.392/.443 x102532,y107646 twice, entity40720. Client trace: .tmp/cn02-dev-resume-20261026/rubber-band-long-trace.json; the GS is authoritative, do not filter those frames.
- Passive observer recorded native sendRun target updates at about50-67ms and corresponding GS run echoes. Repeated identical command coordinates appeared at collision/stalled endpoints. Legacy handler KProtocolProcess.cpp NpcRunCommand calls Npc.SendSerCommand(do_run, X,Y); duplicate targets need not be reissued every50ms.
- MapDataService on native client reports loaded collision data (1496 region dictionary entries) but samples in Map53 region100/105 returned zero. This is **not proof** that GS obstacles and client data are identical or that GS barrier caused any particular correction. Map collision parity remains open.
- Gateway initial post-first-restart session terminated at15:10:28.244 with `Movement exceeds the M1-P0 test range`: an extreme/stale move closed an otherwise authorized session. This is a separate session handling defect; do NOT enlarge the4096 legacy-unit guard.

## Code change and safety
Server location `C:\Server-Game\SwordOnline\Client3D-LAN\gs-bridge-m1`:
- `native_movement.py`: `DuplicateMoveGuard` suppresses identical converted legacy commands for 0.2 seconds; every changed run/walk target still forwards. `bounded_client_movement` keeps existing map, packet, identity and distance guards; when (and only when) a movement exceeds the verified4096 range, return no command rather than disconnecting the whole session.
- `native_session.py`: applies only post-ACK; counts filtered duplicates and rejected far moves, and logs actual GS forced correction with most recent accepted target and elapsed milliseconds. Other malformed/unsupported native packets still fail closed. Real GS authoritative position packets still flow unchanged; no fake position, teleport, collision disablement or source-level gameplay shortcut.
- `test_native_move_duplicate.py`:8 repeat tests +4 bounds/security cases.
- Python unit discovery: **148/148 PASS, exit0**. Readiness first failed for stale hashes, then two exact source-hash pins updated; `world_entry_enabled=true`, `m1_p0_pass=false`.

## Deployment and rollback
- Server QA backup: `C:\Server-Game\SwordOnline\Client3D-LAN\qa_runs\cn02-move-repeat-20261026`.
- Its `native_movement.before.py` SHA256 matches old manifest aa5370d994e7966e1b89bf7fe65647d009ad492a33f5365f43f3e5122bdd21e4; `native_session.before.py` matches 4a5ff8d531cf4be27e1e25b06a7f5331f881f3b8d74c1433bfce36923ecb4f72. `manifest-before.json` is preserved. Both reconstructed originals verified byte-for-byte by digest before deployment.
- First gateway restart: PID1900 ->6896. Initial reconnect revealed the stale-range disconnect.
- Second restart with safe stale-move rejection: PID6896 -> **3828**, port14180 listening; `--native-gs-bridge --allow 192.168.1.13`. Runtime log `qa_runs/cn02-move-repeat-20261026/gateway-rangeguard.log`. No GS/Bishop restarts.
- Runtime post-deploy session entity40723 established. At 15:13:59.708 gateway forwarded native(100596,104367) to legacy(50298,104367), initial GS legacy position(50298,104351). At15:13:59.736 GS issued two *real* forced-position packets to (52032,101696), a **large difference from initial/sent coordinates immediately after reconnect**; these were forwarded without suppression. This is a concrete new root-cause lead involving login/spawn or map/position reconciliation. Do not assert that filtering duplicates fixed this forced teleport.
- Rollback: close QA client, ensure no ESTABLISHED session, identity-check current gateway PID; stop only gateway; replace live two .py from .before.py and manifest from manifest-before.json; verify readiness gate, restart native bridge with QA allowlist and separate log. Do NOT restore only one source file or delete/replace character DB.

## Next blockers
1. Trace GS why it emits legacy206 immediately post-ACK at (52032,101696) after initial position (50298,104351), including newworld/login script and position fix flags. Preserve GS authoritative validation; do not hide forcedPosition.
2. Compare region-specific actual GS barrier grid with native client region100/105 source; client all-zero samples alone are insufficient.
3. Repeat real walking tests through prior problematic spots, verify no 38-unit scene divergence, no repeated forced correction and stable GameServer position over several minutes. **No user acceptance claim yet.**

## Post-deploy observed progress (15:17:50)

Same entity40723 remained connected at 60.1s,120.1s and **180.3s**, with **6108 frames and 2,190,231 traffic bytes**. Gateway port14180 still had one ESTABLISHED session. Native run packets forwarded beyond101 with GS authoritative positions changing; one identical repeat was suppressed at15:14:36.340. Since the two immediate post-login forced corrections at15:13:59.736, the filtered log had no additional forced-position entry through15:17:50. This is encouraging runtime evidence for the relay/session change, **not evidence that collision mismatch or user-visible rubber-banding is resolved across prior problem routes**. User has not yet confirmed visual behavior after this patch.
