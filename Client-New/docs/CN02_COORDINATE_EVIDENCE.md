# CN02 — Coordinate Evidence and Ownership Boundary

**Status:** numerical module implemented and tested on isolated branch cg/coordinate-math; real calibration UNVERIFIED. 2026-10-25 UTC+07.
**Owner:** Codex/ChatGPT. Scene integration partner: Antigravity.
**Gate:** CN02 implementation starts only after independent CN01 review passes. User acceptance of CN01 recorded separately.

## Required data

A map definition associates MapId=53 with scene key, content/provenance version, ground layer, offline spawn marker, scene bounds and a calibration status. Synthetic scene identity must be visible in reports and metadata. Loading scene content alone never grants network WorldReady or authoritative spawn.

A calibration record needs:

| Field | Meaning |
|---|---|
| mapId / calibrationVersion | Explicit map identity and revision |
| sourceKind | verified-reference / synthetic-test / unknown |
| units / axisConvention | Legacy horizontal X/Y and Unity horizontal X/Z orientation |
| anchors[0..2] | Three non-collinear paired points used for fitting |
| holdout | Fourth independently measured paired point, not used in fitting |
| evidence | Source file/log/image path, timestamp, reference/content version and hash for each point |
| coefficients | Finite 2x2 affine matrix plus 2D translation; inverse must exist |
| bounds | Legacy and scene horizontal domains, validated before movement |
| errors / tolerance | Per-point residuals, holdout error, units and justified tolerance |
| status | UNVERIFIED / SYNTHETIC_TEST_ONLY / VERIFIED |

No verified record currently exists. The old single anchor in docs/03 is a research hint only.

## Transform behavior to implement after handoff

Domain math uses double precision and no UnityEngine dependency. Horizontal conversion is `[unityX, unityZ] = A*[legacyX,legacyY]+b`; inverse recovers legacy X/Y. Reject NaN/Infinity, singular/near-singular or collinear fits, missing/duplicate points and points outside configured bounds. Negative coordinates are valid when in bounds. Scene integration obtains Unity height from ground raycast/terrain, outside the pure coordinate module.

Synthetic tests should cover identity, scale/translation, rotation/reflection, round-trip, negative coordinates, bounds, degeneracy, non-finite values and an independent fourth-point residual. All synthetic numbers must be labelled as test fixtures. Passing those tests must not upgrade real calibration status.

## Division of work

