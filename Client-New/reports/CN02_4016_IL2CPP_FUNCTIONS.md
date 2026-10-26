# CN02-04 — 4016 coordinate metadata and native analysis

Date: 2026-10-25 UTC+07. Branch: cg/cn02-anchor-evidence. Prior baseline 7ac67a3.
**Result:** Target native functions found; **CN02-04 remains BLOCKED / NOT VERIFIED**.

## Safety and sources

Only local files were read. Source metadata: global-metadata.dat SHA256 5586F416AA26EBE57EA7F6580D0A53E5E5F29C50E425E64A4D3F2A95D6069E08. Source PE64: GameAssembly.dll SHA256 011357720B210AA1F392A8B62FF8DE090D3A895230A6E0135BCE707196871069. No reference client execution/patch, no Unity project or server changes; no game assets imported.

## Recovered metadata and addresses

Metadata version 39, 31 sections, 21,485 types, 161,267 methods, 122 image definitions. All 16 selected coordinate-related methods belong to Assembly-CSharp.dll.

One original Assembly-CSharp CodeGenModule candidate is at RVA 0x4BEFCF0 with 19,668 method-pointer slots starting RVA 0x5CCDDD0. Bounded native RVA candidates:
- MapService.get_MpsWorldScale: 0x8E2140
- MapService.Mps2WorldPos: 0x8DFD10
- MapService.Map2WorldPos: 0x8DFBF0
- MapService.WorldPos2Mps: 0x8E18B0
- MapConverter.MapToWorldPos: 0xA1FBB0
- MapConverter.MpsToMapPos: 0xA1FC00
- GameWorld.SubWorld.Map2Mps overloads: 0xC790F0, 0xC790B0
- GameWorld.SubWorld.Mps2Map: 0xC79540

MapConverter.MapToWorldPos normalizes two horizontal input components by two denominators, multiplies by two extents and sets world Y to zero. Another Map2Mps overload uses 5-bit and 10-bit shifts. Mps2WorldPos references the single-precision number 1/512; WorldPos2Mps references 512. These are confirmed bounded native instruction/data observations, **not a validated Map53 affine transform**. Map origin, scene correspondence, call parameter meaning and terrain sampling are unresolved.

## Registration and limitations

A contiguous array at RVA 0x5CB5008 contains 121 readable CodeGenModule records. Compiler order is different from metadata image order, and one metadata image (AmazingAssets.TerrainToMesh.dll) was not observed in the array. Unique global CodeRegistration count-and-pointer was not confirmed: code-registration-check.json reports REGISTRATION_UNVERIFIED. The method RVA links therefore remain *static candidates*, not independently validated runtime addresses.

## Reproducibility and verification

Scripts: tools/reference4016/il2cpp/**.
Evidence: reports/evidence/cn02-4016-il2cpp/**.
Python pefile/capstone installed only under ignored .tmp/reference4016-python-deps.
Final 10/10 native metadata/coordinate regression tests PASS: python tools/reference4016/il2cpp/verify_native_coordinate_findings.py. Two test-assertion mistakes in the initial run were corrected; no reference content was changed. No Unity Editor/test/build was run this pass.

## Next bounded step

Complete static callgraph and branch review of MapService conversion methods, including the source of origin/size fields and parameter roles. Independently validate method addresses and establish 3+1 measured same-scene legacy/4016 landmarks before introducing coefficients into Client-New. Do not treat four synthetic or tile-origin markers as verified calibration. Keep CN03 closed.
