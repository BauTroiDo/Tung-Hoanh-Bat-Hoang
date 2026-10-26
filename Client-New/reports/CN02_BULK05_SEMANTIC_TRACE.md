# CN02-BULK-05 — Historical login → WORLD_SYNC → map/scene → movement evidence gates

**Date:** 2026-10-26 Asia/Ho_Chi_Minh. **Branch:** `cg/cn02-anchor-evidence`, prior checkpoint `bcac0ed`.
**Status:** **STATIC NATIVE CONTRACT PASS / HISTORICAL TRACE CORRELATION PASS / LIVE END-TO-END NOT VERIFIED.** CN02-04 **BLOCKED**, CN03 **CLOSED**.

## 1. Data provenance: DO NOT confuse separate sessions

Read-only inputs:
- Original client 4016 `Client-3D-2/update/GameAssembly.dll` SHA256 `011357720B210AA1F392A8B62FF8DE090D3A895230A6E0135BCE707196871069`; matching metadata SHA256 `5586F416AA26EBE57EA7F6580D0A53E5E5F29C50E425E64A4D3F2A95D6069E08`.
- Prior native disassembly evidence `worldsync-contract.json`, `worldsync-field-types.json`, `loadmap-selector.json` and `scene-pack-resolver.json`.
- Method/RVA index `Tool/_outputs/cn02-bulk/method-index.sqlite` read-only.
- Historical instrumented event logs `Client-3D-2/LAN-LOCAL/M1-map53-region-events.jsonl`, `M1-map53-fixed-events.jsonl`, and historical localhost mirror log `M1-map53-http-events.jsonl`.
- Distinct handoff documents dated 2026-10-24 and 2026-10-25: `M1-ACCOUNT-CHECK-AND-GS-HANDSHAKE-20261024.md`, `M1-P0-CONTINUATION-20261025.md`, `M1-MAP53-ASSET-RECOVERY-20261024.md`.

**Critical:** Bishop success and GameServer ACK come from historical *reports* about different sessions; historical `receivedWorldMap` events are instrumented application events, **not raw authenticated frames captured at the same time**. Therefore these cannot be concatenated into one verified end-to-end live session.

## 2. Evidence levels by gate

| Gate | Evidence | Result |
|---|---|---|
| Bishop authorization | 2026-10-24 retrospective QA documents successfully authenticated Bishop / three roles | **Documented historically**, no new auth performed |
| GameServer handoff/sync/ACK | 2026-10-25 QA report of original GS initial sync and one backend ACK | **Documented historically**, separate event/session |
| `WORLD_SYNC` | 19 instrumented `receivedWorldMap` plus 19 `subworldMapSelector` | **Historical observation**; `SubWorld=53, MapCopyID=53, Region=6488114` |
| `MapData/LoadMap` | 19 `originalMapRequest`, 10 `originalMapLoadAttempt`; 19 `originalAvatarCreationReturn` remain `creationPending=true` | **Attempt only**; no visible completed avatar proven |
| PC assets / scene | Historical localhost HTTP and multiple scene retries | **Scene ready/render NOT VERIFIED** |
| `WalkState` and authoritative movement | `WalkState.Enter` static RVA; historical 7 `receivedNpcPosition` events | **Real GS-authoritative move + Unity echo NOT VERIFIED** |
| Original Map53 affine | Default coefficient candidates exist, but no captured current matrix and matched 3+1 anchors | **BLOCKED** |

## 3. A newly clarified Map53 HTTP finding

The historical localhost HTTP event file contains **2,433 well-formed GET records**, **2 malformed lines**; statuses:
- **2,421 HTTP 404**
- **7 HTTP 200**
- **5 HTTP 206**

Critically, there are exactly **two historical `pc_balanghuyen` requests; both returned HTTP 200** with response lengths **9,738,990** and **31,592,781** bytes. No raw asset paths/URLs are copied into Git. This **narrows the diagnosis**: the massive 404 count cannot be used to claim those two PC Ba Lăng Huyện files were unavailable *in this historical log*. An HTTP 200 and reported byte length do NOT establish UnityFS hash/CRC validation, successful asset deserialize, the correct scene key, successful map render or current availability. Older 2026-10-24 report had found both missing; that status is a *prior snapshot* and must not override later HTTP evidence. Reproduce any current asset issues with a fresh capture.

## 4. Grounded native call graph (static, not runtime execution)

Verified callsites from original PE64 and metadata:

```text
ProcessGameUnity.OnSyncWorld      RVA 0xC38990
  at 0xC38D1F → MapService.UpdateWorldInfo       RVA 0x8E0DD0
    at 0x8E1094 → SubWorld.LoadMap                RVA 0xC75DE0
    at 0x8E10E8 → SubWorld.Activate               RVA 0xC753C0

SubWorld.LoadMap
  at 0xC762AC → MapDataService.LoadForTemplate   RVA 0x8C9BF0
  at 0xC7638A → GameManager.LoadScene            RVA 0x918B70

MapDataService.LoadForTemplate
  at 0x8C9F59 → MapDataService.ResolveMapFolderPath RVA 0x8CD240
```

