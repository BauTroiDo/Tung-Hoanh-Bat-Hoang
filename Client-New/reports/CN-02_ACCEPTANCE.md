# CN-02 ACCEPTANCE REPORT — Editable Ba Lang Huyen Scene (Synthetic Fallback), World Loader & Offline Integration

**Gate/task:** CN-02 / `CN02-01..CN02-06`
**Owner:** Antigravity (World/Environment/Scene/Build) & Codex/ChatGPT (Coordinate Math Contract & Module)
**Reviewer:** Codex / ChatGPT
**Environment/timezone/timestamp:** Windows 10/11 x64 (`DESKTOP-TPG2RNF`, GPU NVIDIA GeForce GTX 1080), `Unity 6000.6.7f1 (b22767f04f04)`, Asia/Ho_Chi_Minh, 2026-10-25T16:12:00+07:00
**Branch/commit:** `ag/cn01-review-cn02-world` (from `d08789a` / `755747f`, incorporating coordinate math `417ca6a` / `0888633`)
**Repo clean?** yes (after committing CN-02 milestone)
**Status:** **REVIEW / PARTIAL (`CN02-01, 02, 03, 05, 06 = PASS`; `CN02-04 = BLOCKED / NOT VERIFIED` pending real 3+1 Map53 anchors)** — Overall CN-02 remains below full PASS per `tasks/ANTIGRAVITY_CN02_HANDOFF.md`; **do NOT open CN-03**.

## Scope and changes

- **Goal and exact scope:**
  1. Complete independent review of CN-01 (`reports/CN-01_REVIEW.md`) and mark **CN-01 PASS**.
  2. Implement `JXClient.World` (`MapSceneCatalog`, `MapSceneLoader`, `WorldSceneValidator`, `BaLangHuyenSceneController`) for `MapId=53` with async load/unload, cancellation handling, required 7-root hierarchy validation (`Ground`, `Environment`, `StaticProps`, `SpawnPoints`, `Colliders`, `Lighting`, `Nav`), 5 named Layer-8 ground raycast probes, safe spawn validation, camera bounds enforcement, and strict separation between `IsOfflineSceneReady` (`GameSessionState.OfflineReady`) and network `WorldReady`.
  3. Generate editable synthetic `Assets/_JXClient/Scenes/BaLangHuyen.unity` (`cn02-synthetic-v1`, explicitly labeled as synthetic fallback, NOT original Ba Lang Huyen restoration) with URP materials (`BLH_GrassTerrain`, `BLH_StoneRoad`, `BLH_PlazaTile`, `BLH_HouseWall`, `BLH_TileRoof`, `BLH_WoodTimber`, `BLH_BoundaryRock`), village gate/house prefabs (`BLH_SyntheticGateArchway`, `BLH_SyntheticVillageHouse`), obstacle/perimeter colliders, spawn & anchor candidate markers, and offline movement integration (`OfflinePlayer` + `OrbitFollowCamera` + UI click shield).
  4. Preserve `Bootstrap.unity`, `MovementTest.unity`, `Builds/Windows`, and `Builds/CN01-Windows`; produce a separate standalone build at `Builds/CN02-Windows/JXClientNew.exe`.
  5. Capture 4 real rendered camera views, runtime logs, performance baseline, and provenance manifest; coordinate with Codex via `docs/CN02_COORDINATE_EVIDENCE.md` without creating a competing coordinate converter.
- **Files added/modified (paths):**
  - `reports/CN-01_REVIEW.md` & `reports/evidence/cn01/review-*`
  - `UnityProject/Assets/_JXClient/World/JXClient.World.asmdef` (+ `.meta`)
  - `UnityProject/Assets/_JXClient/World/MapSceneCatalog.cs` (+ `.meta`)
  - `UnityProject/Assets/_JXClient/World/MapSceneLoader.cs` (+ `.meta`)
  - `UnityProject/Assets/_JXClient/World/BaLangHuyenSceneController.cs` (+ `.meta`)
  - `UnityProject/Assets/_JXClient/Environment/Materials/BLH_*.mat` (7 materials + `.meta`)
  - `UnityProject/Assets/_JXClient/Environment/Prefabs/{BLH_SyntheticGateArchway,BLH_SyntheticVillageHouse}.prefab` (+ `.meta`)
  - `UnityProject/Assets/_JXClient/Scenes/BaLangHuyen.unity` (+ `.meta`)
  - `UnityProject/Assets/_JXClient/Editor/Cn02ProjectSetup.cs` (+ `.meta`)
  - `UnityProject/Assets/_JXClient/Editor/JXClient.Editor.asmdef`
  - `UnityProject/Assets/_JXClient/Tests/PlayMode/Cn02WorldSceneTests.cs` (+ `.meta`)
  - `UnityProject/Assets/_JXClient/Tests/PlayMode/JXClient.Tests.PlayMode.asmdef`
  - `UnityProject/ProjectSettings/EditorBuildSettings.asset`
  - `docs/ASSET_PROVENANCE_MANIFEST.md`
  - `docs/CN02_COORDINATE_EVIDENCE.md`
  - `reports/evidence/cn02/{editmode.xml,playmode.xml,runtime-visible.log,view1_spawn_south.png,view2_east_walked.png,view3_north_elevated.png,view4_west_close.png}`
  - `reports/CN-02_ACCEPTANCE.md`
  - `TASK_BOARD.md` & `docs/CHECKPOINT.md`
