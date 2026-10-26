# JX1 3D SwordOnline — Lệnh giai đoạn gameplay sau Map53 collision pilot
**Ngày:** 2026-10-26 · **Người yêu cầu:** tiếp tục hoàn thiện các chức năng gameplay khác vì người chơi đã thử thấy đi không bị giật ngược tại Ba Lăng huyện.
**Môi trường:** DEV / Client4016 tham chiếu / Client-New Unity source, GS Win64 + native gateway.
**Thứ tự:** NPC/AOI → tương tác/hội thoại → nhiệm vụ → combat/skills → items & persistence → mở rộng map.
**Trạng thái di chuyển:** User visual test: NO SNAPBACK on recent Map53 route. GS reboot integrity 1496/1496 passed; old corridor 0/55 blocked; 3,310 false-clear and 5,538 false-blocked sampled 3D/GS mismatches remain. DO NOT equate this with full-map/global collision parity. CN02-04 synthetic anchors remain BLOCKED, CN03 networking remains CLOSED unless independently approved.

## Source of truth (đã kiểm tra)
- DEV server: `C:\Server-Game\SwordOnline\ServerNew\_bin_v2_\gs\gs001.exe`, current QA was PID5036 on 2026-10-26 around17:38, dynamic PID.
- Gateway: `C:\Server-Game\SwordOnline\Client3D-LAN\gs-bridge-m1\native_session.py`, listener port14180; verify PID before changes.
- Native QA: `E:\Games\VLTK3D\SwordOnline\Client-3D-2\update\Game.exe`; source-controlled replacement: `Client-New\UnityProject`.
- `native_session.py` currently logs `M1 native NPC request omitted` for **5-byte native opcode92**; optional legacy packets including nonlocal NPC data are omitted instead of being converted. GS source `KProtocolProcess.cpp` installs `c2s_requestnpc → NpcRequestCommand` and `NpcSet.SyncNpc(ID, Player[nIndex].m_nNetConnectIdx)`.
- Existing conversion preview `gs-bridge-m1/gs_npc_status_merge.py` checks legacy NPC_SYNC opcode76 + NPC_NORMAL_SYNC opcode77 same authoritative NPC ID, but explicitly `production_ready=False`. `gs_world_map53_name_preview.py::npc_name_preview` gives 105B bounded native NPC struct preview and contains unresolved fields. DO NOT blindly send its raw bytes to live client.
- `native_initial_packets.py` currently builds world, local NPC, player, appearance and normal status only. Nonlocal AOI and object lifecycle unimplemented; no fake actors.
- Linux V9 source/reference `E:\Games\VLTK3D\SwordOnline\Source-KhamKhao\jxser_v9\server1` has gameplay Lua/scripts; Windows port has many functions but some are partial/no-op (e.g. skill counts, inlay, partner, dungeon), separate audit `reports/V9_LINUX_WIN64_FUNCTION_PARITY_AUDIT_20261026.md`.
- User screenshot Map53 plaza shows 3D assets, character and UI; visible HUD buttons do NOT prove skills/quests/inventory/interaction work end-to-end.

## GP-01 — NPC/AOI authoritative world sync (P0, FIRST)
1. Read exact legacy `NPC_REQUEST_COMMAND`, `NPC_SYNC`, `NPC_NORMAL_SYNC`, removal and object packets from **running-build-matched** definitions, and native4016 IL2CPP marshalling/opcodes 92/associated response; include field widths, alignment, lengths, mapId/region, ID and entity ownership. Verify actual native game invokes request and expects response; no opcode relabel guesses.
2. Bounded adapter for authenticated client requests: pre-ACK reject, exact 5B, nonzero id, region/map/session bounds, rate limits; forward only structurally valid request to original GS. Do not synthesize NPCs.
3. Convert GS-originated nonlocal NPC spawn/status/remove/update to native equivalents; preserve **server IDs**, NPC type/camp/level/name/appearance and origin positions, native X=legacy X*2 verified Map53. Handle ordering, duplicate requests, stale ID, despawn, reconnect and map region transitions.
4. Verify with two read-only traces first; isolated test/canary before DEV deployment. Keep PID/session/DB and 4016 original binaries stable. Restore on failure.
5. **Acceptance:** user sees at least one real GS-authored Map53 NPC; second player/client (if used) receives same identity and positions; NPC move/despawn/reconnect do not ghost/freeze; 0 false network disconnect; no regression movement and ping.

