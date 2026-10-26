# CN02-04 — Static dispatch audit of MapService.Calibrate (client 4016)

**Date:** 2026-10-25 UTC+07. **Branch:** `cg/cn02-anchor-evidence`. **Prior commit:** `52bc036`.

**Status:** Static dispatch audit complete, Map53 runtime coefficient values NOT VERIFIED, 3+1 anchors missing. CN02-04 BLOCKED; CN03 CLOSED.

## Read-only source identity

- Original installed `Client-3D-2/update/GameAssembly.dll`, SHA256 `011357720B210AA1F392A8B62FF8DE090D3A895230A6E0135BCE707196871069`.
- Original reference record `Client-3D-2/LAN-LOCAL/M1-map53-region.json`, used read-only and checked against its embedded GameAssembly hash.
- Original map scene metadata previously recovered in `reports/evidence/cn02-4016-static/reference-tile-origin-candidates.json`; every tile point is unpaired with legacy.
- No reference processes started. Nothing changed under source client, scene/bundle, UnityProject, Bishop, GS, gateway, database or player characters.

## Native callsite / pointer analysis

The original MapService.Calibrate token and codegen pointer mapping was independently rechecked:

| Item | Result |
|---|---|
| MethodDef | `MapService.Calibrate`, token `060013B8` |
| Native RVA | `0x8DD750` |
| Native VA (preferred image base) | `0x1808DD750` |
| CodeGenModule method pointer slot RVA | `0x5CD7B88` |
| Raw PE file offset of pointer slot | `0x5CD5F88` |
| Section | `.data` |
| Absolute pointer occurrences in original binary | **1**, the above method slot |
| Direct `CALL rel32` and `JMP rel32` targeting Calibrate | **0 matches** across executable sections |

The pointer slot RVA is independently computed as `modulePointerArrayRVA + (MethodDefTokenRow - 1) * 8` and compared to the exact 8-byte native VA stored at that slot. The scanner also rejects a changed binary hash or an unpaired reference scene. This closes a source-of-truth gap in the previous textual scan.

**Interpretation:** no statically stored additional absolute pointer to Calibrate and no direct E8/E9 call-site were found in the scanned PE. This is NOT proof the method is unused: indirect/interface calls, dynamically constructed delegates, reflection, icalls, registration-generated dispatch, memory patches and runtime-loaded modules cannot be excluded by this scanner. Do not label it "never called."

The prior native disassembly already confirmed `MapService.Calibrate` calls `Solve3x3` twice and writes the six static fields. Separately, `MapService..cctor` contains the six default float32 constants. This audit does **not** prove which set remains active when scene Map53 is loaded.

## Bounded numeric sanity check — only a diagnostic

The Map53 local-event JSON carries two `receivedNpcPosition` payloads:
- NPC 41319: `x=51889, y=102090`
- NPC 41320: `x=51889, y=102105`

The file also records `receivedWorldMap.subWorld = 53`. It does **not** include paired coordinates measured on the actual 4016 scene, nor proof that `receivedNpcPosition.x/y` have the same units/axis convention as the `MpsX/MpsY` arguments.

For a **what-if-only** calculation treating those numbers as MPS, applying the confirmed six static `.cctor` defaults yields:

| NPC | Hypothetical Unity X | Hypothetical Unity Z | Inside nominal 2x2 tile envelope? |
|---|---:|---:|---|
| 41319 | +245.23 | -375.90 | No |
| 41320 | +245.34 | -375.80 | No |

The nominal reference-scene tile envelope from four real tile origins is X `[-253.920, +246.080]`, Z `[-247.980, +252.020]`. This is **not necessarily playable map/collider bounds**, and may omit additive scene content. The actor inputs are not independently paired world positions. **This calculation cannot establish that static defaults are wrong or right for Map53**, and must not be used to create fake 3+1 anchors or a passed calibration.

## Reproducibility

New source:
- `tools/reference4016/il2cpp/calibrate_dispatch_audit.py`: SHA guard, exact IL2CPP method-slot matching, direct E8/E9 target scans, read-only source log and 2-point conditional projection.
- `tools/reference4016/il2cpp/verify_calibrate_dispatch.py`: 10 regression checks with provenance and explicit unpaired-state assertions.

Output evidence: `reports/evidence/cn02-4016-il2cpp/calibrate-dispatch.json`.

Verified command, from `Client-New` as working directory:

```powershell
python tools/reference4016/il2cpp/calibrate_dispatch_audit.py --binary "E:\Games\VLTK3D\SwordOnline\Client-3D-2\update\GameAssembly.dll" --methods reports/evidence/cn02-4016-il2cpp/map-field-source.json --scene-candidates reports/evidence/cn02-4016-static/reference-tile-origin-candidates.json --module reports/evidence/cn02-4016-il2cpp/pe-module-candidates.json --map53-log "E:\Games\VLTK3D\SwordOnline\Client-3D-2\LAN-LOCAL\M1-map53-region.json" --out reports/evidence/cn02-4016-il2cpp/calibrate-dispatch.json
python tools/reference4016/il2cpp/verify_calibrate_dispatch.py
python tools/reference4016/il2cpp/verify_map_field_source.py
```

On Windows, all commands exited 0. New tests **10/10 PASS**; prior MapService field suite rerun **10/10 PASS**. No Unity PlayMode/build was run because UnityProject source was untouched.

## Next technical scope

Find which scene lifecycle/provider paths can set `MapService` field data (possibly by reflection/delegate or explicit static field assignment), or obtain an authorized independent Map53 runtime snapshot. Trace only the required asset metadata; do not import source 4016 geometry with unknown rights. Confirm any claimed mapping with at least 3 non-collinear corresponding legacy↔Unity landmarks and an independent fourth holdout before applying to Client-New.

CN02-04 stays BLOCKED / NOT VERIFIED; CN02 PARTIAL; CN03 CLOSED.
