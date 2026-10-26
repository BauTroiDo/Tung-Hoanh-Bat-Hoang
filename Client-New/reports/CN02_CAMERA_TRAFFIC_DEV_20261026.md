# CN02 DEV — right-drag camera and recurring disconnect

Date: 2026-10-26, Asia/Bangkok. Branch: cg/cn02-anchor-evidence. Continues d0d3d1a (real Map53 scene / MPS axis repair). **CN02-04 remains BLOCKED; CN03 remains CLOSED.**

## Camera evidence and implementation

User confirmed Alt+left drag works, then requested right mouse drag. Native4016 MobileInputCinemachine.HandlePCInput (RVA0xA3EC70) checks Alt307/308 and mouse0. The source-controlled Client-New OrbitFollowCamera already uses right mouse, but it is not the reference executable running this QA session.

Added tools/reference4016/right_drag_policy.cjs, right_drag_policy.test.cjs, right_drag_qa.js and run_right_drag_qa.py. This is a **temporary in-memory QA adapter**, not a release-client build or GameAssembly file patch. It preserves native camera calculations; scoped Input return changes apply only inside camera input and PlayerClickDetector.Update. Six-pixel drag threshold distinguishes orbit from a stationary right click. Right skill clicks are deferred until release; right hold-to-repeat skill behavior is deliberately not retained during this experiment. Left movement and native Alt orbit stay native.

First runtime attempt failed: generic EventSystem.IsPointerOverGameObject returned true across the full-screen HUD, cancelling all gestures. A read-only follow-up established the false positive. Corrected the integration to use native PlayerClickDetector.CanProcessWorldClick, which calls IsPointerOverBlockingUI and excludes nonblocking HUD hits. Focus loss cancels the gesture; a UI-started gesture remains cancelled until release.

Five gesture regressions PASS, JavaScript syntax check PASS, runner CLI smoke PASS. User then said “được rồi”; matching v2 telemetry recorded122 orbit frames,10 suppressed physical right-down queries and0 script errors. Later active-run telemetry recorded additional orbit frames and deferred clicks. This establishes working right-drag orbit in this QA session; no claim that a complete360-degree sweep or every combat skill was independently verified. No smoothing change was made.

Active Game.exe21188 retains a runner without an automatic duration limit; it detaches on game exit or Ctrl+C. Starting the old QA launcher alone on a later game run will not install this mapping. Reapply using the new actual PID:

```powershell
python tools/reference4016/run_right_drag_qa.py --pid <actual-game-pid> --log .tmp/right-drag-runtime.jsonl
```

Rollback: stop only that adapter runner, or close the QA game normally. It unloads hooks and native Alt+left control returns. Original GameAssembly SHA256 remains011357720b210aa1f392a8b62ff8de090d3a895230a6e0135bce707196871069. No client binary on disk, GS, Bishop or character DB changed for camera testing.

## Recurring disconnect root cause

User screenshot showed reconnect at14:28:42. Gateway log proves **14:28:38.891: ValueError / Native M1 session traffic limit**, matching that event. Many preceding sessions ended with the identical reason, including14:24:39.377 and14:26:46.268. GS4020 remained running since14:05:21; gateway7324 had not crashed. These are gateway policy closures, not evidence that ping timestamps or the LAN link failed.

native_session.py previously accumulated both directions for the whole session, then closed above4096 native frames or8MiB. Normal GS updates eventually exhaust a lifetime budget. The controlled dispatcher also imposed a300-second lifetime, which would cause later reconnects even after removing the first cap.

Changed the controlled DEV policy to8MiB /4096 native frames **per10-second fixed traffic window**, counted before native sends and after validated inbound frames, including original GS batch bytes as before. Reject overload, negative accounting and malformed/unsupported traffic; retain authentication, real ACK, movement validation and real ping pairing. Close after90seconds without received traffic. Controlled dispatcher uses max_seconds=None; the explicit diagnostic max_seconds parameter remains available. Log session age and lifetime traffic counters every60seconds for runtime verification. Fixed windows permit a boundary burst of up to two adjacent window budgets; this is a controlled DEV policy, not a production bandwidth specification.

New7-test suite reproduced1 failure (old300-second dispatch) and6 missing-budget errors, then PASS. Full staged and deployed suites: **136/136 PASS, exit0**. Sustained simulated600-second traffic exceeds both previous lifetime caps; independent burst and idle tests still reject. Readiness refused stale hashes; refreshed only native_session.py and native_gs_dispatch.py pins, retaining world_entry_enabled=true and m1_p0_pass=false.

Backup and test/deployment logs: server Client3D-LAN/qa_runs/cn02-traffic-window-20261026. Source review patch and compact deployment metadata: reports/evidence/cn02-camera-traffic. Restarted the identity-checked gateway7324 to1900 after warning the user that the current session would reconnect. Native greeting34B/prefix22002000 verified. GS4020 and Bishop unchanged, DB untouched. No Unity/GS build required for this Python-only gateway change.

## Runtime acceptance and remaining work

New real session began14:32:49, entity40720, map53. ACK sent once; real GS143→122(9B)→174/131 timestamp pairs resumed. At14:37:49.879 the same entity40720 remained alive at300.1seconds,16177native frames and9169039bytes, exceeding all three old lifetime cutoffs (4096frames,8MiB,300seconds). User confirmed the repeated disconnect stopped. This is a five-minute runtime smoke acceptance for the identified policy defect, not an overnight stability claim. Do not infer connection stability from unit tests or a greeting probe.

Latest GS screenshot also isolates Lua failure: newworldscript_default.lua line42 calls EnterMap:OnPlayerEvent, but actual misc/eventsys/eventsys.lua defines OnEvent and lacks OnPlayerEvent. Recursive source search found callers but no definition. Do not add a no-op or equate missing online/login/logout sources with this specific nil callback. Correct callback/player-context behavior and load order still need verification before patching Lua.

Movement smoothing, actual NPC/AOI conversion, real3+1 landmarks and broader runtime stability remain open. Camera acceptance and fixing one disconnect cause do not satisfy those gates.

Rollback gateway: stop the identity-checked current gateway; restore native_session.py, native_gs_dispatch.py and native-p0-readiness.json from the traffic QA before/ directory; verify readiness, then restart with the saved prior options. The traffic regression intentionally fails against the old policy. No GS/Bishop restart or character reset is required.

New user steering: runs forward then snaps back. Rubber-banding remains OPEN; inspect authoritative/forced-position and local prediction together before changing interpolation or server validation.

