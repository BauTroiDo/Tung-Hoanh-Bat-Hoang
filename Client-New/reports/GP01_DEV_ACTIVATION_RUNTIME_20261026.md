# GP01 — Controlled DEV Activation, Real GS NPC Snapshot (2026-10-26)

## Result

**GP01 server-side NPC discovery activated and runtime-confirmed; 4016 NPC rendering remains BLOCKED / not accepted.**

During the 19:51–19:53 (Asia/Ho_Chi_Minh) canary on Map53:
- Native4016 reconnect succeeded; authoritative player **entity 40707**, MapId53, genuine native ACK; live gateway logged `GP01 DEV NPC snapshot requested after real ACK entity=40707` at 19:51:32.
- At 19:52:36 gateway logged **23** actual full GS `NPC_SYNC` legacy opcode76 records, with real server IDs; `full_metadata` reached 23. Separately 23 original GS metadata requests were logged.
- 0 `GP01 ignored malformed` warnings, 0 `GS forced position` lines and 0 `session error` lines in the sampled window. This is not exhaustive long-run movement acceptance.
- Gateway socket `14180` accepted an Established TCP connection from 192.168.1.13. GS port51189 and Bishop56722 were listening. Native full 4016 NPC-create frames **were not sent** (`native spawn still gated`) because exact native marshalling + model mapping have not yet passed verification.
- Thus this is evidence for **GS real NPC enumeration → Gateway receipt**, **NOT** NPC render/click/dialogue or entire global NPC parity.

## Active DEV deployment

- Server: `WIN-33R6OP5GO1M`, `C:\Server-Game\SwordOnline\ServerNew\_bin_v2_\gs`.
- `gs001.exe` PID **6120** started with runtime `JX3D_GP01_NPC_SNAPSHOT=1`.
- Installed `CoreServer.dll` SHA256 **5B5FCDE48B9F11888FFF6F02F9E3743A6273945BEC70F03EFF2046BD682C9887**. This is the new candidate built from Stage13P source (`e011493`), with 55 GP01 lines, worktree `C:\Server-Game\SwordOnline\_AI_Worktrees\gp01-npc-stage13p`, commit **b5eec46**.
- **Exact installed pre-change DLL baseline** SHA256 **C4ADAD7B996712A3D489C9C6CA65B9D30963BA07389AE3544114A5464BAC82C7**, matched Stage13P isolated release before patch.
- New CoreServer built release x64 in `C:\Server-Game\SwordOnline\_AI_Builds\gp01-npc-stage13p\gs`. **1291/1291** exports retained, zero removals/ordinal changes; all **130** named `Engine.dll` imports of candidate exist in the installed Engine. Installed `Engine.dll` unchanged (SHA256 `E5D5B79EB2AF871BAB9CF6C5F9AEC0ED89C24A7D4DCB5E63BC75D0285C3B0EDD`).
- Gateway Python PID **1088**, `gateway.py --allow 192.168.1.13 --native-gs-bridge --log-file C:\Server-Game\SwordOnline\Client3D-LAN\qa_runs\gp01-activation-20261026\gateway-gp01.log`.
- Gateway runtime flags: `GP01_NPC_DISCOVERY_ENABLED=1` and `GP01_DEV_NPC_SNAPSHOT=1`. Ordinary movement guard, GS position authority, packet framing and collision remain unchanged.
- The readiness gate initially refused changed `native_session.py` because of a stale hash. Re-ran **178/178 bridge tests PASS** (exit0), confirmed preexisting client DLL/roundtrip/ACK evidence remains anchored, updated only the `native_session.py` source hash in `native-p0-readiness.json`; `evaluate()` returned `CONTROLLED_MAP53_PROTOCOL_TEST_READY`, `world_entry_enabled=true`, `m1_p0_pass=false`. Did **not** bypass or remove readiness checks.
- Backup of original manifest: `qa_runs\gp01-activation-20261026\native-p0-readiness.pre-gp01.json`.

## Safety and rollback

- Explicit user approval for DEV activation. Gateway was stopped first to close the only 4016 session; waited for GS connection release before stopping GS. **No database file was directly modified.** GS disconnect/save was not independently confirmed with a character persistence roundtrip and remains a QA gate.
- Before replacement, copied original `CoreServer.dll/.map/.pdb`, `native_session.py`, `native_npc_aoi.py`, `gateway.py` into:
  `C:\Server-Game\SwordOnline\Client3D-LAN\qa_runs\gp01-activation-20261026`
  with per-file SHA256 recorded. Original Core DLL backup `CoreServer.installed.dll`.
- Rollback: disconnect Gateway safely, verify no active client connection, stop GS; restore all three original CoreServer files from the above backup. Start GS **without** `JX3D_GP01_NPC_SNAPSHOT`; start Gateway without the two GP01 environment flags, retaining current validated native session module and the **updated readiness manifest** to preserve CN02 movement and current protocol hashes. Do not blindly restore `native-p0-readiness.pre-gp01.json` alone: it references a pre-CN02 `native_session.py` hash and would block Gateway startup.
- Character database not overwritten, no NPC IDs forged, no server authoritative collision disabled.

## Remaining GP01 gate

1. Independently verify the exact 4016 native NPC create/update/remove packet layouts from real handler/IL2CPP evidence (native NPC_SYNC opcode98 and native NPC_MIN opcode84 plus required model/appearance fields).
2. Map real legacy GS NPC IDs, kind, coordinates (legacy/native X scale), names and model identifiers to 4016; do **not** substitute template IDs for runtime IDs or use unverified `production_ready=False` preview records in live packets.
3. Guarded runtime QA: at least one authentic GS NPC visible at correct Map53 position, click opens GS-authored dialogue, stable movement/collision, reconnect/despawn and reject tests. Only then GP01 PASS.
