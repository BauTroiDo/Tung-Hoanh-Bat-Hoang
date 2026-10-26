# CN-01 Acceptance / Implementation Handoff

**Status:** REVIEW; implemented and automatically verified, not independent/user acceptance.
**Owner:** Codex/ChatGPT. **Branch:** cg/cn01-offline-movement.
**Baseline:** b0dfacc (CN-00 signed off). **Date:** 2026-10-25, Asia/Bangkok (UTC+07).

## Delivered behavior

MovementTest scene has synthetic ground (Ground layer 8), a capsule avatar prefab, solid collision wall, follow/orbit/zoom camera and a uGUI input shield/status panel. Left click or primary touch on valid ground sets one offline destination. UI, offscreen/non-finite input and missing/non-ground colliders do not create movement. CharacterController moves at 4 units/second, stops at walls and refuses unsupported next steps. Metrics report actual speed and Idle/Walking/Blocked state.

This is straight-line offline movement with collision stopping; no automatic route around obstacles, skeletal animation clips, Map53 or server authority is implemented. MotionState is an animation-facing state only. Touch input is implemented but not verified on a physical mobile device (CN-06).

Source: new Movement assembly (OfflineMovement, OrbitFollowCamera, MovementHud), Cn01ProjectSetup editor generator, PlayMode assembly/tests, MovementTest.unity, OfflinePlayer.prefab and three generated materials. Existing Editor asmdef gains Movement/InputSystem/uGUI references. CN00 test now verifies Bootstrap is enabled in build settings rather than requiring it to remain scene index 0; MovementTest starts the new build, and Bootstrap is still included. CN00 source/scene/build output remains intact.

## Test/build evidence

| Evidence | Actual result |
|---|---|
| Initial behavioral red | 6 PlayMode tests: 4 failed, 2 passed against minimal unimplemented API |
| Tiny-step regression red | TinyFrameStepIsNotDiscardedByController failed before fix |
| Mouse pipeline diagnostic red | Virtual mouse disabled by Editor focus policy; accepted intents 0 |
| Final PlayMode | 9/9 passed, 0 failed/skipped, Unity process exit 0 |
| Final EditMode | 3/3 passed, 0 failed/skipped, Unity process exit 0 |
| Win64 build | Succeeded, errors 0, warnings 0, process exit 0; 104188338 bytes |
| Standalone batch smoke | exit 0; start (0,0,-3), end approximately (-2.93,0.08,-1.04) |
| Standalone visible smoke | exit 0; distance to requested target 0.113; real PNG opened and visually inspected |
| Asset .meta audit | 0 missing companions under Assets/_JXClient |

Final XML, red XML, actual screenshot, source/build hashes and runtime excerpts: `reports/evidence/cn01/`.

Actual Unity commands (PowerShell; all executed against this project):

```powershell
$editor = 'C:/Program Files/Unity/Hub/Editor/6000.6.7f1/Editor/Unity.exe'
$project = 'E:/Games/VLTK3D/SwordOnline/Client-New/UnityProject'
& $editor -batchmode -nographics -projectPath $project -runTests -testPlatform PlayMode -testResults E:/Games/VLTK3D/SwordOnline/Client-New/.tmp/cn01-playmode.xml -logFile E:/Games/VLTK3D/SwordOnline/Client-New/.tmp/cn01-playmode.log
& $editor -batchmode -nographics -projectPath $project -runTests -testPlatform EditMode -testResults E:/Games/VLTK3D/SwordOnline/Client-New/.tmp/cn01-editmode.xml -logFile E:/Games/VLTK3D/SwordOnline/Client-New/.tmp/cn01-editmode.log
& $editor -batchmode -nographics -quit -projectPath $project -executeMethod JXClient.Editor.Cn01ProjectSetup.Generate -logFile E:/Games/VLTK3D/SwordOnline/Client-New/.tmp/cn01-generate.log
& $editor -batchmode -nographics -quit -projectPath $project -executeMethod JXClient.Editor.Cn01ProjectSetup.Build -logFile E:/Games/VLTK3D/SwordOnline/Client-New/.tmp/cn01-build.log
```

