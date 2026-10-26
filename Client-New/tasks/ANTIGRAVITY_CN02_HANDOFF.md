# Antigravity — Independent CN01 Review → CN02 World/Scene Implementation

**Workspace:** E:/Games/VLTK3D/SwordOnline/Client-New
**Handoff baseline:** commit 755747f, branch cg/cn01-offline-movement (read latest HEAD/checkpoint before acting).
**User direction 2026-10-25:** user accepted the playable result ("ok rồi") and requested continuation plus assignment of other components to Antigravity. This is user acceptance, not a newly captured physical-click trace or an independent code review.

## Start here

Read AGENTS.md, README.md, TASK_BOARD.md, docs/CHECKPOINT.md, reports/CN-01_ACCEPTANCE.md, docs/CN02_COORDINATE_EVIDENCE.md and tasks/CN-01_TO_CN-06.md. Check current Git changes and claims. Work from the branch containing 755747f and subsequent handoff docs, never the stale ag/cn00-bootstrap baseline. Create a new work branch from current verified HEAD if safe; preserve and coordinate any uncommitted work. Record exact paths before edits. Do not reset, clean, overwrite or recreate the project.

Unity already installed: C:/Program Files/Unity/Hub/Editor/6000.6.7f1/Editor/Unity.exe. Keep Unity 6000.6.7f1, URP 17.6.0 and current packages unless an actual incompatibility is demonstrated. No environment rescan/reinstall or CN00 redo is needed.

## 1. Independent review of CN01 first

You are the reviewer of Codex implementation 755747f. Review the delta from b0dfacc, actual source and stored evidence. Independently run full EditMode/PlayMode tests and normal Win64 MovementTest build/runtime. Existing expected baseline: 9 PlayMode + 3 EditMode, build/runtime exit 0, all .meta tracked. Recheck movement, UI input shielding, missing ground, wall collision and three orbit/zoom cases. The minMoveDistance tiny-frame regression is intentional; do not remove it or reintroduce the default threshold.

Write reports/CN-01_REVIEW.md with actual commands, exit codes, evidence and limitations. Record the user's acceptance without inventing physical mouse timestamps or asking them to accept the same result again. If review passes, mark CN01 PASS and CN02 IN_PROGRESS in TASK_BOARD and checkpoint, then continue step 2 autonomously. If a defect exists, add a regression, fix it with an explicit narrow claim, verify and document it before proceeding. Do not self-certify unrelated gates.

## 2. Your implementation scope — CN02 World / environment / integration

Build a real editable Ba Lang Huyen scene under UnityProject/Assets/_JXClient/:

- World/**: map scene definition/catalog for MapId 53, async load/unload with cancellation/failure handling, ground/camera/spawn validation. Offline scene ready must remain distinct from network WorldReady.
- Scenes/BaLangHuyen.unity and scene .meta: roots Ground, Environment, StaticProps, SpawnPoints, Colliders, Lighting, Nav; visible ground, colliders, coherent scale, useful camera bounds. No fake Map53 label over an empty/blue view.
- Environment/** and approved prefabs/materials: terrain, paths/landmarks and obstacles appropriate to the scene. Prefer new synthetic assets until actual rights are documented. Explicitly label synthetic fallback; never describe it as exact restoration of original BLH.
- Editor/Cn02ProjectSetup.cs (or similarly scoped builder): generate/validate assets through real Unity Editor APIs; reproducible scene/build commands. Preserve Bootstrap, MovementTest, current player prefab and their test paths.
- Tests/PlayMode/Cn02*: load/unload, renderer/collider readiness, five named ground probes, safe spawn on ground, camera bounds, and offline movement through the scene. Keep all CN00/CN01 regression tests.
- Builds/CN02-Windows/**: a separate actual Win64 build, four real rendered camera views, runtime logs and a small measured performance baseline. Do not overwrite Builds/Windows or Builds/CN01-Windows.

Read-only inventory may examine existing local 4016 map bundle/catalog paths referenced in docs/04_ASSET_SCENE.md. Record names, size, SHA256, Unity version and rights status. No reference execution, binary patching, bulk extraction or import of unknown-rights assets. Work around missing assets using synthetic content while preserving the unproven status of original-map restoration.

## 3. Division of ownership

**Antigravity:** World/**, Environment/**, BaLangHuyen scene, new map-specific prefabs, Cn02 editor tools, Cn02 PlayMode tests, CN02 builds and reports/CN-02_ACCEPTANCE.md/evidence/cn02/**.

**Codex/ChatGPT:** docs/03_WORLD_COORDINATES.md, docs/CN02_COORDINATE_EVIDENCE.md, future Domain/Coordinates/** + Tests/EditMode/Coordinates/**, coordinate calibration/contract review. Those future source paths are reserved by task assignment, not implemented or claimed as passing yet. Do not build a second coordinate converter in World; agree the data boundary first.

Shared checkpoint, board and provenance manifest require short explicit claims at each handoff. Changes to existing Movement/** or existing tests need a narrow reason/claim; do not replace working movement wholesale. Do not rename or delete existing baseline evidence.

## 4. Coordinates / missing evidence

Use docs/CN02_COORDINATE_EVIDENCE.md. Supply three non-collinear matching legacy/scene anchors plus an independent fourth point, with source evidence and measured error. A single historical (52032,101696) ↔ (~209.8,5.904738,103.71) pair is unverified and cannot establish the transform.

If real anchors are unavailable, still finish the editable synthetic scene and offline integration. Report CN02-04 BLOCKED/NOT VERIFIED and keep overall CN02 below PASS; synthetic fixtures prove only mathematics, not real Map53 calibration. Height comes from ground/terrain; legacy Y is a horizontal coordinate, not Unity height. Never invent coefficients, spawn positions claimed to be server-authoritative, or original landmarks presented as verified.

## 5. Deliver and stop at the correct gate

Produce reports/CN-01_REVIEW.md and reports/CN-02_ACCEPTANCE.md; exact changed files, Unity commands/exits, all suite counts, build artifacts/hashes (include actual game assemblies/scene data, not shared Unity launcher alone), four runtime images, provenance, known gaps, performance measurements and rollback. Update board/checkpoint, commit small milestones and report Git status. Never claim a push without a configured/verified remote.

Do not start CN03 networking, auth/roles, gateway rollout, combat, PvP or Web/Mobile deployment. Keep Bishop, gs001.exe, databases, existing characters and reference clients untouched. The production server must not be changed to make the client test pass.