## GP-02 — NPC click, dialogue, interaction (P0)
1. Find native click/interact C2S and GS `NpcTalkCommand` mapping, bounds/map/distance checks. Translate server-authored dialogue branches and choices; no fabricated local dialog.
2. Gate interaction by NPC existence/ID/session/range. Failed or unsupported cases show consistent error, do not hang in pending modal state.
3. **Acceptance:** click a named real NPC, server sends exact dialogue, select 2 options, state/quest advance as expected, reopen/relogin repeat successfully.

## GP-03 — quests/task progress + navigation (P1)
- Load existing main/side quest server Lua, quest grants/conditions/rewards, dialogue transitions, navigation MapId/NpcId, completion flags and persistence. Avoid fake task advancement.
- Test beginner quests at Map53, both accepted and rejected conditions, reconnect and map change.
- Do not assume scripts exist when original GS Lua hooks `playerlogin.lua`/`playerlogout.lua` were previously missing / nil; trace actual virtual script mount and resolved callsite instead of writing empty stub.

## GP-04 — skills/combat + death/respawn (P1)
- Map native attack/cast/target IDs, authoritative GS combat state, attack cooldown/speed/range, HP/MP changes, dead/revive/respawn, no arbitrary damage in gateway.
- Audit known Windows V9 API placeholders (`GetSkillCount`, `GetTotalSkill`) and repair against true GS state only when semantic contract verified; no fake success.
- Tests: valid skill, invalid cooldown/out-of-range, NPC attacked, target dead, revive, long session, reconnect with correct HP.

## GP-05 — character UI, bag, equipment, money, saving (P1)
- Client UI should populate from authoritative GS packets: real bag item IDs/amounts, equip/unequip, character attributes, skills, money, target effects; reject malformed packets and inventory duplication.
- Fix stub API such as `InlayStarStone` only with real item mutation, validation, atomic persistence and rollback; no item/gold fabrication.
- Verify actual character database survives logout/login and restart. Avoid real-character destructive tests; use disposable QA role.

## GP-06 — shared expansion (P2)
- Only after Map53 core gameplay E2E, extend other supported maps with versioned map manifest, native/GS coordinate + collision parity tests, AOI, region transitions. NPC and items must remain consistent across map boundaries.
- Full native32x32 vs GS16x32 collision mismatch remains independent global gate. No blanket server barrier disable.

## Mandatory delivery contract for Antigravity / developer
- One active milestone at a time. **Start GP-01 now** with read-only protocol inventory & negative tests. Do not edit collision code or previously reviewed native movement/ping unless a proven regression requires it.
- Worktree/branch isolation; do not reset or stage another agent's WIP. Before modifications: inspect latest HEAD/Git status, live services, backup source and readiness/hash gate.
- For each feature provide exact source files, evidence hashes, protocol table, tests + exit codes, DEV deploy/rollback recipe, real 4016 runtime traces, and explicit passed/blocked items. Keep multi-map scope generic, Map53 pilot.
- Continue only after objective tests; do not claim feature complete from mocked/client offline tests alone.
- Review Linux V9 parity separately, and protect user character DB.
- Record progress in `TASK_BOARD.md`, `docs/CHECKPOINT.md`, `docs/NEXT_SESSION_HANDOFF.md`; create `reports/GP01_NPC_AOI_RUNTIME_ACCEPTANCE.md` when GP-01 verified. Commit and publish to designated GitHub repo on a separate review branch, never overwrite main.