All **6/6 direct callsite edges** checked against stored original native evidence. These indicate code-path *possibility*, not that those branches succeeded or completed in the historical UI. The subsequent async `SceneLoadService` / Addressables work must be observed or correlated with actual request IDs; do not invent a continuous dynamic stack across disjoint traces.

`WORLD_SYNC` named type fields and `SubWorld.LoadMap` formal parameters independently verified by IL2CPP metadata:
- `MapCopyID` at source record offset +0x14 → `edx` → `nTemplateId`.
- `SubWorld` offset +0x04 → `r8d` → `nMapId`.
- `Region` offset +0x08 → `r9d` → `nRegion`.
- `nMpsX` and `nMpsY` are zeroed in the observed native code path.

Projection of **instrumented historical values**, **NOT wire capture**, gives `LoadMap(53,53,6488114,0,0)`. 7/7 known static method-signature/RVA anchors matched in the SQLite index: `BishopNetwork.Login`, `GameServerNetwork.ConnectToServer`, `ProcessGameUnity.OnSyncWorld`, `MapDataService.LoadForTemplate`, `SceneLoadService.ResolveVariantKeyAsync`, `WalkState.Enter`, `StateMachine.ChangeState`.

## 5. User-provided IDA 7.3

User explicitly confirmed **`E:\Hack Game\Cong-cu\IDA 7.3` works for their normal workflow** and authorized using it. Treat it as an available manual disassembler without revisiting licensing concerns. No `Patched` copy was used.

A bounded IDA **headless** smoke was attempted on a standalone **130,560-byte Rust-generated PE**, *not the game*: `idat64.exe -A -S... -o...`; process exit **1 after ~1.19 sec**, message `plugins/python64.dll can't load file`. Static import inspection shows this plugin requires `python27.dll`; dependency was not resolved within the environment of that specific batch process. This does **not** demonstrate that GUI IDA is broken. We did not install, patch, replace or overwrite any IDA files. Read-only fallback is official Ghidra 12.2, already validated against the client.

Evidence stored outside Git in `Tool/_outputs/cn02-bulk/semantic-trace/ida73-smoke-run.json`, test script and synthetic input in %TEMP%. For future unattended batch use, validate an authorized Python 2.7 x64 runtime/path in a separate controlled setup, without installing arbitrary cracked DLLs.

## 6. Repeatable correlation and regression

Tracked scripts:
- `tools/reference4016/build_semantic_trace.py` reads only original hashes, static native evidence, historical QA JSONL and selected report assertions, emits **sanitized counts, field mapping, gate status and source classifications**.
- `tools/reference4016/verify_semantic_trace.py` covers stages, native callsite edges, map field offsets, original SHA, historical HTTP counts and PC 200 results, IDA headless smoke limitation, no leaked secrets/bodies and **explicit non-acceptance**.

Summary `reports/evidence/cn02-4016-il2cpp/semantic-trace-summary.json` contains no raw captured credential/role names/GUID/tickets, secret material, live packet bytes or reconstructed method bodies.

Commands from `Client-New`:

```powershell
python tools/reference4016/build_semantic_trace.py --out reports/evidence/cn02-4016-il2cpp/semantic-trace-summary.json
python tools/reference4016/verify_semantic_trace.py
```

29/29 new tests PASS; previous CN02 suites 190/190 PASS when rerun; **219/219 total static tests**. These tests validate the preserved *evidence model*, not working game login/map/movement.

## 7. Next acceptance gate (CN02-BULK-06 / CN02-04)

Before marking an end-to-end pass, capture **one pseudonymous session ID with matching actor identity**, *no credentials/tickets/raw sensitive wire frames*:
1. Bishop permit and GS handshake accepted, same issued session actor;
2. GS-authoritative world sync `MapCopyID=53, SubWorld=53, Region`, with source and timestamp;
3. Unity `LoadMap` and exact **catalog-matched PC bundle/scene identity**, verified asset integrity;
4. Unity scene ready, visible same player (and screenshot/object evidence);
5. One movement acknowledged by **original GameServer**, position echoed/visible in client; explicit pass/fail with time-order and actor invariants;
6. Six active Map53 affine coefficients and 3 paired non-collinear **same-scene original MPS↔Unity** anchors plus an independent fourth holdout.

Do NOT restart `Bishop` or `gs001`, change gateway flags, select any real player or launch client without coordination. No code derived from native pseudo has been merged into UnityProject. Existing 8 Unity material modifications belong to another lane and must remain untouched.
