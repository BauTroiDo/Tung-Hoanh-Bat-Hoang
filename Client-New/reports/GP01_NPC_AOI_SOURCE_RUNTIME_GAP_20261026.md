# GP01-02 — Static NPC vs live GS AOI gap, 2026-10-26
**Status:** READ-ONLY SOURCE AUDIT PASS; LIVE NPC PACKET GAP; NPC UI/interaction NOT ACCEPTED.

## Verified facts
- Current DEV: WIN-33R6OP5GO1M, gs001.exe PID5036, native gateway python PID3828 (dynamic PIDs); Client4016 Game.exe PID21188.
- Map53 loose authoritative data currently under `maps/country/balanghuyen/v_{ry}/{rx}_region_s.dat`. File-format `KRegion::LoadServerNpc` reads the third combined-file section after obstacles and trap; NPC head begins with uint32 count.
- Standalone, read-only audit parsed **1496/1496** region files, **0 errors**; **347** region files contain at least one static NPC record, **845** total static NPC definitions; **1149** files have empty NPC sections. Near problematic R(98,102), this specific region has **one static NPC definition**, and 13 nearby regions in ±2-region radius have at least one definition.
- This proves disk content, **NOT** actual live NPC spawned via `NpcSet.AddMapNpc`. GS setting `settings/gamesetting_gs001.ini` has **NpcActivation=1**. In source `KRegion::LoadServerNpc` with NPC activation enabled, essential NPCs may prewarm while other NPCs are deferred until `KRegion::UpdateActiveType → ActivateRegionNpcs`; these calls still require live proof.
- `KNpc::NormalSync` broadcasts actual `NPC_NORMAL_SYNC (legacy77)` to own and adjacent regions when NPC is alive, active, in region. `KProtocolProcess::NpcRequestCommand` receives legacy73 and calls `NpcSet.SyncNpc`.
- Across the existing gateway 7.4MB log sampled via `Get-Content | Select-String`, **99,458 omitted-feature log lines** break down as 75=39,241; 80=55,153; 84=1; 145=5,042; 169=15; 205=1; 254=5. **No omitted legacy76/77** observed in this log. The native NPC request omitted log likewise was not observed in the recent subset. These absence observations are bounded by gateway logging and do NOT prove GS contains no NPC at runtime.
- Earlier GP01 Phase1 converter remains feature-gated `GP01_NPC_DISCOVERY_ENABLED=0`; live bridge PID3828 has not reloaded code. Full native NPC98/84 conversion remains unverified and **intentionally not emitted**. The user-reported Map53 movement improvement is not touched.

## Artifact + tests
- DEV script `C:\Server-Game\SwordOnline\Client3D-LAN\qa_runs\chatgpt-cn02-independent\audit_map53_npc_source_readonly.py` and test `test_audit_map53_npc_source_readonly.py`. Source files were read-only inputs; not attached to running GS memory.
- Auditor: `python audit_map53_npc_source_readonly.py C:\Server-Game\SwordOnline\ServerNew\_bin_v2_\gs\maps\country\balanghuyen` → exit **0**, all1496 parsed, 845 static defs.
- Audit unit tests: **3/3 PASS**, exit 0.
- Native bridge suite: **173/173 PASS**, exit 0.
- No restart, database write, active socket interception, binary/client modification, memory patch or new gameplay NPC frame to client.

## Required next gate before NPC runtime enabled
1. Correlate a live GS NPC index+ID and active region near the authenticated player, to discriminate *static NPC not activated* vs *AOI send absent* vs *gateway suppression*. Use only read-only GS instrumentation or dedicated QA instance.
2. Capture authenticated full legacy76/77/84/155 records if present; verify native4016 IL2CPP packet handler and exact 98/84 layouts with valid IDs, appearance, camp, NPC model mapping; no fabricated/unverified values.
3. Implement real native create/update/remove, bounded interaction requests; 4016 runtime QA: real named NPC appears, exact GS identity, click dialog, despawn/reconnect, no speed/collision regressions.
4. Only enable GP01 on guarded DEV canary after all validators PASS. Maintain collision global gates separately and never declare NPC PASS from disk counts/tests alone.