- **Explicitly untouched systems:**
  - Server `192.168.1.69` (`Bishop` 56722, `gs001.exe` 51189, `Gateway` 14180) and existing 3 characters.
  - Reference client `Client-3D-2/update` (`Game.exe` SHA256 `1CAE3982C04BE9D6F8F7EA8CE272600400655D40FBF0671818EFF1D7A79750EE`, `GameAssembly.dll` SHA256 `011357720B210AA1F392A8B62FF8DE090D3A895230A6E0135BCE707196871069` verified unchanged).
  - Prior builds `Builds/Windows` (CN-00) and `Builds/CN01-Windows` (CN-01).

## Evidence table (`CN02-01..CN02-06`)

| Test ID | Expected | Actual | Command/Artifact | Exit code | Status |
|---|---|---|---|---|---|
| CN02-01 | Scene `BaLangHuyen.unity` opens in Editor with required 7 roots (`Ground`, `Environment`, `StaticProps`, `SpawnPoints`, `Colliders`, `Lighting`, `Nav`) | Generated and opened in Editor/PlayMode; all 7 required roots verified present; async load/unload & cancellation verified | `Cn02ProjectSetup.Generate`; `Cn02WorldSceneTests.BaLangHuyenScene_LoadsValidHierarchyFiveGroundProbesAndUnloadsCleanly` | 0 | **PASS** (Synthetic scene) |
| CN02-02 | Visible ground mesh/terrain + URP materials | `BLH_MainMeadowGround` (60x60m), `BLH_NorthSouthRoad`, `BLH_EastWestRoad`, `BLH_CenterPlazaGround` with `Universal Render Pipeline/Lit` materials, plus 2 gates, 4 houses, and 6 obstacle/perimeter colliders | `UnityProject/Assets/_JXClient/Environment/**`; `reports/evidence/cn02/view1..4*.png` | 0 | **PASS** (Synthetic scene) |
| CN02-03 | Collider and raycast hits ground at 5 named sample points | All 5 probes hit Layer 8 `Ground` with `normal.y = 1.00`: `Probe_CenterPlaza` `(0, 0.04, 0)`, `Probe_NorthGatePath` `(0, 0.02, 18)`, `Probe_SouthGatePath` `(0, 0.02, -18)`, `Probe_EastBridgeApproach` `(18, 0.03, 0)`, `Probe_WestMarketApproach` `(-18, 0.03, 0)` | `reports/evidence/cn02/runtime-visible.log`; `reports/evidence/cn02/playmode.xml` | 0 | **PASS** |
| CN02-04 | Legacy ↔ Unity coordinate calibration matches 3 non-collinear anchors + 4th holdout point | Pure math module `JXClient.Domain.Coordinates.AffineMapTransform` + 11 synthetic EditMode tests PASS (`14/14` total EditMode), and scene candidate markers exist under `SpawnPoints`, **but real verified 3+1 legacy/Unity Map53 anchor pairs are not yet available** (`MapCalibrationStatus.Unverified`) | `docs/CN02_COORDINATE_EVIDENCE.md`; `reports/CN02_COORDINATE_IMPLEMENTATION.md`; `reports/evidence/cn02/editmode.xml` | 0 (math) | **BLOCKED / NOT VERIFIED** (awaiting real 3+1 anchors) |
| CN02-05 | No empty blue background; spawn, camera bounds, lighting & offline movement work across 4 real camera angles | 4 real 1280x720 PNGs captured from `Builds/CN02-Windows/JXClientNew.exe`; player spawns at `(0, 0.04, -3)`, walks offline to `(3.92, 0.11, -1.07)` (`distance=0.104`), stops at `Obstacle_WestWall` in PlayMode test, camera clamped to `Bounds((0,4,0), (56,20,56))` | `reports/evidence/cn02/{view1_spawn_south.png,view2_east_walked.png,view3_north_elevated.png,view4_west_close.png}` | 0 | **PASS** (Synthetic scene) |
| CN02-06 | Asset provenance/license manifest valid; 0 missing `.meta` | Read-only 4016 inventory recorded (`catalog.bin`, `catalog.hash`, `balanghuyen.mpk`); no unlicensed bundles extracted/imported; all committed assets are first-party synthetic with 0 missing `.meta` | `docs/ASSET_PROVENANCE_MANIFEST.md` | 0 | **PASS** |

