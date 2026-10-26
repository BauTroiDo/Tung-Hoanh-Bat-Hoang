# CN02 DEV — scene and movement recovered by MPS unit conversion

Date: 2026-10-26, Asia/Bangkok (UTC+7). User-authorized DEV modification/restart.

## Observed result

After deployment and a new user login, **real PC_BLH_Bake terrain/buildings became visible and the user confirmed click movement works**. User supplied a new screenshot showing the real courtyard, roofs, trees and player. The same QA process recorded entity **40714**, Map53, loaded PC_BLH_Bake, visible terrain renderer samples, local player position about **(-3, 0, 14.2)** and camera **(2.63, 5.5, 17.45)**. User movement sequence was12 at the snapshot. Gateway subsequently recorded **163 movement forwards** and GS positions changing in this same entity/session, e.g. legacy (51820,101674) → (51883,101443), plus a genuine ping roundtrip at14:00:17. No actor teleport, affine calibration edit or fabricated GS ACK was used.

**Limits:** movement is visibly working but user reports stutter. Camera360 is a new follow-up request. NPC/AOI remains omitted. Three-plus-one independent calibration landmarks remain missing. **CN02-04 BLOCKED; CN03 CLOSED.** This is a specific coordinate-unit repair, not acceptance of all gameplay.

## Root cause proved

Actual DEV GS source `KRegion.h` defines region width512, height1024, and16×32 cells of32. `KSubWorld.cpp::Mps2Map` shifts X by9 and Y by10. Original4016 metadata defines KRegion width1024, height1024 and32×32 cells of32. The gateway had copied MPS X unchanged across these distinct unit systems and computed the initial native region with legacy X/1024.

Before repair, real loader fields already said BaLangHuyen / PC variant / Addressables loaded / map visible / no in-flight load. Real scene inventory contained PC_BLH_Bake with eight active roots. Player entity40710 was at approximately **(247.75,0,-373.56)**; camera followed it outside the four terrain tiles (nominal X[-253.92,246.08], Z[-247.98,252.02]). Terrain sample renderers were not visible. Live six affine coefficients were captured rather than assumed:

`m00=-2.4063999653, m01=3.5840001106, m10=3.8912000656, m11=3.3280000687, offsetX=-225.5200042725, offsetZ=-1433.8399658203`.

MapDataService bounds were regionX74..117, regionY83..116, template53, country/balanghuyen. Legacy X51889/512 is region101, while the erroneous native X51889/1024 is region50, outside that loaded region range. Doubling X at the wire boundary preserves the actual region; no new fitted coefficients are needed.

Read-only physics queries did **not** establish a ground collider: the vertical hit at the player was its own collider, layer17, Y2. The nearby off-body ray missed, as did a diagnostic origin ray. All eight PC scene roots returned zero Collider components in the examined instance. Do not interpret the avatar collider hit as ground readiness or add arbitrary colliders: native input can use another ground/path representation. Before repair gameplay/UI input locks were false but userCommandSeq remained0. The reported earlier clicks were not correlated to a fresh outbound command; reconnect-time automatic moves were excluded from acceptance.

## Minimal gateway change

Changed only native_initial_packets.py, native_movement.py and native_session.py plus tests/readiness proof:

1. Outbound MPS: nativeX=2×legacyX, nativeY=legacyY, with signed-coordinate overflow guards.
2. Initial NPC_SYNC coordinates use the independently measured packed wire offsets53/57. WORLD_SYNC.Region is computed from converted native MPS, preserving GS region identity. Gateway authoritative summaries stay in legacy units.
3. Native walk/run destinations convert back with legacyX=floor(nativeX/2) before the existing4096 legacy-unit distance guard. Odd native X loses at most0.5 legacy MPS; Y is unchanged.
4. Authoritative position and walk/run target echoes use native units. Fixed-point X offsets are doubled modulo32768; their fractional carry is added to integer native MPS X. Y offsets remain unchanged.
5. Corrected the old region regression's erroneous expectation: fixture legacy(52032,101696) now gives native(104064,101696) and packed region0x00630065, rather than0x00630032.

Before implementation the new12-test suite reproduced five assertion failures and five errors from missing conversion/helper behavior; two guards already passed. Staged and deployed **129/129 bridge tests PASS**, exit0. Tests cover initial region/actor, run/walk inverse, odd X, range limits, overflow, fractional carry, authoritative summary units and target identity/length.

