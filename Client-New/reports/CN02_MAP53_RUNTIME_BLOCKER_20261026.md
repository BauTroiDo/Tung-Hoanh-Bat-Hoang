# CN02 Map53 — Live defect: avatar/HUD only, world geometry and NPCs missing

**Date:** 2026-10-26, observed user screenshot ~07:50 local time, live diagnosis ~07:55. **Client:** original 4016, `Game.exe` PID 19552; original binary and metadata left unchanged. **Server:** Bishop PID3212, gs001 PID7444, QA native gateway PID7600 with allowlist 192.168.1.13; localhost Addressables mirror PID13100.

## User-visible defect (real evidence)

Screenshot provided by operator shows native world HUD/character and nameplate visible, but mostly a blue empty background, no Ba Lang Huyen terrain/buildings/NPCs, loading circle in top-right map area (`Ba Lăng huyện / PC 35`) and apparently immobile character. This is **NOT** original Map53 loaded, even though the HUD names the map.

## Timeline checked directly from gateway

**Fresh same-session trace at 07:48:51–07:48:53** (additional similar sessions at 07:50:53, 07:52:50, 07:54:50):
- Bishop authentication PASS, three-character roster returned, actual role selection verified.
- **Bishop GameServer permit SENT**: native 61 bytes; prior 05:59 role permit issue has been surpassed.
- Native GS login authorized, original GS connection opened and received initial sync (2 batches, 1595 bytes in first session).
- Native initial conversion sent map=53, entity numeric 40707, only five initial packets; **omitted** legacy opcodes `69,118,161,164,165,166,167,214` (later initial records omit 161).
- Original GS sync ACK sent once. One movement command forwarded; original GS position updates observed (e.g. x=51889, y=102110→102120).
- Client sent a 5-byte NPC/AOI request; current gateway **explicitly logs `M1 native NPC request omitted`**. Native chat subscriptions also intentionally omitted.
- Server continually sends optional legacy opcodes 75 (302-byte) and 80 (6-byte), but the native bridge logs **`M1 legacy feature omitted`** for them. Do not assume their exact function/meaning from numeric opcodes alone; map schema before implementing.
- Session has disconnected with `Native M1 session traffic limit` (8 MiB/4096 frames), and once `Unsupported native M1 command`. This causes reconnects; do not extend limits or silence validation merely to appear stable.

**Key distinction:** GS sent a position response, but no controlled post-scene-ready user click→GS authoritative move→Unity echo was captured. The reported "cannot move" remains an acceptance failure.

## Map and Addressables loader evidence

- Existing read-only original-client probes from Oct 25 (historical): `WORLD_SYNC(SubWorld=53, MapCopyID=53, Region=6488114)`, `SubWorld.LoadMap(selector=53, mapId=53)`, `originalMapRequest(53)`, sometimes `originalMapLoadAttempt(mapId=53, current=53)`, `originalAvatarCreationReturn(creationPending=true)`. These do not prove success, and belong to earlier sessions.
- Fresh 2026-10-26 mirror log (file `Tool/_outputs/cn02-bulk/map53-qa09/mirror-requests.jsonl`) contains only `catalog_1.0.hash` 200 requests. **No Ba Lang Huyen assets/scenes bundle GET in current observed period.** Two original PC bundles are present and cryptographically verified offline, but the client has **not requested them from the mirror**; possible reasons include unresolved Addressables scene key, no load progression, a different resource path, or another cache/path state. Do NOT claim a specific root cause until live scene/asset APIs are observed.
- `Game.exe` live TCP connections to gateway established, but no evidence of remote CDN traffic at snapshot. Local mirror listening on 14186. Original GameAssembly, Game.exe and metadata hashes matched checkpoint.
- 12-second bounded **numeric-only Frida observer** attached to PID19552, using six previously validated RVAs (WORLD_SYNC, LoadMap, MapRequest, MapLoadAttempt, NPC_SYNC, LocalPlayer), then unloaded/detached. It returned installed=true and **no new event** within this window, so it offers no new positive map/asset proof. Script and JSON outside Git: `Tool/_outputs/cn02-bulk/map53-resume-20261026-0745/trace_readonly.py`, `readonly-trace.json`. No app methods, packet send or file edits in the observer.

## Prioritized repair path / acceptance gates

**P0-A — Real scene/Addressables progression (first):**
1. With a controlled single QA character and no automatic repeated login, attach bounded read-only instrumentation to client scene-loading state; record exact scene key, catalog locator, loaded scene name, map manager load task state, reported failure/callback, and actual GET URLs (if any).
2. Compare exact Unity 4016 catalog keys/dependencies and official hashed PC BaLangHuyen UnityFS bundles; do not fabricate or swap file formats. Determine why none of the two verified bundles were fetched. Scene must show real geometry/collider/NPC placeholders as appropriate; avatar must settle on verified terrain.
3. On a test copy, fix only the proven resolver/loading defect, run regression, then deploy to DEV in a controlled clean client session.

**P0-B — NPC/AOI compatibility:** The 5-byte native NPC request handler is a deliberate omission in `gs-bridge-m1/native_session.py`. Implement proper translated native request↔legacy GS response and server-authored NPC_SYNC/AOI records **after** verifying the exact protocols, entity IDs, ordering, map/position and bounds; the bridge must not fake NPC actors or forward raw legacy bytes. Build unit + integration tests for presence, despawn, reconnect.

**P0-C — Movement & reconnect stability:** Existing bridge accepts limited movement and passes GS positions, but client-side controller/scene synchronization isn't accepted. Capture one actual user click→native intent→original GS response→client echo **after scene-ready**. Separately diagnose repeated 75/80 optional packet traffic, 4096-frame cap, and unsupported command disconnect; add verified, bounded policy/tests, not a blanket limit increase.

**DO NOT:** restart newly repaired Bishop/gs001 without specific need, bypass Bishop authorization, change DB, falsify a screenshot or GS ACK, overwrite original client binaries, or mark CN02-04 PASS based only on login + UI.

**Status:** Bishop permit and GS initial sync **OBSERVED PASS**; original BaLangHuyen scene render **FAIL**; NPC/AOI **NOT IMPLEMENTED in current native bridge**; movement **NOT VERIFIED / user reports unavailable**; affine 6 + 3+1 real landmarks **NOT VERIFIED**; CN02-04 **BLOCKED**, CN03 **CLOSED**.