## Build, Tests & Runtime Performance Baseline

- **Unity Editor version and install path:** `6000.6.7f1 (b22767f04f04)` at `C:\Program Files\Unity\Hub\Editor\6000.6.7f1\Editor\Unity.exe`
- **Exact commands executed:**
  1. **Generate Scene & Environment:**
     ```powershell
     & "C:\Program Files\Unity\Hub\Editor\6000.6.7f1\Editor\Unity.exe" -batchmode -nographics -quit -projectPath "E:\Games\VLTK3D\SwordOnline\Client-New\UnityProject" -executeMethod JXClient.Editor.Cn02ProjectSetup.Generate -logFile "E:\Games\VLTK3D\SwordOnline\Client-New\.tmp\cn02_generate.log"
     ```
     Exit code: `0`
  2. **Combined EditMode Suite (`14/14 PASS`, exit code `0`):**
     ```powershell
     & "C:\Program Files\Unity\Hub\Editor\6000.6.7f1\Editor\Unity.exe" -batchmode -nographics -projectPath "E:\Games\VLTK3D\SwordOnline\Client-New\UnityProject" -runTests -testPlatform EditMode -testResults "E:\Games\VLTK3D\SwordOnline\Client-New\reports\evidence\cn02\editmode.xml" -logFile "E:\Games\VLTK3D\SwordOnline\Client-New\.tmp\cn02_editmode_combined.log"
     ```
     Result: `total=14, passed=14, failed=0, skipped=0` (3 CN-00 bootstrap/domain tests + 11 `AffineMapTransformTests`).
  3. **Combined PlayMode Suite (`13/13 PASS`, exit code `0`):**
     ```powershell
     & "C:\Program Files\Unity\Hub\Editor\6000.6.7f1\Editor\Unity.exe" -batchmode -nographics -projectPath "E:\Games\VLTK3D\SwordOnline\Client-New\UnityProject" -runTests -testPlatform PlayMode -testResults "E:\Games\VLTK3D\SwordOnline\Client-New\.tmp\cn02_playmode.xml" -logFile "E:\Games\VLTK3D\SwordOnline\Client-New\.tmp\cn02_playmode.log"
     ```
     Result: `total=13, passed=13, failed=0, skipped=0` (9 CN-01 movement/scene tests + 4 `Cn02WorldSceneTests`).
  4. **Standalone Win64 Build (`Builds/CN02-Windows`, exit code `0`):**
     ```powershell
     & "C:\Program Files\Unity\Hub\Editor\6000.6.7f1\Editor\Unity.exe" -batchmode -nographics -quit -projectPath "E:\Games\VLTK3D\SwordOnline\Client-New\UnityProject" -executeMethod JXClient.Editor.Cn02ProjectSetup.Build -logFile "E:\Games\VLTK3D\SwordOnline\Client-New\.tmp\cn02_build_final.log"
     ```
     Result: `[CN-02] BUILD result=Succeeded errors=0 warnings=0 bytes=104266682 path=E:\Games\VLTK3D\SwordOnline\Client-New\Builds\CN02-Windows\JXClientNew.exe`
  5. **Visible Multi-Angle Runtime Smoke (`exit code 0`):**
     ```powershell
     & "E:\Games\VLTK3D\SwordOnline\Client-New\Builds\CN02-Windows\JXClientNew.exe" -cn02SmokeTest -cn02ScreenshotDir "E:\Games\VLTK3D\SwordOnline\Client-New\reports\evidence\cn02" -screen-width 1280 -screen-height 720 -screen-fullscreen 0 -logFile "E:\Games\VLTK3D\SwordOnline\Client-New\.tmp\cn02_runtime_visible_final.log"
     ```
     Exit code: `0`