- Antigravity supplies editable world geometry, named anchor markers/probes, scene bounds, ground height sampling and the evidence table. It may retain scene-space offline walking without pretending it is a calibrated legacy mapping.
- Codex owns the future pure math module and EditMode tests in Domain/Coordinates/** and Tests/EditMode/Coordinates/**, plus review of the evidence/threshold. The source module is delivered in the coordinate component commit; see reports/CN02_COORDINATE_IMPLEMENTATION.md for API and integration evidence.
- Avoid a competing transform hidden in map loader, Movement or a server adapter. Preserve docs/02_PROTOCOL.md as a draft until the networking gate.

## CN02 acceptance boundary

CN02-01/02/03/05/06 can gather scene/build evidence with synthetic content. CN02-04 requires actual verified 3+1 anchors. If missing, report it precisely and do not sign overall gate PASS or open CN03. This does not prevent completing and committing useful scene work.

## Numerical module delivered

Namespace JXClient.Domain.Coordinates: PlanarPoint, PlanarBounds, CoordinateAnchor, AffineMapTransform.Fit/ToScene/ToLegacy/HoldoutError. Unity EditMode 14/14 (11 new), PlayMode 9/9 on isolated baseline. All fixture coordinates are synthetic. Module readiness does not upgrade real calibration status or CN02-04. Review report and verification.json identify exact scope and commands.


## Scene-side anchor candidates & ground probes supplied by Antigravity — 2026-10-25

Antigravity implemented `UnityProject/Assets/_JXClient/Scenes/BaLangHuyen.unity` (`cn02-synthetic-v1`, `MapCalibrationStatus.Unverified`) without adding any competing coordinate converter in `World/**`.

### 1. Five verified Layer-8 Ground raycast probes (`CN02-03` — PASS)

| Probe Name | Horizontal `(X, Z)` | Measured Raycast Hit `(X, Y, Z)` | Hit Normal | Hit Collider |
|---|---|---|---|---|
| `Probe_CenterPlaza` | `(0.00, 0.00)` | `(0.00, 0.04, 0.00)` | `(0.00, 1.00, 0.00)` | `BLH_CenterPlazaGround` |
| `Probe_NorthGatePath` | `(0.00, 18.00)` | `(0.00, 0.02, 18.00)` | `(0.00, 1.00, 0.00)` | `BLH_NorthSouthRoad` |
| `Probe_SouthGatePath` | `(0.00, -18.00)` | `(0.00, 0.02, -18.00)` | `(0.00, 1.00, 0.00)` | `BLH_NorthSouthRoad` |
| `Probe_EastBridgeApproach` | `(18.00, 0.00)` | `(18.00, 0.03, 0.00)` | `(0.00, 1.00, 0.00)` | `BLH_EastWestRoad` |
| `Probe_WestMarketApproach` | `(-18.00, 0.00)` | `(-18.00, 0.03, 0.00)` | `(0.00, 1.00, 0.00)` | `BLH_EastWestRoad` |

### 2. Scene marker transforms under `SpawnPoints` awaiting real legacy `3+1` pairing (`CN02-04` — BLOCKED / NOT VERIFIED)

| Scene Marker Transform (`SpawnPoints/*`) | Unity Scene Position `(X, Y, Z)` | Paired Legacy `(legacyX, legacyY)` | Calibration Status |
|---|---|---|---|
| `OfflineSpawn_VillageCenter` | `(0.00, 0.05, -3.00)` (sampled ground `Y = 0.04`) | Unpaired (offline scene spawn only) | `SYNTHETIC_SCENE_ONLY` |
| `AnchorCandidate_CenterPlaza` | `(0.00, 0.05, 0.00)` | Missing verified legacy anchor | `UNVERIFIED` |
| `AnchorCandidate_NorthGate` | `(0.00, 0.05, 18.00)` | Missing verified legacy anchor | `UNVERIFIED` |
| `AnchorCandidate_EastRoad` | `(18.00, 0.05, 0.00)` | Missing verified legacy anchor | `UNVERIFIED` |
| `AnchorCandidate_SouthWestCorner` | `(-14.00, 0.05, -14.00)` | Missing verified holdout anchor | `UNVERIFIED` |

- **Current CN02-04 status:** `BLOCKED` / `NOT VERIFIED` (no fabricated legacy coordinates or affine coefficients were invented; single historical hint `(52032, 101696) <-> (~209.8, 5.904738, 103.71)` is insufficient to fit a non-collinear 2D affine transform).
- **Integration boundary with `JXClient.Domain.Coordinates`:** `WorldSceneValidator.TrySampleGroundHeight(scene, horizontalPos, groundLayer, out groundPoint, out hitInfo)` provides the Unity `Y` height lookup from the horizontal `(unityX, unityZ)` output of `AffineMapTransform.ToScene` once verified `3+1` anchors are supplied.

## Anchor Evidence Editor tool — implementation available

Open Unity menu **JXClient > CN-02 > Anchor Evidence (3+1)**. The window uses UI Toolkit and lives under Editor/CalibrationTools. It does not change MapSceneCatalog or send network traffic.

1. Opening the menu loads BaLangHuyen and preloads the form (Unity asks before discarding unsaved scene edits). Capture markers again only to start a fresh draft. The tool reads the four named SpawnPoints markers from the saved BaLangHuyen scene; it refuses dirty scene capture and does not save/modify geometry. Legacy values remain explicitly unpaired (`hasLegacy=false`).
2. Declare reference build identity, valid legacy bounds and a justified positive holdout tolerance in scene units. Scene bounds initially come from the map catalog; they may be reviewed/adjusted explicitly.
3. Enter measured legacy coordinates for the three fit points and independent fourth holdout; confirm each was actually measured. Attach local evidence files (up to 16 MiB) with event/measurement timestamps and matching map/build/landmark context. SHA256 is computed from the selected file; file content is not copied or uploaded.
4. Assess candidate. Four measured-reference labels/hashes, distinct locations, finite/invertible geometry, bounds and holdout error are required. A changed/missing evidence file downgrades readiness. Synthetic/unknown provenance never becomes ready.
5. Export draft JSON. Imports require schema/map/scene/provenance compatibility, current scene hash, exactly four markers and matching marker coordinates. File status is recalculated; `independentReviewRequired` is always true.

Highest numerical status is `ReadyForReview`, never Verified. File integrity cannot establish semantic authenticity of a measurement. The reviewer must read referenced evidence, confirm source timestamps/build/landmark correspondence, justify tolerance and approve actual calibration separately. `createdUtc` records draft creation, not an invented legacy measurement timestamp.

For a reproducible unpaired draft, execute Editor method `JXClient.Editor.Calibration.AnchorCalibrationTools.ExportCurrentSceneDraft` in batch mode. Outputs go to reports/evidence/anchors/drafts/ with unique filenames. No converter coefficients or calibration status are applied to runtime automatically.

### Historical anchor clarification

Read-only source audit found Clent-3D/LAN/world_conversion.py and reference/world_baling.markers.json. The sole BeginPoint marker is BeginPoint01=(209.8,5.904738,103.71), paired in that old source with legacy (52032,101696). Its converter uses a default scale 1/32 and explicitly returns coordinate_alignment=spawn_anchor, movement_calibrated=False. This is the Unity2022 world_baling reference, not evidence of 4016/new synthetic BaLangHuyen geometry equivalence. See reports/evidence/anchor-workflow/legacy-source-audit.json. Do not generate three extra "anchors" from that formula and claim they were independently observed.
