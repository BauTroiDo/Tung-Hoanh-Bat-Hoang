# CN02-04 — Client 4016 MapService writer and world-sync cross references

Date: 2026-10-25 UTC+07. Prior commit 5f5280b. Branch cg/cn02-anchor-evidence.

**Result:** Confirmed direct ProcessGameUnity.OnSyncWorld -> MapService.UpdateWorldInfo relationship from original IL2CPP and PE64 exception records. Original reference binaries read-only, unchanged; UnityProject/server/database untouched.

## Confirmed call chain

- Caller: ProcessGameUnity.OnSyncWorld, entry RVA 0xC38990.
- Instruction-boundary-confirmed CALL at RVA 0xC38D1F.
- Callee: MapService.UpdateWorldInfo, entry RVA 0x8E0DD0.

Verified direct transfer counts across executable PE sections:

| Method | Confirmed direct transfers |
|---|---:|
| Calibrate | 0 |
| Mps2WorldPos | 74 |
| WorldPos2Mps | 109 |
| Map2WorldPos | 19 |
| UpdateWorldInfo | 1 |
| Solve3x3 | 2 |

These counts cannot exclude reflection, delegates, virtual or indirect calls. They do not prove which coefficient values are active in Map53.

## Static matrix writer audit

- 19,668 original codegen method slots; 16,619 distinct nonzero code pointers; 40 MapService method bodies inspected.
- Original MapService .cctor initializes m00, m01, m10, m11, offsetX, offsetZ. Calibrate passes destination pointers for these fields into Solve3x3 twice.
- UpdateWorldInfo has a store into [rdi+0x10] at RVA 0x8E0ED9. Matching offset alone is **not** proof that this is MapService.offsetX: the base register references another world object. No actual Map53 replacement of the six fields was verified.
- A first experimental scanner incorrectly mixed MethodDef token indices across assemblies. Corrected scanner restricts owner types to image 0 Assembly-CSharp.dll (type indices 0..2668) and validates Calibrate token row 0x13B8, Mps2WorldPos row 0x139D. The first output was discarded.
- Two initial tests used numerical inventory thresholds above observed counts; bounds were corrected and all final tests passed.

## Evidence and limits

SHA256 original GameAssembly.dll: 011357720B210AA1F392A8B62FF8DE090D3A895230A6E0135BCE707196871069. Metadata: 5586F416AA26EBE57EA7F6580D0A53E5E5F29C50E425E64A4D3F2A95D6069E08.

Source: tools/reference4016/il2cpp/map_writer_xrefs.py and verify_map_writer_xrefs.py.
Evidence: reports/evidence/cn02-4016-il2cpp/map-writer-xrefs.json.

Executed original-source static scanner and three test suites: 11/11 new xref regression, 10/10 dispatch regression, 10/10 writer regression, all PASS, processes exit 0. No fresh Unity build/test: UnityProject source did not change. No live gameplay or original-client execution.

Next: trace OnSyncWorld input fields and UpdateWorldInfo callees to identify Map53 runtime coefficient provider, then require three independent noncollinear paired landmarks and one holdout in the same original scene. Do not use synthetic markers or conditional NPC projections as pairs.

**CN02-04 BLOCKED / NOT VERIFIED; CN02 PARTIAL; CN03 CLOSED.**
