# CN02-04 — Reverse analysis of original client 4016 (static, read-only)

**Date:** 2026-10-25 18:19 UTC+07. **Owner:** ChatGPT. **Branch:** `cg/cn02-anchor-evidence`.
**Scope:** An authorized next-step analysis of the user's locally available 4016 scene/asset bundles and IL2CPP metadata. Only read-only parsing was performed on reference inputs; no original `Game.exe` execution, server call, modified asset, binary patch, player login, or original asset import into Client-New.
**Gate:** CN02-04 remains **BLOCKED / NOT VERIFIED**. CN03 remains CLOSED.

## Preflight

- Read existing AGENTS.md, NEXT_SESSION_HANDOFF.md, CHECKPOINT.md and TASK_BOARD.md in the preceding checkpoint.
- Git branch `cg/cn02-anchor-evidence`, HEAD `542c13e`, clean before claim; coordinate worktree retained. No Git remote configured.
- Unity 6000.6.7f1 Editor PID 17540 already running the correct project; left open and did not launch a second Editor. Antigravity previous file claims were released. This pass claimed only tools/reference4016/**, reports/evidence/cn02-4016-static/**, this report and shared handoff/board/checkpoint files.
- Tool: UnityPy 1.25.4 installed into ignored `.tmp/reference4016-python-deps` (not into UnityProject). Static scripts were created by us and are source controlled. Python 3.13.

## Confirmed source files

| Reference file (read-only) | Length | SHA256 |
|---|---:|---|
| `Client-3D-2/LAN-LOCAL/Map53Official4016/pc_balanghuyen_scenes_all_4e0a8e141624756b20ca4ccfa32087fb.bundle` | 9,738,990 | EEB567D492CE740AAD8A796F531D993917F511FDA60ECF4DDE25DA6C2B4DEC4A |
| `Client-3D-2/LAN-LOCAL/Map53Official4016/pc_balanghuyen_assets_all_53fcdd524b8c67eafad5c013f036ed4b.bundle` | 31,592,781 | 73283BA18C144FB37286CAAD5F6BF4DB4F17C25BD94A3B470FA3E5D07A5DF4D1 |

Both files have `UnityFS` headers and encode Unity version `6000.3.9f1` (different from Client-New editor `6000.6.7f1`). The scene bundle's AssetBundle scene hash references `Assets/_SyncAsset/Dev-Map/JX1_BaNi/-----Prefab/BLH/PC/PC_BLH_Bake.unity`. This is source-built baked scene metadata, not the editable Client-New fallback.

Original `Client-3D-2/update/Game.exe` SHA256 remains `1CAE3982C04BE9D6F8F7EA8CE272600400655D40FBF0671818EFF1D7A79750EE`; `GameAssembly.dll` remains `011357720B210AA1F392A8B62FF8DE090D3A895230A6E0135BCE707196871069` (matches the documented baselines).

## Reverse-engineered scene hierarchy and geometry metadata

- **Scene bundle:** 152 Unity objects: 29 GameObjects, 29 Transforms, 20 MeshRenderers, 20 MeshFilters, 6 Mesh objects, 7 MonoBehaviours, plus lighting/material/texture data. All 29 scene Transforms are readable with no unresolved parent in this bundle.
- **Separate assets bundle:** 1,680 objects, including 539 GameObjects, 539 Transforms, 337 Mesh objects. These include prefab/animation data; their existence **does not** prove corresponding objects are placed in the world scene.
- **Terrain:** four scene-space nodes `TerrainBLH2a_01_01`, `TerrainBLH2a_01_02`, `TerrainBLH2a_02_01`, `TerrainBLH2a_02_02`, on a 250-unit X/Z pitch, all with identity local rotation and world scale `(1,1,1)`.
- Scene `TerrainBLH2a` Mesh local AABB center `(125,17.5,125)`, half-extents `(125,2.5,125)`, vertexCount `9025`. This describes a 250 × 250 horizontal *local* mesh extent, not independently measured walkable/raycast terrain boundaries; other terrain-linked objects refer to statically combined meshes or external serialized-file entries.
- Root `NenMap` world origin `(-503.920013,-20,-497.980011)`, which **must not** be assumed to be a player spawn or a legacy-to-Unity offset.
- Nominal tile-origin range in this scene is X `[-253.920013,-3.920013]`, Z `[-247.980011,2.019989]`, with four tile origins spaced 250. The 2x2 tile *layout* spans nominally 500 units per axis if each terrain tile covers its 250-unit local square; exact usable map/collision bounds require independent geometry/terrain checks.

### Four actual Unity scene tile origins (NOT legacy pairs)

| Candidate | Unity world X | Unity world Y | Unity world Z | Legacy pair |
|---|---:|---:|---:|---|
| TerrainBLH2a_01_01 | -253.920013 | -20 | -247.980011 | Not observed |
| TerrainBLH2a_01_02 | -253.920013 | -20 | 2.019989 | Not observed |
| TerrainBLH2a_02_01 | -3.920013 | -20 | -247.980011 | Not observed |
| TerrainBLH2a_02_02 | -3.920013 | -20 | 2.019989 | Not observed |

These are actual parsed original scene Transform origins, but they are not independently matched physical landmarks in the legacy world. The artifact `reference-tile-origin-candidates.json` explicitly has `legacyMap53XY=null`, `hasLegacy=false`, status `UNPAIRED_REFERENCE_SCENE_ONLY` and labels the fourth point a *holdout candidate only*. It is **not** the Map53 calibration document for the synthetic `BaLangHuyen.unity`, and must never upgrade CN02-04 status.

## IL2CPP metadata reverse reconnaissance

Read only `Client-3D-2/update/Game_Data/il2cpp_data/Metadata/global-metadata.dat` (20,101,168 bytes). An identifier-only bounded scan found candidates including:
`Mps2WorldPos`, `Map2Mps`, `Map2WorldPos`, `MapToWorldPos`, `MpsToMapPos`, `GridToMpsCentreX`, `GridToMpsCentreY`, `get_MpsWorldScale`, `_mpsWorldScale`, `GetMpsX`, `GetMpsY`, `m_cSubworldCoord`, `_serverTargetWorldPos`, `LoadAddressableMapSceneAsync`.

**Crucial limitation:** identifier strings do not prove any method's implementing algorithm, assembly owner, call path, active code, axis convention or numerical scaling. The earlier Unity2022 `1/32` value remains an unverified historical hint and must not be inferred as the 4016 conversion. No native function decompilation or hook was performed in this pass.

## Reproducible commands and tests

Run in local PowerShell with `<root>` = `E:\Games\VLTK3D\SwordOnline\Client-New` and `<scene>` = original scene bundle path above:

```powershell
python "<root>\tools\reference4016\probe_4016.py" "<scene>"
python "<root>\tools\reference4016\scene_transform_inventory.py" --bundle "<scene>" --out "<root>\reports\evidence\cn02-4016-static\scene-transform-inventory.json"
python "<root>\tools\reference4016\mesh_bounds_inventory.py" --bundle "<scene>" --out "<root>\reports\evidence\cn02-4016-static\mesh-bounds-inventory.json"
python "<root>\tools\reference4016\make_reference_candidates.py" --input "<root>\reports\evidence\cn02-4016-static\scene-transform-inventory.json" --out "<root>\reports\evidence\cn02-4016-static\reference-tile-origin-candidates.json"
python "<root>\tools\reference4016\verify_4016_inventories.py"
```

These commands were actually run, plus an asset-bundle probe and IL2CPP identifier scan; all completed with process exit code 0. **Regression checks 8/8 PASS**, including source hash, 29 transforms, four terrain transforms, 250-unit terrain mesh metadata, scene name and explicit prohibition on treating this as 3+1 calibration. No Unity EditMode/PlayMode rebuild was run because Client-New C#/scene source was unchanged and the Editor was open.

Generated metadata-only evidence:
- `reports/evidence/cn02-4016-static/scene-transform-inventory.json`
- `reports/evidence/cn02-4016-static/mesh-bounds-inventory.json`
- `reports/evidence/cn02-4016-static/reference-tile-origin-candidates.json`
- `reports/evidence/cn02-4016-static/il2cpp-coordinate-symbol-hints.json`

## What the next implementation step actually requires

1. **Restore algorithm semantics by static IL2CPP disassembly**: map metadata methods such as `Mps2WorldPos` / `Map2Mps` / `get_MpsWorldScale` to actual GameAssembly.dll function implementations. Verify inputs, constants, axes, map origin, rounding, world-map context and 4016 build identity. Do not infer these from names alone.
2. Obtain four **independently corresponding** legacy Map53 locations and original 4016 scene-space locations, tied to the same content/build and landmark. A recovered algorithm can propose sample pairs but cannot substitute for independently observed holdout evidence.
3. Only when the real mapping and use rights of original world geometry are resolved, separately decide how to rebuild a corresponding editable Client-New scene. Do not silently overwrite or "calibrate" 56 × 56 synthetic fallback using 4016's 250-unit original tiles.
4. Have an independent reviewer validate anchor provenance, non-collinearity, bounds, residual and runtime ground alignment; **only then** consider CN02-04 PASS and CN03 opening.

## Safety / rollback

No server or player touched. No reference binary/bundle modified or launched, no extracted meshes/textures/materials saved or copied to UnityProject, no original scripts/IL2CPP bodies committed, no public push. `.tmp/reference4016-python-deps` is ignored; code is entirely under `tools/reference4016`. Revert the local documentation/tools commit to undo this addition without affecting prior CN01/CN02 runtime assets. Original asset redistribution rights remain UNKNOWN.