- **Artifact sizes & SHA256 hashes:**
  - `Builds/CN02-Windows/JXClientNew.exe` (`667,136` bytes): `9806ABDE4A406ED68647E9505852C49F6AA72A372F4F084E2AA2821AFFEA57A2`
  - `Builds/CN02-Windows/JXClientNew_Data/Managed/JXClient.World.dll` (`26,112` bytes): `8C0CFCD15691A925BC07D8848BEE89CC85C7B2295C4787BD7CFCBDA14266B5B1`
  - `Builds/CN02-Windows/JXClientNew_Data/Managed/JXClient.Movement.dll` (`12,288` bytes): `97777C814FFFDEA3E867E372AD79A41BE3C4A8A66396124A19C72BB66EEDF06C`
  - `Builds/CN02-Windows/JXClientNew_Data/Managed/JXClient.Domain.dll` (`11,776` bytes): `D172787EAF4B12658456673142F4F48872606F7F063E8A131CA019C86EB8E8D8`
  - `Builds/CN02-Windows/JXClientNew_Data/level0` (`BaLangHuyen`, `33,284` bytes): `209F368FF034E7E4152396FD702590D1E6F312FBD8C2D2DDADE54D4486CBC053`
  - `Builds/CN02-Windows/JXClientNew_Data/level1` (`MovementTest`, `16,100` bytes): `284D78B3D3650592AA319DF2CEA9A8D4D0AD12611DA60895E5F5B570F7AA3C2D`
  - `Builds/CN02-Windows/JXClientNew_Data/level2` (`Bootstrap`, `3,480` bytes): `46CDBE56946C208FB1AB540D0C4AAE88C6499B39BA23391932B24E4A0FF2C680`
  - `UnityProject/Assets/_JXClient/Scenes/BaLangHuyen.unity` (`106,053` bytes): `1DF531129F8C502B54093DB5AD6C9939A97C187F22ACF4D939DDE34A04FE5A7A`
- **Measured Runtime Performance Baseline (1280x720, Direct3D 12 on NVIDIA GeForce GTX 1080):**
  - Unload/scene startup time: `0.78 ms`
  - Traversal simulation: `1,541` frames over `1.09 s` -> average **`1,415.1 FPS`** (instantaneous HUD views measured `1,432.4 FPS` to `1,654.9 FPS` uncapped).
  - Managed GC memory (`GC.GetTotalMemory`): **`2.59 MB`**.
- **Four Real Runtime Camera Screenshots (`reports/evidence/cn02/`):**
  1. `view1_spawn_south.png` (`118,805` bytes): Angle 1 South Overview (`Yaw=0, Pitch=45, Dist=18`) at initial spawn `(0.00, 0.04, -3.00)`.
  2. `view2_east_walked.png` (`133,670` bytes): Angle 2 East View Post-Walk (`Yaw=90, Pitch=40, Dist=16`) after walking to `(3.92, 0.11, -1.07)`.
  3. `view3_north_elevated.png` (`121,690` bytes): Angle 3 North Elevated (`Yaw=180, Pitch=60, Dist=22`).
  4. `view4_west_close.png` (`133,560` bytes): Angle 4 West Diagonal Close (`Yaw=240, Pitch=35, Dist=12`).

## Safety, Asset Compliance & Rollback

- **Security check / credentials redacted:** Zero server endpoints, passwords, or tickets in code, assets, or logs.
- **Asset source/license/provenance checked:** `docs/ASSET_PROVENANCE_MANIFEST.md` updated with read-only 4016 inventory and synthetic fallback records. No third-party or legacy-client assets were imported.
- **Original Bishop/GS/client reference unchanged:** Verified SHA256 of `Client-3D-2/update/Game.exe` and `GameAssembly.dll` match baseline 100%.
- **Rollback steps:**
  - To revert CN-02 world/scene additions while keeping CN-01 PASS and coordinate math: `git revert <cn02-world-commit>` and remove `Builds/CN02-Windows`.

## Findings and next gate

- **Issues found & resolved during implementation:**
  1. When `MapSceneLoader.CancelCurrentLoad()` cancelled an in-progress additive `SceneManager.LoadSceneAsync`, the underlying Unity async operation still completed a frame later; `Cn02WorldSceneTests.TearDown` now waits for and unloads any cancelled additive scene so subsequent `OfflineMovementTests` have a clean physics world (`13/13` PlayMode PASS).
  2. In `BaLangHuyen`, the initial smoke walk target projected onto the top-left UI `InputShield`; adjusted target to `(4, 0, -1)` on the east road (`screen=(764.2, 386.8)`), confirming both UI shielding and road walking.
- **Blockers/risks (`CN02-04`):**
  - Real `3+1` non-collinear legacy/Unity Map53 anchor pairs and licensed original Ba Lang Huyen meshes remain unavailable. Per `tasks/ANTIGRAVITY_CN02_HANDOFF.md` and `docs/CN02_COORDINATE_EVIDENCE.md`, `BaLangHuyen.unity` is explicitly labeled `SYNTHETIC FALLBACK — NOT ORIGINAL BLH RESTORATION` and `CN02-04` is kept `BLOCKED / NOT VERIFIED`.
- **Recommended next exact action:**
  - Codex/ChatGPT reviews `reports/CN-01_REVIEW.md` and `reports/CN-02_ACCEPTANCE.md`.
  - Supply 3 verified non-collinear legacy↔scene Map53 anchor pairs + 1 independent holdout point (and/or licensed Map53 geometry if original layout is required) to calibrate `AffineMapTransform` and complete `CN02-04` before opening `CN-03`.
- **Reviewer decision:** PENDING REVIEW (`CN-03` remains CLOSED).