Generation ran before full scene tests. Production fix was then retested and rebuilt; final test-only virtual input addition did not change runtime assembly. Standalone commands:

```powershell
$exe = 'E:/Games/VLTK3D/SwordOnline/Client-New/Builds/CN01-Windows/JXClientNew.exe'
& $exe -batchmode -cn01SmokeTest -screen-width 1280 -screen-height 720 -screen-fullscreen 0 -logFile E:/Games/VLTK3D/SwordOnline/Client-New/.tmp/cn01-runtime-batch.log
& $exe -cn01SmokeTest -cn01Screenshot E:/Games/VLTK3D/SwordOnline/Client-New/.tmp/cn01-runtime-visible.png -screen-width 1280 -screen-height 720 -screen-fullscreen 0 -logFile E:/Games/VLTK3D/SwordOnline/Client-New/.tmp/cn01-runtime-visible.log
```

Actual runs used Start-Process with the corresponding arguments and recorded process exits. Visible render required a normal window; hidden rendered runs could not save screenshots. Two native Computer Use capture attempts timed out; those attempts are not visual proof. The accepted PNG was produced by the visible player itself and opened with the image viewer.

## Defect found and fixed

Initial standalone smoke failed exit 2 despite passing coarse-step tests. CharacterController's default minMoveDistance 0.001 discarded high-frequency tiny deltas. A real PlayMode regression with dt=0.00001 reproduced it. Movement now owns minMoveDistance=0 before moving. Regression and standalone smoke then passed.

The virtual mouse test separately failed because the Editor disabled the device on lost focus. Test temporarily sets IgnoreFocus/AllDeviceInputAlwaysGoesToGameView, enables its virtual mouse, and restores both settings/removes the device in finally. It verifies a queued MouseState reaches real Update and creates exactly one intent. No user OS input, focus policy or system setting was changed by that fixture.

Runtime still logs stripped/unsupported URP DOF/Panini shader warnings inherited from template configuration; Bootstrap/MovementTest do not use these effects. BuildSummary's zero warnings does not imply warning-free runtime.

## Gate matrix

| ID | Automated evidence | Status |
|---|---|---|
| CN01-01 | Real scene load verifies Ground raycast, player controller, camera target, uGUI shield | PASS |
| CN01-02 | GroundClickMovesAtBoundedSpeed + MouseInputEventReachesUpdateAndCreatesOneIntent | Automated PASS; human physical click not witnessed |
| CN01-03 | UiPanelConsumesClick uses actual EventSystem/GraphicRaycaster | PASS |
| CN01-04 | MissingGroundRejectsInput and unsupported next-step test | PASS |
| CN01-05 | WallStopsPlayerWithoutPenetration + standalone target traversal | PASS for synthetic test scene |
| CN01-06 | Three yaw/pitch/zoom configurations produce real ground ray hits | PASS |

**Pending before overall gate PASS:** independent review and direct user click confirmation in the playable EXE. Automated smoke directly invokes the pointer API; the separate virtual MouseState test exercises Input System. Neither is represented as a human click. Do not begin CN-02 until sign-off.

## Safety, provenance, rollback

No production server or reference client launched/modified. Both Game.exe/GameAssembly.dll reference SHA256 values still match CN00 baseline (verification.json). No transport, credentials, copied proprietary assets or new packages. Synthetic primitive geometry/materials and first-party code only; manifest updated. No push; repository has no remote.

CN00 build remains at Builds/Windows. New playable build: Builds/CN01-Windows (run the EXE with its entire sibling Data/Mono directories, not the launcher alone). Launcher SHA256 matches CN00 because Unity shares that launcher; Movement DLL and scene-data hashes in verification.json identify this build.

Rollback: preserve this branch and return to baseline b0dfacc in a separate checkout when needed; do not hard-reset or clean a shared worktree. Review changes with git diff b0dfacc..HEAD. Build artifacts are ignored by Git and can be regenerated with the commands above.

Final diff note: Unity serialized TagManager from version 2 to 3 while registering Ground layer 8; empty rendering-layer padding was normalized by the Editor, existing named light layers preserved. Final runtime distance 0.113. Final build repeated after newline normalization and passed; no semantic runtime changes after this build.
