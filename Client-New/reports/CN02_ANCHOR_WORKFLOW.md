# CN02 Anchor Evidence Workflow — Implementation and Source Audit

**Branch:** cg/cn02-anchor-evidence. **Baseline:** 8bfe8bc. **Date:** 2026-10-25, Asia/Bangkok (UTC+07).
**Result:** Functional Editor evidence workflow delivered; real Map53 calibration remains UNVERIFIED. CN03 stays closed.

## Read-only historical source finding

Found the old converter at Clent-3D/LAN/world_conversion.py, not in Client-New. Its map_53_transform uses the sole BeginPoint01 in reference/world_baling.markers.json, paired with hardcoded legacy (52032,101696). This source belongs to the Unity2022 world_baling reference. It explicitly labels alignment spawn_anchor and movement_calibrated=False; default scale 1/32 is not a surveyed terrain transform. The corresponding position is (209.8,5.904738,103.71). Do not transfer that single spawn alignment to 4016 BLH_Bake 3 or the synthetic BaLangHuyen scene, or generate extra calculated points and call them independent anchors.

Only source/JSON reads and SHA256 checks were performed; source was neither imported nor executed. Fingerprints/context are in reports/evidence/anchor-workflow/legacy-source-audit.json. No raw legacy source or proprietary geometry was copied into Assets.

## Delivered code

- Domain/Coordinates/CalibrationEvidence.cs: exactly three fit observations and one distinct holdout, explicit positive tolerance, named/provenance-tagged observations, SHA256 metadata checks, affine fitting and holdout residual. Statuses: InvalidInput, NeedsEvidence, OutOfTolerance, ReadyForReview. No Verified status or map-catalog mutation exists.
- Editor/CalibrationTools/AnchorCalibrationTools.cs: saved scene marker capture, unique draft JSON export/import, schema/map/provenance/scene hash validation, marker identity/coordinate comparison to actual scene, evidence-file SHA256 recheck, numeric/size limits and derived status recalculation. `independentReviewRequired` is forced true.
- Editor/CalibrationTools/AnchorCalibrationWindow.cs: UI Toolkit window, capture/import/assess/export controls, legacy bounds and tolerance entry, four fit/holdout groups, local evidence picker. It opens BaLangHuyen and preloads its markers; unsaved scene edits use Unity's save-choice dialog instead of silent discard. It refuses scene switching while Play Mode is active.
- New Domain and Editor tests. Editor test assembly is Editor-only and not shipped in the game.

The tool validates file integrity and candidate geometry, not truth of an operator's measurement claim. Referenced evidence must still be reviewed for timestamps, matching builds/map/landmarks and valid error tolerance. `createdUtc` is draft creation time, not an invented measurement time.

## How to use

1. Open menu **JXClient > CN-02 > Anchor Evidence (3+1)**. BaLangHuyen opens, four markers prefill and the Scene View frames the map. This corrects the blank SampleScene context seen in the user's screenshot.
2. Declare reference build identity, valid legacy bounds and justified holdout tolerance. Defaults intentionally do not invent these values.
3. Enter the measured legacy X/Y for each scene X/Z marker, including the independent fourth point. Attach matching evidence files and hashes, classify provenance honestly.
4. Assess candidate, then export a versioned draft. Highest outcome is ReadyForReview; this never updates runtime calibration to Verified.
5. Import rejects stale scenes, changed marker coordinates, wrong shape/schema/map and recalculates status. Missing/changed proof files cannot remain ready.

A real unpaired draft is already in reports/evidence/anchors/drafts/: 4 actual scene marker rows, `hasLegacy=false`, status DRAFT_UNPAIRED, current scene SHA256. Zero legacy placeholders are explicitly unpaired, not fake measured anchors.

## Verification and failure history

- Initial Domain API shell: 8 tests, 5 failed and 3 passed (exit 2). Implemented assessment then passed all 8.
- Initial Editor API shell: 3 missing capture/import/UI behaviors failed (exit 2), then passed after implementation.
- Two additional integrity tests reproduced acceptance of modified marker coordinates and malformed draft shape (exit 2); current importer rejects both. Evidence-file mutation test confirms readiness is downgraded.
- The user screenshot showed SampleScene with the tool open but empty. A new regression reproduced that Open left Bootstrap active instead of the target scene (exit 2); Open now loads BaLangHuyen and preloads its marker form.
- The full window-opening regression initially failed with `No graphic device is available` when run with -nographics. The final complete EditMode command omits that flag so it can instantiate a real Editor window; no errors were suppressed or tests skipped.
- **Final EditMode: 29/29 PASS**, zero failed/skipped, process exit 0 (14 baseline + 8 assessment + 7 Editor tests).
- **Final PlayMode: 18/18 PASS**, zero failed/skipped, process exit 0.
- **Actual draft export: exit 0**, current scene fingerprint/markers captured through Unity APIs.
- **Separate Win64 build: exit 0, Build Finished, Result: Success.** Output Builds/CN02-AnchorWorkflow-Windows. Neither JXClient.Editor.dll nor the calibration Editor test assembly is in the player.

Commands (actual project path E:/Games/VLTK3D/SwordOnline/Client-New/UnityProject, installed Unity 6000.6.7f1):

```text
-batchmode -projectPath <project> -runTests -testPlatform EditMode -testResults <root>/.tmp/anchor-workflow-editmode.xml -logFile <root>/.tmp/anchor-workflow-editmode.log
-batchmode -nographics -projectPath <project> -runTests -testPlatform PlayMode -testResults <root>/.tmp/anchor-workflow-playmode.xml -logFile <root>/.tmp/anchor-workflow-playmode.log
-batchmode -nographics -quit -projectPath <project> -executeMethod JXClient.Editor.Calibration.AnchorCalibrationTools.ExportCurrentSceneDraft -logFile <root>/.tmp/anchor-workflow-export.log
-batchmode -nographics -quit -projectPath <project> -buildWindows64Player <root>/Builds/CN02-AnchorWorkflow-Windows/JXClientNew.exe -logFile <root>/.tmp/anchor-workflow-build.log
```

Final Open/menu refinement is Editor-only and tested after the player build; runtime assemblies were not modified by that refinement. A new player runtime smoke was not needed or claimed for this Editor feature; previous lifecycle-fixed runtime proof remains in reports/evidence/cn02-fixes/.

Native UI automation confirmed the actual AnchorCalibrationWindow pane. Window capture timed out, then file-dialog input could not obtain geometry/focus. No successful OS click test or post-fix screenshot is claimed. The fresh owned Editor process was restarted through the Unity Open command; native window title changed to BaLangHuyen. Functional Open/capture/import/export/UI tree assertions passed in the actual graphical Unity Test Runner. The user's earlier screenshot is evidence of the pre-fix SampleScene context only.

## Remaining gate / rollback

No real 3+1 paired anchors were discovered. The old source proves only one Unity2022 spawn alignment; CN02-04 and overall CN02 remain below PASS. No server, database, role, reference execution or reference asset import. No new Unity packages, public upload or push.

Rollback: revert this workflow commit after checking consumers; runtime map geometry and previous builds were preserved. Draft files are standalone evidence artifacts, not runtime settings, and may be retained for review. Save/export entered draft values before closing the Editor or recapturing markers.
