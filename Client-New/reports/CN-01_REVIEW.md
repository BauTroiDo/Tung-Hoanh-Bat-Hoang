# CN-01 Independent Review Report — Offline Movement, Camera, Input Shielding & Colliders

**Gate/task:** CN-01 (`b0dfacc..755747f`)
**Implementer:** Codex / ChatGPT (`cg/cn01-offline-movement`)
**Independent Reviewer:** Antigravity (`ag/cn01-review-cn02-world`)
**Environment/timezone/timestamp:** Windows 10/11 x64 (`DESKTOP-TPG2RNF`, GPU NVIDIA GeForce GTX 1080), `Unity 6000.6.7f1 (b22767f04f04)`, Asia/Ho_Chi_Minh, 2026-10-25T15:56:15+07:00
**Reviewed source commit:** `755747f` (handoff commit `d08789a`)
**Review Decision:** **PASS**

## 1. Scope & Delta Reviewed (`b0dfacc..755747f`)

Reviewed all files added/modified by `755747f`:
- `UnityProject/Assets/_JXClient/Movement/JXClient.Movement.asmdef`
- `UnityProject/Assets/_JXClient/Movement/OfflineMovement.cs`
- `UnityProject/Assets/_JXClient/Movement/OrbitFollowCamera.cs`
- `UnityProject/Assets/_JXClient/Movement/MovementHud.cs`
- `UnityProject/Assets/_JXClient/Movement/Materials/{Avatar,Ground,Wall}.mat` (+ `.meta`)
- `UnityProject/Assets/_JXClient/Prefabs/OfflinePlayer.prefab` (+ `.meta`)
- `UnityProject/Assets/_JXClient/Scenes/MovementTest.unity` (+ `.meta`)
- `UnityProject/Assets/_JXClient/Editor/Cn01ProjectSetup.cs` (+ `.meta`)
- `UnityProject/Assets/_JXClient/Editor/JXClient.Editor.asmdef`
- `UnityProject/Assets/_JXClient/Tests/EditMode/Cn00BootstrapTests.cs`
- `UnityProject/Assets/_JXClient/Tests/PlayMode/JXClient.Tests.PlayMode.asmdef` (+ `.meta`)
- `UnityProject/Assets/_JXClient/Tests/PlayMode/OfflineMovementTests.cs` (+ `.meta`)
- `UnityProject/Assets/_JXClient/Tests/PlayMode/Cn01SceneTests.cs` (+ `.meta`)
- `UnityProject/ProjectSettings/TagManager.asset` & `EditorBuildSettings.asset`

### Code & Architecture Verification
1. **Clean Separation & No Premature Networking:** `JXClient.Movement` depends only on `Unity.InputSystem` and `Unity.ugui`. No fake server sockets, credentials, or mock GameServer authority exist.
2. **Input & UI Shielding (`CN01-02`, `CN01-03`):** `OfflineMovement.TryPointer` validates finite screen coordinates within `ViewCamera.pixelRect`, queries `EventSystem.current.RaycastAll` for any `GraphicRaycaster` hit first, and rejects clicks over UI panels (`"UI consumed click"`).
3. **Surface & Obstacle Raycast (`CN01-01`, `CN01-04`, `CN01-05`):** Physics raycast inspects the nearest physical surface across `Physics.DefaultRaycastLayers` so walls/obstacles block clicks behind them, then verifies the hit collider is on `GroundMask` (Layer 8 `Ground`) with `normal.y >= 0.5f`. Before each `CharacterController.Move` step, downward ground support at `next + Vector3.up` is verified so the player cannot walk off unsupported edges.
4. **Tiny-Step Controller Threshold Fix (`minMoveDistance = 0`):** Explicitly verified in `OfflineMovement.Tick` (`controller.minMoveDistance = 0`) and covered by `OfflineMovementTests.TinyFrameStepIsNotDiscardedByController`. High-FPS standalone frames are not discarded.
5. **Camera Orbit & Zoom (`CN01-06`):** `OrbitFollowCamera` clamps pitch `[25..75]` and distance `[6..24]` and updates transform pose before raycasting.
6. **Asset `.meta` Companion Audit:** 0 missing `.meta` files across all assets under `UnityProject/Assets/_JXClient`.

## 2. Independent Test, Build & Runtime Re-execution

All commands were executed independently by Antigravity against `E:/Games/VLTK3D/SwordOnline/Client-New/UnityProject`:

1. **EditMode Test Suite (`3/3 PASS`, exit code `0`):**
   ```powershell
   & "C:\Program Files\Unity\Hub\Editor\6000.6.7f1\Editor\Unity.exe" -batchmode -nographics -projectPath "E:\Games\VLTK3D\SwordOnline\Client-New\UnityProject" -runTests -testPlatform EditMode -testResults "E:\Games\VLTK3D\SwordOnline\Client-New\.tmp\cn01_review_editmode.xml" -logFile "E:\Games\VLTK3D\SwordOnline\Client-New\.tmp\cn01_review_editmode.log"
   ```
   - Artifact saved: `reports/evidence/cn01/review-editmode.xml` (`total=3, passed=3, failed=0, skipped=0`).
