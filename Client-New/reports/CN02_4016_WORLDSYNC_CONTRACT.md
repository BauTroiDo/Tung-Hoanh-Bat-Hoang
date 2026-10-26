# CN02-04 — Original 4016 world-sync input and SubWorld loader contract

**Date:** 2026-10-25 (UTC+07). **Branch:** `cg/cn02-anchor-evidence`; baseline `b1ebdbf`.
**Status:** SubWorld loader parameter *byte offsets* recovered statically; Map53 live matrix and independent 3+1 anchor pairs remain unverified.

## Reference inputs and protections

Read-only: original client 4016 `Client-3D-2/update/GameAssembly.dll` (SHA256 `011357720B210AA1F392A8B62FF8DE090D3A895230A6E0135BCE707196871069`), and `Game_Data/il2cpp_data/Metadata/global-metadata.dat` (SHA256 `5586F416AA26EBE57EA7F6580D0A53E5E5F29C50E425E64A4D3F2A95D6069E08`). Binary fingerprints are verified before analysis. No original game execution, patching, data import, UnityProject modifications or GameServer/DB changes.

## Verified original native chain

```text
ProcessGameUnity.OnSyncWorld          RVA 0xC38990
    CALL at 0xC38D1F
MapService.UpdateWorldInfo            RVA 0x8E0DD0
    CALL at 0x8E1094
SubWorld.LoadMap                      RVA 0xC75DE0
    [world state processing]
MapService.UpdateWorldInfo
    CALL at 0x8E10E8
SubWorld.Activate                     RVA 0xC753C0
```

Associated calls identified in the same real methods include `GameServerNetwork.NotifyExchangeWorldSynced`, `ToastManager.ChangeWorld`, `NPCInfoService.ResetServerPosTracker`, `GameManager.ForceCancelPathFinding`, `JXCharacter.EndAwaitRevivePosition`, `NPCInfoService.UpdatePlayerSync`.

**OnSyncWorld argument provenance:** entry at `0xC389A3` moves the native second argument (`rdx`) into `rdi`. The method reads the input record at `[rdi]` and `[rdi+0x10]` (two separate 16-byte windows). Immediately before the `UpdateWorldInfo` call, 32 bytes are copied into a stack-local buffer, `rcx` is set to its address (`rsp+0x40`), and `edx` is cleared to 0. This demonstrates a fixed-width input copy and passing-by-pointer **in the observed compiled path**, not a formally identified wire protocol message type or total network frame length.

## Three fields passed to SubWorld.LoadMap

In `MapService.UpdateWorldInfo`, the local input record pointer is kept in `rbx`; at RVA `0x8E1054` and `0x8E1057` both 16-byte halves are loaded. Direct native instructions at RVA `0x8E106B..0x8E1094` then pass the following 32-bit values:

| Loader argument register | Source within copied world-sync record | Evidence |
|---|---:|---|
| `r8d` | DWORD at offset `+0x04` | `movq r8,xmm0` from input+0; `shr r8,0x20` |
| `r9d` | DWORD at offset `+0x08` | `psrldq xmm1,8`; `movd r9d,xmm1` |
| `edx` | DWORD at offset `+0x14` | `movq rdx,xmm2` from input+0x10; `shr rdx,0x20` |

`rcx` is a previously resolved `SubWorld` instance pointer. The stack also provides zero-valued extra arguments. `SubWorld.LoadMap` entry preserves `edx` in `edi`, `r8d` in `r14d` and `r9d` in `ebp`.

**Important:** these *byte offsets and register transfers* are established; the fields' business names, signedness, coordinate units, map ID mapping and Map53 semantics are **not** independently confirmed. Do not label `+0x04` as `mapId` or `+0x14` as `region` without additional metadata / original packet bytes.

**Activation:** after the direct `LoadMap` call, `UpdateWorldInfo` resolves a SubWorld pointer and calls `SubWorld.Activate` with `edx=0`. The PE64 exception entry covering `SubWorld.Activate`'s entry is only 25 bytes / 8 decoded instructions; it does **not** cover the entire possible control-flow continuation. No claim has been made about its complete body.

## Static analysis limits

- A `.pdata`-scoped native scan proves exact direct calls and offsets, but cannot show a scene was successfully rendered, networking completed, or the player moved.
- No live value snapshot for `m00,m01,m10,m11,offsetX,offsetZ`. The six previously recovered `.cctor` values are class defaults, not verified active Map53 coefficients. `Calibrate` may be invoked indirectly or other writers may exist.
- Existing Map53 log contains two *unpaired* actor coordinate events. It does not include four matched original 4016 Unity world positions.
- Original scene tiles and synthetic Client-New marker coordinates must not be treated as calibration anchors.
- Source or usage rights of original 4016 scene assets remain unverified; none imported.

## Reproduction and tests

Scripts:
- `tools/reference4016/il2cpp/worldsync_contract.py` — checks SHA, resolves Assembly-CSharp-scoped CodeGenModule pointers, disassembles bounded unwind ranges, records the exact callsite instruction context. Pointer and exception arrays are each read from the PE once (avoids slow repeated requests).
- `tools/reference4016/il2cpp/verify_worldsync_contract.py` — 14 regression assertions for original method ownership, call links, packet argument offsets and non-acceptance gates.

Evidence: `reports/evidence/cn02-4016-il2cpp/worldsync-contract.json`.

The first exploratory Python scanner was stopped after identifying slow repeated PE table reads; the optimized read-only version produced evidence and **14/14 PASS** new tests (exit 0). The earlier MapService writer, dispatch and default-field regressions were also rerun: **11/11 + 10/10 + 10/10 PASS**, totaling **45/45 static tests PASS**. This did not require modifying binary input or an existing run of the original client.

Commands from Client-New working directory:

```powershell
python tools/reference4016/il2cpp/worldsync_contract.py --binary "E:\Games\VLTK3D\SwordOnline\Client-3D-2\update\GameAssembly.dll" --metadata "E:\Games\VLTK3D\SwordOnline\Client-3D-2\update\Game_Data\il2cpp_data\Metadata\global-metadata.dat" --module reports/evidence/cn02-4016-il2cpp/pe-module-candidates.json --writer-audit reports/evidence/cn02-4016-il2cpp/map-writer-xrefs.json --out reports/evidence/cn02-4016-il2cpp/worldsync-contract.json
python tools/reference4016/il2cpp/verify_worldsync_contract.py
```

## Exact next engineering step

Recover the original packet/subworld field definitions (type/sign/meaning for DWORD offsets +4,+8,+0x14) and trace map lifecycle fields to their scene selectors. Independently acquire 3 noncollinear original-scene corresponding legacy and Unity anchor pairs and a fourth holdout; then validate whether the original default affine coefficients are active on Map53. Only after evidence and review, propose runtime mapping for Client-New. **CN02-04 BLOCKED; CN03 CLOSED.**
