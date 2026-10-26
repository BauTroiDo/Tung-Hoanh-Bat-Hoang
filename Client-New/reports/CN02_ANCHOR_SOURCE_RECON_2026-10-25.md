# CN02-04 — Read-only Map53 anchor evidence reconnaissance

**Date:** 2026-10-25, UTC+07. **Owner:** ChatGPT. **Branch/base:** `cg/cn02-anchor-evidence` / `f30744c`.
**Scope:** Bounded read-only review of existing Client-New evidence and selected local reference records. No reference execution, asset extraction, server connection, Unity batch process, or Unity project source edit.
**Verdict:** **BLOCKED / NOT VERIFIED** — 0 verified 3+1 pairs for the same reference scene/build and the Client-New BaLangHuyen scene.

## Safety and ownership checks

- Read `AGENTS.md`, `docs/NEXT_SESSION_HANDOFF.md`, `docs/CHECKPOINT.md`, `TASK_BOARD.md`; additionally reviewed CN02 reports and coordinate protocol.
- Root `git status --porcelain=v1 -uall` was empty, branch `cg/cn02-anchor-evidence`, HEAD `f30744c`. Isolated `.tmp/coordinate-worktree` status was also empty; no branch reset/merge was attempted.
- Existing Unity Editor PID 17540 uses `Client-New/UnityProject`; PID 19304 is its import worker. Did not launch another Editor or close either process.
- Latest historical Antigravity/Codex claims are explicitly handed off/released in `TASK_BOARD.md`. This task only claims this report and three shared documentation files.

## Inventory / source identity (read-only)

| Local evidence | Observation | Usable as 3+1 measured pairs? |
|---|---|---|
| `Client-New/reports/evidence/anchors/drafts/map53-anchors-20261025-102929-20804c5bad4445bda82ad52a3d12be81.json` | 4 saved-scene Unity markers, all `hasLegacy=false`, provenance `cn02-synthetic-v1`, status `DRAFT_UNPAIRED` | **No.** No legacy point or measurement proof |
| `Client-3D-2/LAN-LOCAL/M1-map53-region.json` | Two `receivedNpcPosition` events: actor 41319 `(51889,102090)` and actor 41320 `(51889,102105)`; both legacy-only, same X | **No.** Collinear and no corresponding 4016/Client-New Unity world coordinates; actor positions are not automatically fixed landmarks |
| `Client-3D-2/LAN-LOCAL/M1-map53-fixed.json` and `M1-map53-earlyroot.json` | Network/map loading events, no matched Unity world-space point observations | **No** |
| `Clent-3D/LAN/world_conversion.py` and `reference/world_baling.markers.json` | Only BeginPoint01 maps legacy `(52032,101696)` to Unity2022 `(209.8,5.904738,103.71)`; historic converter has `movement_calibrated=False` | **No.** Single spawn-only hint from a *different* scene/build |
| `Client-3D-2/LAN-LOCAL/Map53Official4016/` | Official-named Map53 4016 assets and scene bundle are present; no independently measured paired marker records in the reviewed text evidence | **No.** Bundle presence/hash is not coordinate evidence or a redistribution license |
| `Client-3D-2/LAN-LOCAL/Map53Official4016/additional-proofs.json` | Hash/URL inventory of other map bundles; no paired Map53 observations | **No** |

Read-only file scan: `Get-ChildItem ... -Filter "M1-map53-*.json" | Select-String -Pattern 'worldPos|scenePos|unityPos|anchor|receivedNpcPosition|legacyCoord|sceneX|sceneZ'` returned only two `receivedNpcPosition` matches from `M1-map53-region.json`. This is a **bounded scan**, not a machine-wide proof of nonexistence.

## Integrity measurements

- `Client-3D-2/update/Game.exe`: SHA256 `1CAE3982C04BE9D6F8F7EA8CE272600400655D40FBF0671818EFF1D7A79750EE`, matching documented reference baseline.
- `Client-3D-2/update/GameAssembly.dll`: SHA256 `011357720B210AA1F392A8B62FF8DE090D3A895230A6E0135BCE707196871069`, matching baseline.
- `M1-map53-region.json`: SHA256 `EE997AD6763C1EA2DCB7DC2710330A8C2F631558625F7E18000A001965C6DBB0`.
- Official-named 4016 BLH assets bundle: SHA256 `73283BA18C144FB37286CAAD5F6BF4DB4F17C25BD94A3B470FA3E5D07A5DF4D1` (31,592,781 bytes).
- Official-named 4016 BLH scene bundle: SHA256 `EEB567D492CE740AAD8A796F531D993917F511FDA60ECF4DDE25DA6C2B4DEC4A` (9,738,990 bytes).

These checks demonstrate identity/immutability against the recorded baseline, not asset use rights or geometry correctness.

## CN02-04 actual unblock criteria

1. **Choose and freeze one mapping target**: the 4016 `BLH_Bake 3` reference scene *or* a legitimately rebuilt Client-New scene with identified corresponding world landmarks. Current synthetic marker names/positions are not established as the same physical landmarks in 4016. Do not fit unlike geometry and declare original-map fidelity.
2. **Obtain authority to observe the reference runtime** before running 4016 or using any asset. No such runtime was launched during this audit. Preserve reference binaries and original server.
3. **Record four independent matched observations**: one stable landmark identity per row, verified legacy JX1 Map53 X/Y and target Unity world X/Z for that *same* landmark, the exact build, scene/content hash, capture timestamp, source screenshot/log/file hash, measurement method and axis convention.
4. **Use three non-collinear fit points** spread across the scene; reserve the fourth independently observed point as a holdout. Neither interpolation from one historic anchor nor repeated player positions with the same X can substitute.
5. **Assess the saved evidence** in `JXClient > CN-02 > Anchor Evidence (3+1)`; choose a gameplay-justified world-unit tolerance, verify bounds and residual, and separately review source authenticity. `ReadyForReview` does not imply `Verified`.
6. Only after measured evidence and independent review should the coordinate mapping be proposed for runtime; do not open CN03 from synthetic fixtures.

## Validation and conclusion

- New Unity build/tests: **not run**, because Editor is already open and no C#/scene source changed.
- Prior milestone evidence remains: EditMode **29/29**, PlayMode **18/18**, Win64 build `exit 0` (`reports/CN02_ANCHOR_WORKFLOW.md`); these are historical verified results, not a fresh test run.
- No writable action occurred outside `Client-New`. No files were copied from reference, no client/server processes were stopped or modified, and no data was pushed.
- **Gate remains CN02 PARTIAL; CN02-04 BLOCKED/NOT VERIFIED; CN03 CLOSED.**
