# CN02-04 — Original 4016 WORLD_SYNC struct and named LoadMap parameters

**Date:** 2026-10-25, UTC+07. **Branch:** `cg/cn02-anchor-evidence`. **Prior commit:** `5a956bd`.
**Status:** Original input struct identity and named map-selector transfer recovered; **CN02-04 BLOCKED**, **CN03 CLOSED** until independent live Map53 affine coefficients and 3+1 anchors.

## Scope and source identity

All source files accessed read-only on the connected development computer:

- Client 4016 `Client-3D-2/update/Game_Data/il2cpp_data/Metadata/global-metadata.dat`, SHA256 `5586F416AA26EBE57EA7F6580D0A53E5E5F29C50E425E64A4D3F2A95D6069E08`.
- Client 4016 `Client-3D-2/update/GameAssembly.dll`, SHA256 `011357720B210AA1F392A8B62FF8DE090D3A895230A6E0135BCE707196871069`.
- Historical local test-harness event trace `Client-3D-2/LAN-LOCAL/M1-map53-region.json`. Its embedded original binary hash matches the DLL. It is **not** a player-runtime coordinate capture or proof Map53 rendered successfully.
- Existing static native function evidence in `reports/evidence/cn02-4016-il2cpp/worldsync-contract.json`.

No game executable launched, modified or patched. Unity Editor was already running and left untouched. No UnityProject, reference asset, Bishop, GameServer, DB, character or scene was modified.

## Identity of native OnSyncWorld argument

IL2CPP v39 MethodDefinition, ParamDefinition and TypeDefinition tables confirm:

- `ProcessGameUnity.OnSyncWorld(WORLD_SYNC data)` — MethodDef token `0x060038A9`; parameter type registry index `52052`.
- `MapService.UpdateWorldInfo(WORLD_SYNC data)` — MethodDef token `0x060013B0`; parameter type registry index `52052`.
- `Network.Header.WORLD_SYNC` — TypeDefinition index `13018`, byval type registry index `52052`, exact match to **both** methods.

Original `WORLD_SYNC` FieldDefinition names and order (seven fields) are:

| Order | Name | Field type registry index |
|---:|---|---:|
| 0 | `ProtocolType` | 33519 |
| 1 | `SubWorld` | 40360 |
| 2 | `Region` | 40360 |
| 3 | `Weather` | 33519 |
| 4 | `Frame` | 50516 |
| 5 | `MapCopyID` | 40360 |
| 6 | `szName` | 29914 |

The three selected map fields `SubWorld`, `Region`, `MapCopyID` share a type-registry index and are each handled as 32-bit quantities in the observed native call. The exact signedness of the underlying registry type and the sizes/offsets of the other fields have not been independently resolved.

## Original native map-loader parameter names and bindings

The original metadata names the five formal parameters of `GameWorld.SubWorld.LoadMap`, MethodDef `0x060039F9`:

`nTemplateId, nMapId, nRegion, nMpsX, nMpsY`.

Disassembly of `MapService.UpdateWorldInfo` at callsite RVA `0x8E1094` and the source `WORLD_SYNC` field order produces the following named bindings:

| Original WORLD_SYNC field | Observed input byte offset | Native transfer | LoadMap formal parameter |
|---|---:|---|---|
| `MapCopyID` | `+0x14` | `edx` | `nTemplateId` |
| `SubWorld` | `+0x04` | `r8d` | `nMapId` |
| `Region` | `+0x08` | `r9d` | `nRegion` |
| No corresponding record source | N/A | zeroed `esi` to stack `+0x20` | `nMpsX = 0` |
| No corresponding record source | N/A | zeroed `esi` to stack `+0x28` | `nMpsY = 0` |

Formal method signatures come from v39 metadata, and native transfer offsets/registers come from bounded PE64 disassembly. The association of named fields to offsets is supported by field order and original native input use; unrelated fields' full packed layout and on-wire frame boundaries are not asserted.

`MapService.UpdateWorldInfo` subsequently calls `SubWorld.Activate` at RVA `0x8E10E8`, and the same function resets NPC/path state. It does not supply verified new Map53 affine matrix values.

## Specific historical Map53 example (NOT independent live acceptance)

The existing `M1-map53-region.json` emitted two identical `receivedWorldMap` events:

```text
SubWorld = 53
MapCopyID = 53
Region = 6488114
```

Under the **verified client method and field mappings**, those logged values imply the following arguments would be passed on the corresponding code path:

```text
SubWorld.LoadMap(
    nTemplateId = 53,
    nMapId      = 53,
    nRegion     = 6488114,
    nMpsX       = 0,
    nMpsY       = 0
)
```

These values are grounded in the historical test harness, not sampled actual Unity world positions. The trace cannot prove `SubWorld.LoadMap` completed, the original scene rendered, the actor spawned, or that the six default `MapService` affine constants were active on Map53.

## Tests, evidence and reproducibility

New Python source: `tools/reference4016/il2cpp/worldsync_field_types.py`. It fails closed if metadata type identity, field order, original parameter names, native transfer pattern, historical event log or SHA fingerprint disagree.

Verifier: `tools/reference4016/il2cpp/verify_worldsync_field_types.py`; final **19/19 new static provenance/semantic assertions PASS**, exit 0. Four existing native evidence suites were rerun (14/14, 11/11, 10/10, 10/10): **64/64 static regression assertions PASS** across all five suites, process exit 0.

Output JSON: `reports/evidence/cn02-4016-il2cpp/worldsync-field-types.json`. No binary payload extracted or committed.

Reproduce from `Client-New`:

```powershell
python tools/reference4016/il2cpp/worldsync_field_types.py --metadata "E:\Games\VLTK3D\SwordOnline\Client-3D-2\update\Game_Data\il2cpp_data\Metadata\global-metadata.dat" --worldsync-evidence reports/evidence/cn02-4016-il2cpp/worldsync-contract.json --map53-log "E:\Games\VLTK3D\SwordOnline\Client-3D-2\LAN-LOCAL\M1-map53-region.json" --output reports/evidence/cn02-4016-il2cpp/worldsync-field-types.json
python tools/reference4016/il2cpp/verify_worldsync_field_types.py
```

**Next engineering target:** recover exactly how `SubWorld.LoadMap` uses `nTemplateId = MapCopyID`, `nMapId = SubWorld` and `nRegion = Region` to select scene configuration or map content. After that, a separate authorized **Map53 original-runtime evidence capture** and four corresponding 3+1 landmarks are needed to validate the transformation and allow changes to Client-New Unity runtime. Keep CN03 closed until that gate.
