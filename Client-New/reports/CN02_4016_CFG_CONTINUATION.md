# CN02-04 — Client 4016 native coordinate CFG continuation

**Date:** 2026-10-25 (UTC+07)
**Branch / prior HEAD:** `cg/cn02-anchor-evidence` / `83c895c`
**Scope:** Static, read-only examination of the original client 4016 `GameAssembly.dll`; only metadata and our own diagnostics committed. UnityProject, 4016 client binaries, live Bishop/GS/DB and characters unchanged.

## Confirmed findings

Using the previously mapped CodeGenModule candidate and PE64 exception directory (`330,024` unwind function records), inspected the complete 52-instruction `MapService.Mps2WorldPos` body, 84-instruction `MapService.WorldPos2Mps` body, and 62-instruction `MapService.Map2WorldPos` body. Cross-calls in `Map2WorldPos` target native RVA `0xC790F0` (`SubWorld.Map2Mps`) and then `0x8DFD10` (`MapService.Mps2WorldPos`). This independently supports the method-pointer associations, though the complete global CodeRegistration remains unverified.

The actual **forward coordinate formula** compiled into `MapService.Mps2WorldPos` is an affine transform, with *per-map* values read from MapService static field storage:

```text
u = MpsX / 512
v = MpsY / 512
UnityX = a*u + b*v + originX
UnityY = suppliedHeight
UnityZ = c*u + d*v + originZ
```

Field layout as observed from the native `[staticFields + offset]` access:

| Offset | Role inferred from arithmetic |
|---|---|
| `+0x00` | `a` = coefficient of MpsX in UnityX |
| `+0x04` | `b` = coefficient of MpsY in UnityX |
| `+0x08` | `c` = coefficient of MpsX in UnityZ |
| `+0x0C` | `d` = coefficient of MpsY in UnityZ |
| `+0x10` | `originX` |
| `+0x14` | `originZ` |
| `+0x48` | cached effective scale used by `get_MpsWorldScale` |

**Inverse** `WorldPos2Mps` computes the determinant `D = a*d - b*c`, subtracts the origin, then:

```text
MpsX_candidate = 512 * (d*(UnityX-originX) - b*(UnityZ-originZ)) / D
MpsY_candidate = 512 * (a*(UnityZ-originZ) - c*(UnityX-originX)) / D
```

Native inverse uses float32 arithmetic and then invokes the same helper at RVA `0x740CE0` separately on the two results prior to integer conversion. **The helper's rounding/tie/overflow semantics were not independently characterized** in this pass. Therefore these expressions intentionally say `_candidate`, not byte-identical runtime output.

`MapService.get_MpsWorldScale` reads a cache at `+0x48` and, when zero, computes a scale using magnitudes of the transform's two horizontal columns. The observed arithmetic is consistent with `sqrt((sqrt(a*a+c*c)/512) * (sqrt(b*b+d*d)/512))` before storing that cache. This scale getter does not supply the six numerical Map53 coefficients.

The `MapConverter.MapToWorldPos` path uses a different component-wise normalization by caller-provided dimensions; it must **not** be substituted for the dynamic MapService transform.

## Important limitations

1. The six coefficient values `a,b,c,d,originX,originZ` are **runtime-loaded map context**; this static inspection did not find their actual values for Map 53. The Unity 4016 original scene tile coordinates are not equivalent to measured corresponding legacy anchors.
2. Four original terrain tile origins under `PC_BLH_Bake.unity` are **unpaired** with JX1 legacy coordinates. No valid independently observed 3+1 pairs have been produced.
3. CodeRegistration verification remains unresolved: a 121-record CodeGenModule array vs 122 metadata images. No 8-byte absolute pointer to the array was found in the PE, so the old field lookup cannot certify registration; a RIP-relative or indirect initialization path may exist. The candidate method mapping is supported by cross-calls, but do not claim global registration PASS.
4. Full static native CFG was not validated against execution of the reference client; no original assets were extracted/imported into Client-New. No Unity gameplay regression/build was run because UnityProject source was unchanged.
5. Source/license rights of original scene geometry remain unknown; no binary or original asset payload is committed.

## Evidence / repeatable tests

Source:
- `tools/reference4016/il2cpp/cfg_coordinate_analysis.py`: SHA-checked PE64 exception/unwind scoped disassembly; field-offset, constant and direct-call report. When no unwind record exists for a leaf function it caps at 512 bytes and first `ret`.
- `tools/reference4016/il2cpp/verify_cfg_semantics.py`: regression tests of static instructions, call links and synthetic purely mathematical invertibility, **not an acceptance test on Map53**.

Generated report: `reports/evidence/cn02-4016-il2cpp/cfg-semantics.json`.

Actual commands executed on the user's Windows workstation:

```powershell
python tools/reference4016/il2cpp/cfg_coordinate_analysis.py --binary "E:\Games\VLTK3D\SwordOnline\Client-3D-2\update\GameAssembly.dll" --candidates reports/evidence/cn02-4016-il2cpp/native-method-candidates.json --out reports/evidence/cn02-4016-il2cpp/cfg-semantics.json
python tools/reference4016/il2cpp/verify_cfg_semantics.py
```

All executed with exit code 0. The new CFG suite returned **11/11 PASS**; the prior native method suite was independently rerun and returned **10/10 PASS**. Both are static/fixture tests, not Map53 acceptance. The binary fingerprint checked in the new analyzer is `011357720B210AA1F392A8B62FF8DE090D3A895230A6E0135BCE707196871069`.

## Next constrained action

Identify the **writers/data source** for the six MapService fields in actual 4016 Map53 content, independently corroborate the values with 3+1 same-landmark MPS/Unity observations, and review float32 rounding and map bounds. Only then propose a separate Client-New runtime transform candidate and perform Unity ground/collider validation.

**Gate unchanged: CN02-04 BLOCKED / NOT VERIFIED, CN02 PARTIAL, CN03 CLOSED.**