2. **PlayMode Test Suite (`9/9 PASS`, exit code `0`):**
   ```powershell
   & "C:\Program Files\Unity\Hub\Editor\6000.6.7f1\Editor\Unity.exe" -batchmode -nographics -projectPath "E:\Games\VLTK3D\SwordOnline\Client-New\UnityProject" -runTests -testPlatform PlayMode -testResults "E:\Games\VLTK3D\SwordOnline\Client-New\.tmp\cn01_review_playmode.xml" -logFile "E:\Games\VLTK3D\SwordOnline\Client-New\.tmp\cn01_review_playmode.log"
   ```
   - Artifact saved: `reports/evidence/cn01/review-playmode.xml` (`total=9, passed=9, failed=0, skipped=0`):
     - `Cn01SceneTests.RealMovementSceneHasGroundPlayerCameraAndShield`: Passed (`0.131259s`)
     - `OfflineMovementTests.CameraOrbitAndZoomKeepGroundRaycastValid`: Passed (`0.009779s`)
     - `OfflineMovementTests.GroundClickMovesAtBoundedSpeed`: Passed (`0.021234s`)
     - `OfflineMovementTests.InvalidInputAndGroundEdgeCannotMovePlayer`: Passed (`0.003728s`)
     - `OfflineMovementTests.MissingGroundRejectsInput`: Passed (`0.002023s`)
     - `OfflineMovementTests.MouseInputEventReachesUpdateAndCreatesOneIntent`: Passed (`0.044637s`)
     - `OfflineMovementTests.TinyFrameStepIsNotDiscardedByController`: Passed (`0.003539s`)
     - `OfflineMovementTests.UiPanelConsumesClick`: Passed (`0.010335s`)
     - `OfflineMovementTests.WallStopsPlayerWithoutPenetration`: Passed (`0.024547s`)
3. **Independent Win64 Rebuild (`exit code 0`):**
   ```powershell
   & "C:\Program Files\Unity\Hub\Editor\6000.6.7f1\Editor\Unity.exe" -batchmode -nographics -quit -projectPath "E:\Games\VLTK3D\SwordOnline\Client-New\UnityProject" -executeMethod JXClient.Editor.Cn01ProjectSetup.Build -logFile "E:\Games\VLTK3D\SwordOnline\Client-New\.tmp\cn01_review_build.log"
   ```
   - Log: `[CN-01] BUILD result=Succeeded errors=0 warnings=0 bytes=104188338`
   - `Builds/CN01-Windows/JXClientNew_Data/Managed/JXClient.Movement.dll` SHA256: `97777C814FFFDEA3E867E372AD79A41BE3C4A8A66396124A19C72BB66EEDF06C`
   - `Builds/CN01-Windows/JXClientNew_Data/level0` SHA256: `244BFB726C1D8A61BF1B9C974BF9089517C81075A46BCE9E61C90EE5F7A9BE87`
4. **Independent Win64 Visible Runtime Smoke (`exit code 0`):**
   ```powershell
   & "E:\Games\VLTK3D\SwordOnline\Client-New\Builds\CN01-Windows\JXClientNew.exe" -cn01SmokeTest -cn01Screenshot "E:\Games\VLTK3D\SwordOnline\Client-New\.tmp\cn01_review_postbuild.png" -screen-width 1280 -screen-height 720 -screen-fullscreen 0 -logFile "E:\Games\VLTK3D\SwordOnline\Client-New\.tmp\cn01_review_postbuild.log"
   ```
   - Runtime trace:
     - `[CN-01] INPUT accepted=1 screen=(536.37, 390.72) destination=(-3.00, 0.00, -1.00) source=pointer`
     - `[CN-01] AUTOMATED_SMOKE start=(0.00, 0.00, -3.00) accepted=True target=(-3.00, 0.00, -1.00)`
     - `[CN-01] AUTOMATED_SMOKE end=(-2.93, 0.08, -1.04) distance=0.113 passed=True`
   - Visual verification: `reports/evidence/cn01/review-runtime.png` opened and inspected; displays green walkable ground, capsule avatar at `(-2.93, 0.08, -1.04)`, grey collision wall, and top-left HUD shield.

## 3. Gate Matrix Evaluation (`CN01-01..CN01-06`)

| ID | Criterion | Evidence | Status |
|---|---|---|---|
| CN01-01 | Scene `MovementTest` has GroundCollider (`Ground` layer 8) | `Cn01SceneTests.RealMovementSceneHasGroundPlayerCameraAndShield` + `level0` runtime verification | PASS |
| CN01-02 | Click not through UI onto Ground creates 1 MoveIntent | `GroundClickMovesAtBoundedSpeed` + `MouseInputEventReachesUpdateAndCreatesOneIntent` + user playtest acceptance recorded on 2026-10-25 ("ok rồi") | PASS |
| CN01-03 | Click onto UI does NOT create movement | `UiPanelConsumesClick` (`EventSystem` + `GraphicRaycaster`) | PASS |
| CN01-04 | Missing ground collider safely rejects movement | `MissingGroundRejectsInput` + `InvalidInputAndGroundEdgeCannotMovePlayer` | PASS |
| CN01-05 | Player does not penetrate obstacles/terrain | `WallStopsPlayerWithoutPenetration` + standalone traversal | PASS |
| CN01-06 | Camera zoom/rotate still raycasts ground accurately | `CameraOrbitAndZoomKeepGroundRaycastValid` (3 yaw/pitch/distance combinations) | PASS |

## 4. Documented Limitations & Handoff to CN-02

- User acceptance of the playable CN-01 build was confirmed on 2026-10-25 ("ok rồi"); per user instruction, no repeat user confirmation is requested and no synthetic physical mouse timestamp is fabricated.
- Straight-line `CharacterController` motion stops cleanly at obstacles; NavMesh pathfinding around obstacles, skeletal animation clips, and mobile hardware touch testing remain outside CN-01 scope.
- **Decision:** Gate **CN-01 is officially marked PASS**. Opening **CN-02** (Editable `BaLangHuyen.unity` scene, `World` loader/validator, 5-point ground probes, camera bounds, offline movement integration, and `Builds/CN02-Windows`).
