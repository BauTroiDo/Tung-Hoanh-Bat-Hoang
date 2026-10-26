# CN02 Integration Notes — Codex coordinate component

**Updated:** 2026-10-25 16:05 UTC+07.

Coordinate source/tests are now integrated into the root branch as commit `417ca6a` (cherry-pick of `cg/coordinate-math` / `d5d28ca`). Integration ran while no Unity Editor process was active and root index was empty. Hash comparison of all previously pending root files showed **0 changed files**; Antigravity World/scene changes were preserved.

Component verification on isolated baseline: **14/14 EditMode, 9/9 PlayMode**, Unity exit 0. Main API/evidence: reports/CN02_COORDINATE_IMPLEMENTATION.md and reports/evidence/coordinates/. World already references Domain, so no new asmdef reference is needed. Numerical module is ready; real Map53 3+1 calibration remains UNVERIFIED.

Root combined suite must be rerun after integration and after scene-test fixes; isolated results are not the combined root result. In the earlier root artifact reports/evidence/cn02/playmode.xml (run 09:02:28Z, before coordinate integration), observed failures were:

- Cn02WorldSceneTests.BaLangHuyenScene_OfflinePlayerWalksRoadStopsAtObstacleAndRespectsCameraBounds: clicked destination fell under UI shield. Current test source already changes camera angle; verify with fresh run.
- OfflineMovementTests.InvalidInputAndGroundEdgeCannotMovePlayer: x became 0.399979 after fixture ground removed.
- OfflineMovementTests.MissingGroundRejectsInput: unexpected accepted click after fixture ground disabled.

The latter two warrant scene/physics isolation investigation: another loaded Ground collider can satisfy the raycast despite the fixture's collider being removed. Confirm loaded scenes and ownership/unload behavior before altering production movement or weakening negative assertions. Antigravity owns these scene/test files; Codex has not overwritten them.

Please keep original negative regression intent, record every actual failure/fix, and rerun the full root suites. Expected minimum EditMode count is now 14 (11 new coordinate tests plus 3 baseline), not the earlier 3. Do not mark global CN02 PASS while failures or real-anchor gaps remain.

## Latest integration update

- Combined root EditMode rerun after integration: **14/14 PASS**, zero failed/skipped, process exit 0. Preserved reports/evidence/coordinates/root-editmode.xml.
- Refreshed Antigravity root PlayMode artifact at 09:06:20Z: **13/13 PASS**. This supersedes the earlier three failures above; retain their history rather than treating the earlier red artifact as the current state.
- Root CN02 Win64 rebuild completed exit 0, BuildSummary Succeeded, errors=0, warnings=0. Actual built JXClient.Domain.dll contains JXClient.Domain.Coordinates.AffineMapTransform (reflection inspection); module is included in the playable build.
- Opened actual view2_east_walked.png: visible synthetic village/terrain/roads/buildings, avatar and explicit UNVERIFIED calibration label. This is a new synthetic replacement, not original BLH restoration.

## Code-review follow-ups for map owner (not claimed as reproduced runtime failures)

1. MapSceneLoader.cs cancellation: the generation-mismatch yield break inside the pending `while (!op.isDone)` exits before the later completed-operation unload branch. A cancellation that is observed while loading is genuinely pending can abandon cleanup/callback. Add a delayed-cancellation regression (not only cancellation when the operation already finished), then ensure operation completion/unload is owned and callback delivered exactly once.
2. WorldSceneValidator ground sampling currently uses global Physics.Raycast and layer only; it does not verify that a hit belongs to the scene being validated. Another additively loaded Ground can satisfy probes/spawn. Add a negative scene-ownership regression and scene-scoped hit filtering before relying on this readiness check for networking.

These follow-ups stay with Antigravity's claimed loader/scene test scope. Codex has not overwritten those files. Overall CN02 remains below PASS while real anchors are missing; current numerical component/tests are verified.

Source integrity note: raw file hashes from the isolated LF checkout differ after Git materializes CRLF in the Windows root checkout. Comparing normalized source text gives 0 drift, and git diff d5d28ca HEAD for the coordinate source/tests is empty. This is line-ending normalization, not a source change; use Git blob identity or normalized content when comparing the two checkouts.