Readiness rejected stale source after copy. Only the three changed source hashes were refreshed; readiness then returned world_entry_enabled=true and **m1_p0_pass=false**. Deployed hashes and zero-context review patch are in reports/evidence/cn02-mps-units/deployment.json and mps-units.patch. No Unity/GS build required: no Unity source or native GS binary changed.

## Deployment and rollback

QA area: `C:\Server-Game\SwordOnline\Client3D-LAN\qa_runs\cn02-mps-units-20261026` contains stage, red/green/deployed test logs, source patch, deployment.json and **before/** backups for all three source files, readiness manifest and prior region test.

Attempted normal window closure; client shutdown log confirmed Physics cleanup and Input System Shutdown, and there were no GS network sockets. The residual nonresponding Game.exe31732 process, holding only local instrumentation sockets, was then stopped by exact PID/path check. Gateway restart was guarded by zero Established sessions on14180 and verification of PID4088's command. New gateway **PID7324**, log gateway-units.log, still `--native-gs-bridge --allow 192.168.1.13`. Native greeting34B / prefix22002000 verified after restart. New QA Game.exe **PID21188**, launcher6360; original binary hashes match checkpoint. Bishop3212 and GS4168 unchanged; DB untouched.

Rollback: close QA normally and verify no active gateway session; stop the verified current gateway; restore native_initial_packets.py, native_movement.py, native_session.py, native-p0-readiness.json and test_native_map_region.py from before/. The new unit regression will intentionally fail against the pre-fix behavior. Verify readiness and start with saved gateway-before-command.txt options and a fresh log. Do not restart GS/Bishop or reset a character for rollback.

## Ping, Lua, stability and next work

Independent prior-run extraction proved **37 real timestamp roundtrips** GS143 → C2S122 nine bytes → GS174/native131, matching server and client ticks. First entity40707 tick333135/client3482894; another tick334001/client3528360. Ping remains working after the units repair: entity40714 tick355001/client231280. The earlier ping candidate padding discussion is superseded by actual paired-token evidence.

Repeated CDN failure in the observed loaded-map run was specifically **/upload/xaphu_routes.json**; it was not a missing terrain bundle request. The scene was already loaded from Addressables/cache. Do not keep treating catalog-only mirror logs as proof of absent geometry. Scene inventory and actor position now explain the blue-world defect.

Lua: missing online.lua/playerlogin.lua/playerlogout.lua sources remain as diagnosed in CN02_DEV_RUNTIME_RESUME_20261026.md. No substitute hooks were created. NPC requests and nonlocal AOI remain unimplemented, so NPC absence is not fixed by this patch.

Prior gateway trace also recorded six traffic-limit disconnects and one unsupported-command disconnect. Existing QA caps remain, and can disrupt longer sessions; ping success does not fix those policies.

Follow-up: measure rendered motion against native position/forced-position callback timing before changing reconciliation. For camera, static native HandlePCInput code tests Unity KeyCode308/307 (LeftAlt/RightAlt) plus mouse button0; test Alt+left drag and capture actual camera yaw before changing camera settings. User clarified the request is camera orbit around the avatar.

## Follow-up live boundary — GS restart and camera/input measurement

At14:05:21 GS PID changed from4168 to4020; Bishop remains3212. This restart was **not initiated by this task**. The entity40714 scene/movement proof above is from the earlier GS4168 session; later entities40707..40711 are separate GS4020 sessions and are not spliced into that acceptance. The new source/manifest still pass129/129 tests and readiness remains controlled-ready/m1_p0_pass=false.

Camera clarified by user: orbit around the avatar. User tried Alt+left drag and reported no orbit. A later bounded45s capture found MobileInputCinemachine enabled, orbital/input controllers present/enabled, no transition coroutine, HorizontalAxis[-180,180], Wrap=true, Restrictions=0, sensitivity0.5, horizontalValue=-120. HandlePCInput ran every frame but SetInputValues count stayed0;7166 observed Alt checks returned false and no left-button true return was seen. Therefore **the capture did not overlap an active gesture**; it does not establish a broken orbit range or justify altering it. Fresh user gesture capture remains pending.

A separate30s trace after GS4020 restart observed1040 frames, entity40707,508 normal-position callbacks(opcode90), median sampled frame interval33ms,85 moving frames and maximum frame displacement0.088 units. Most samples were stationary; this is **insufficient active-movement evidence to diagnose user-reported stutter**. No smoothing, camera setter or input-hook replacement was installed. All bounded observers detached. Detailed compact summaries are under reports/evidence/cn02-mps-units; raw frame data remains in .tmp outside Git.
