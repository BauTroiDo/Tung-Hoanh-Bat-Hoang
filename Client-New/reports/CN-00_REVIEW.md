# CN-00 Independent Review / Review checkpoint

Reviewer: Codex (ChatGPT). Date: 2026-10-25, Asia/Bangkok (UTC+07).
Scope: read-only review of Antigravity bootstrap; isolated test-evidence adapter.
Decision: PASS for CN-00 after source commit 55b54b8 and handoff 7d419db. CN-01 READY; no gameplay/network acceptance.

## Ownership and concurrent work

Antigravity session `12d70f78-67cf-4aab-8199-ecdbbb67a249` is actively implementing CN00-A..D on `ag/cn00-bootstrap`. Its transcript shows project creation at 15:02 and new asmdefs at 15:05. The active claim includes UnityProject/**, acceptance report, TASK_BOARD and docs/CHECKPOINT. Reviewer has not modified those files or the pre-existing untracked acceptance report. This review is a separate checkpoint; the gate owner must incorporate verified facts into the main checkpoint.

A temporary review branch was created before the active claim appeared; checkout was restored to `ag/cn00-bootstrap`, without reset, stash or loss of changes. No remote is configured. No push was attempted.

## Verified facts

- Installed Editor: `C:/Program Files/Unity/Hub/Editor/6000.6.7f1/Editor/Unity.exe`, revision `b22767f04f04`. Hub exists and is running. Previous NOT INSTALLED report is obsolete.
- Unity Personal entitlement was resolved in the actual Editor creation log. Licensing log also contains signature-validation and token-refresh warnings; successful entitlement and process completion do not prove future build licensing will always work.
- Project was created by Antigravity using the official installed `com.unity.template.urp-blank-17.2.6.tgz`, with URP 17.6.0 resolved by Package Manager. `ProjectVersion.txt` records 6000.6.7f1. This is a deliberate version deviation from preferred 6000.3.9f1; do not mix assets/binaries with reference clients.
- `.tmp/unity_create_project.log` ends in `Exiting batchmode successfully now!` and application return code 0. The task-70 output independently records project creation exit 0. No `error CS`, `Scripts have compiler errors` or `Compilation failed` match in that log at review time. This proves initial template creation, not compilation of later JXClient changes.
- Installed playback variations include `win64_player_development_mono` and `win64_player_nondevelopment_mono`; WebGLSupport also exists. Actual PC/Web builds remain unproven; no Android/iOS module verification has been performed.
- Project and JXClient asmdefs are being created but are not yet tracked in Git at this checkpoint.

## Reference integrity

Read-only `Get-FileHash -Algorithm SHA256`, matching baseline:

| File under Client-3D-2/update | SHA256 |
|---|---|
| Game.exe | 1CAE3982C04BE9D6F8F7EA8CE272600400655D40FBF0671818EFF1D7A79750EE |
| GameAssembly.dll | 011357720B210AA1F392A8B62FF8DE090D3A895230A6E0135BCE707196871069 |

Reviewer neither launched reference clients nor connected to or modified the game server. Hash coverage is limited to these two files, not the entire reference installation.

## Reviewer implementation and validation

- Added `reports/verification/Test-EditModeResult.ps1`, a read-only NUnit 3 result validator. Missing/malformed XML, empty suites, skipped/failed cases and contradictory counts cannot be accepted as passing evidence. Exit 0 means structurally valid, non-empty, all-pass XML; exit 2 means unverified. Actual Unity command, exit code, correct project and freshness still require separate review.
- Added `reports/verification/Test-EditModeResult.Tests.ps1`: seven isolated fixture cases. Red: failed because reader did not exist (exit 64). Green: seven cases passed, test command exit 0. These are evidence-reader tests, NOT Unity EditMode tests or CN00-04 acceptance.
- Windows PowerShell 5.1 refused script execution under its existing policy. Tests use installed PowerShell 7 (`pwsh`); no execution policy was changed.
- Added `*.slnx` to `.gitignore` because Unity 6 generated UnityProject.slnx. `git check-ignore` confirms solution, Library and Logs exclusion. Source, Packages, ProjectSettings and .meta remain eligible for version control.
- Hub-bundled Unity CLI is 1.0.0-beta.13. Update check found beta.14; update failed with access denied writing unity.exe.previous in Program Files. No elevated retry, permission change or substitute Editor was used.

Run from repository root with PowerShell 7:

```powershell
& ./reports/verification/Test-EditModeResult.Tests.ps1
& ./reports/verification/Test-EditModeResult.ps1 -ResultPath '<actual Unity NUnit XML path>'
```

## Gate evidence still required

| ID | Current review finding |
|---|---|
| CN00-01 | Editor and entitlement confirmed; version deviation documented |
| CN00-02 | Official URP project creation exit 0; later source/import review pending |
| CN00-03 | PASS for current source: scene-generation compile exit 0; no C# compile error matches |
| CN00-04 | PASS: real EditMode suite 3/3, zero failed/skipped; task-99 exit 0 |
| CN00-05 | PASS: Win64 build exit 0; BuildSummary Succeeded, errors=0, warnings=0 |
| CN00-06 | PASS for Bootstrap: real image inspected and independent batch runtime exit 0 after 30 frames |
| CN00-07 | PASS: source/evidence committed, provenance manifest present, source hashes unchanged; clean handoff confirmed |

Next exact action: let Antigravity finish its claimed Bootstrap/tests/build work; independently inspect actual NUnit XML, build exit/result, executable hash and runtime evidence, then update this review. Keep the current worktree and unfinished changes. Do not start a second Editor on the same project while its import/test/build is running.

Rollback for reviewer-only changes: revert the review commit after confirming no later agent changes depend on it. Do not restore the whole repository or overwrite Antigravity files.

## Verified update — 15:07–15:08 UTC+07

- Scene-generation process completed exit 0; `.tmp/unity_generate_bootstrap.log` confirms sole Bootstrap scene. No C# compilation errors matched. All 27 asset files inspected had companion .meta files.
- Real EditMode XML `.tmp/editmode_test_results.xml` records `2026-10-25 08:07:02Z`, correct `JXClient.Tests.EditMode.dll` under this project's Library/ScriptAssemblies. Three actual test cases passed with zero failures/skips. Antigravity task-99 output records test process exit 0.
- Preserved exact XML in `reports/evidence/CN00-EditMode-20261025-150702.xml`; the evidence reader returned Verified=true, Total=3, Passed=3, Failed=0. Passing cases: Bootstrap scene initializes OfflineReady; domain constraints; movement prohibited before WorldReady.
- Source review: Domain has no engine references, Editor and EditMode assemblies are Editor-only. Runtime Bootstrap code has no transport or server credentials. No networking was exercised.
- Non-blocking review risk to check at runtime: BootstrapRunner requests ScreenCapture.CaptureScreenshot and Application.Quit in the same Update. A request log alone cannot establish that the image was written. Require an actual readable image and runtime log; if absent, gate owner should wait for screenshot completion before exiting.
- Build is currently compiling shaders; no Win64 success claimed at this checkpoint.

## Verified update — 15:09–15:11 UTC+07

Win64 build completed: task-103 exit 0 and actual BuildSummary Succeeded, totalErrors=0, totalWarnings=0, totalSize=104153315 bytes. EXE is `Builds/Windows/JXClientNew.exe`, 667136 bytes, SHA256 `9806ABDE4A406ED68647E9505852C49F6AA72A372F4F084E2AA2821AFFEA57A2`. The EXE hash alone identifies the launcher; it is not a hash of all player data.

Opened and visually inspected Antigravity's real 1280x720 PNG `reports/evidence/cn00/cn00_bootstrap_smoke.png`: rendered plane and cube, Bootstrap scene, OfflineReady, GroundCollider Ready, MoveAllowed=False. Corresponding runtime log shows actual 30-frame advancement and shutdown. This is only an offline Bootstrap scene, not Map53/gameplay or network proof. Runtime log has warnings for stripped/unsupported DOF and Panini shaders; build's zero-warning summary does not mean warning-free runtime.

Independent reviewer launch:

```powershell
# New client only; never the reference client.
Start-Process Builds/Windows/JXClientNew.exe -WindowStyle Hidden -ArgumentList '-batchmode -cn00SmokeTest -cn00Screenshot .tmp/reviewer_runtime_batch.png -logFile .tmp/reviewer_runtime_batch.log -screen-width 1280 -screen-height 720 -screen-fullscreen 0' -Wait -PassThru
```

Actual execution used absolute paths and quoted arguments. The first hidden non-batch run initialized OfflineReady but did not reach 30 frames within 25 seconds; reviewer terminated only its own PID 22332. The second run with `-batchmode` completed exit 0 after 30 frames and Ready=True. Screenshot failed (`Failed to capture screen shot.`), so that run is **not** counted as visual evidence. The independently opened Antigravity PNG supplies the visual proof. Do not treat screenshot-request logs as proof of image creation in future automation.

`reports/evidence/CN00-Reviewer-Snapshot.json` pins source/asmdef/scene/.meta hashes and build/test/runtime observations. CN00-07 remains pending: source and scene must be committed, provenance recorded, main checkpoint reconciled, and worktree state verified by the gate owner. Reviewer has not taken over files claimed by Antigravity or marked the overall gate PASS.


## Final reviewer decision — CN-00 PASS

Reviewed source commit 55b54b8, handoff 7d419db. All 97 tracked UnityProject files inspected for scope; no generated Library/Logs/Temp/Builds tracked. Working tree clean at handoff before reviewer sign-off edits. Source hashes match reviewed snapshot (0 drift). Provenance archive SHA256 corrected from file length to actual hash. Current README/board/checkpoint reconciled. CN-01 is READY, unclaimed and not implemented. Earlier sections retain the audit history, not the final gate status. No public push or production changes. Runtime limitations above remain follow-up items.
