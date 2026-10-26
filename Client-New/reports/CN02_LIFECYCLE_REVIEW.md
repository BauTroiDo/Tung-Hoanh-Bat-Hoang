# CN02 Lifecycle / Scene Ownership Review and Fixes

**Baseline:** 609f98f (Antigravity handed off, worktree clean). **Branch:** cg/cn02-lifecycle-fixes.
**Date:** 2026-10-25, Asia/Bangkok (UTC+07). **Owner:** Codex/ChatGPT.
**Result:** Review defects reproduced and fixed; 18 PlayMode / 14 EditMode passing. Overall CN02 still below PASS because real calibration remains unverified.

## Root causes and changes

1. A generation mismatch inside the pending LoadSceneAsync loop used yield break before the cleanup branch. Cancelled additive loads could finish in Unity after the routine had exited, with no cleanup/callback. The routine now stays scheduled until Unity finishes, unloads cancelled content, clears pending ownership and invokes cancellation callback once. Caller must keep the routine scheduled and use CancelCurrentLoad, rather than stopping the coroutine externally.
2. CancelCurrentLoad made IsLoading=false immediately, which let a replacement start while the original operation was still pending. A pending-operation guard now retains exclusive admission until cleanup completes; a later load is accepted after that point. UnloadCurrentMapRoutine waits for pending cleanup while the load coroutine continues running.
3. Loading the same map twice and cancelling the duplicate could select the original scene by name and unload it. This was reproduced with admitted=True and originalLoaded=False. Already-loaded targets are now rejected before starting a Unity operation, preserving the original scene and invoking the rejection callback once. Explicitly unload before reloading the same target.
4. Ground probes and spawn used global Physics.Raycast by layer. Another additive scene could supply missing ground or hide real target ground. Sampling now requires the target Scene and queries only active, enabled, non-trigger colliders on its Ground layer, selecting the nearest valid upward surface. Terrain/scene geometry was not changed.

Changed runtime API: WorldSceneValidator.TrySampleGroundHeight(Scene scene, Vector3 horizontalPosition, int groundLayer, out Vector3 groundPoint, out RaycastHit hitInfo). All existing callers were updated. This is an internal offline scene-validation API, not a network protocol change.

## Test-first evidence

- Four initial regressions failed, Unity exit 2: pending cancellation cleanup, replacement admission during cancellation, foreign Ground supplying missing target ground/spawn, foreign foreground hiding target ground.
- After the first fix all four passed.
- Additional duplicate-target regression failed (exit 2), confirming cancellation unloaded the original map. After the duplicate admission guard it passed.
- Full current PlayMode suite: **18/18 PASS**, zero failed/skipped, process exit 0 (13 baseline + 5 regressions).
- Full current EditMode suite: **14/14 PASS**, zero failed/skipped, process exit 0.
- The installed Unity NUnit does not expose Assert.Multiple; the regression assertion was adjusted to a compatible combined assertion before recording its real behavioral red result.

Results and red artifacts are in reports/evidence/cn02-fixes/. Existing tests/evidence were preserved.

## Build and real runtime

New build: Builds/CN02-Review-Windows/JXClientNew.exe. Existing CN00/CN01/CN02 build folders preserved.

Actual build: installed Unity 6000.6.7f1 with `-batchmode -nographics -quit -projectPath <Client-New>/UnityProject -buildWindows64Player <Client-New>/Builds/CN02-Review-Windows/JXClientNew.exe -logFile <Client-New>/.tmp/cn02-fixes-build.log`. Process exit 0; log records `Build Finished, Result: Success.`

Actual visible player run: `-cn02SmokeTest -cn02ScreenshotDir <Client-New>/.tmp/cn02-fixes-runtime -screen-width 1280 -screen-height 720 -screen-fullscreen 0 -logFile <Client-New>/.tmp/cn02-fixes-runtime.log`. Process exit 0; scene OfflineReady=True, all five own-scene probes hit, avatar walked from (0,0.04,-3) to approximately (3.92,0.11,-1.07), target distance 0.103. Four actual PNGs were written; east view opened and visually inspected. This was an automated offline smoke, not a new human-click or GS-authority claim.

Built World DLL SHA256: 170DB8E61F8DD9B7B7BA15FA0090C357C59A9E7010E6B4B3C5F71288E5A321BF. Additional artifact/source hashes are in verification.json. Runtime still has the previously documented URP stripped post-processing warnings; no new network behavior is introduced.

Cancellation regressions establish the supported additive workflow. LoadSceneMode.Single cannot restore a scene that Unity already replaced; transactional Single-mode rollback is not delivered or claimed. External StopCoroutine/disposal before completion is also not a substitute for the cooperative cancellation API. Ground collider enumeration is intended for readiness/probe checks, not a new per-frame large-world acceleration structure.

## Anchor investigation / corrected inventory

Read-only file investigation found both original-named map bundles under Client-3D-2/LAN-LOCAL/Map53Official4016. Earlier checkpoint's statement that these files were absent was incorrect. They have not been extracted, imported, executed or copied into Assets; rights remain UNKNOWN, not established merely by possession.

| File | Bytes | SHA256 |
|---|---:|---|
| pc_balanghuyen_assets_all_53fcdd524b8c67eafad5c013f036ed4b.bundle | 31592781 | 73283BA18C144FB37286CAAD5F6BF4DB4F17C25BD94A3B470FA3E5D07A5DF4D1 |
| pc_balanghuyen_scenes_all_4e0a8e141624756b20ca4ccfa32087fb.bundle | 9738990 | EEB567D492CE740AAD8A796F531D993917F511FDA60ECF4DDE25DA6C2B4DEC4A |

Existing M1-map53-region.json contains legacy-only NPC positions (51889,102090) and (51889,102105). They share X and lack paired Unity scene points. The old single historical anchor remains insufficient. No real 3+1 calibration or original-geometry restoration has been established. Inventory/search was bounded to existing local documentation/reference folders; no claim of an exhaustive machine-wide search.

## Gate / safety / handoff

Both technical review items are fixed and regression-tested, including the newly reproduced duplicate-map ownership bug. CN02-04 remains BLOCKED/NOT VERIFIED; do not open CN03. Any further source review should use the new fix commit, rather than the old 13-test/old build result.

Server, database, characters and reference clients were neither launched nor modified. Reference Game.exe and GameAssembly.dll SHA256 values still match baseline. Only read-only inventory was performed outside Client-New. Git history and Antigravity geometry/source were preserved.

Rollback: revert the fix milestone on its branch after checking consumers of the scene-scoped sampling signature; do not hard-reset/clean shared work. The previous Builds/CN02-Windows output remains available for comparison. Independent review of the fix commit may follow without redoing accepted user confirmation.
