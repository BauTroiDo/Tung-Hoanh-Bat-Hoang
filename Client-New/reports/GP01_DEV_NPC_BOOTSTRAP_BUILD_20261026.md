# GP01 — Map53 authoritative NPC bootstrap: isolated build and QA gate

**Date:** 2026-10-26. **Result:** source patch + Python tests PASS, standalone Engine/CoreServer build PASS, **NOT deployed**; client NPC rendering remains BLOCKED.

## Why
- DEV Map53 has 845 static NPC definitions across 347 regions, but existing Gateway log contains no legacy NPC_MIN(77) or NPC_FULL(76) feature omission. The initial GS sync also did not deliver nonlocal NPC state.
- Nearest static definition around historic player(50401,104412) was about 852 MPS, around later player(50381,107753) about 389 MPS. Source presence is not runtime spawn; actual GS NPC IDs cannot be inferred from template IDs.
- Country Map53, NpcActivation=1 (some NPCs eager, others region activated); do not turn off GS collision/NPC validation or invent NPCs.

## Isolated server patch
- Worktree: `C:\Server-Game\SwordOnline\_AI_Worktrees\gp01-npc-bootstrap`, branch `fix/gp01-npc-bootstrap`, commit `f976dc0` based on `4c25bf7`.
- Exactly 55 lines added in `gameserver/Sources/Core/Src/KProtocolProcess.cpp`; CRLF/legacy GBK bytes preserved; `git -c core.whitespace=cr-at-eol diff HEAD^ --check` exit0.
- Compile flag **`JX3D_GP01_NPC_SNAPSHOT`**, runtime env **`JX3D_GP01_NPC_SNAPSHOT=1`** independently required. Both disabled by default. Dedicated DEV original-GS 69-byte `c2s_requestnpc` with uint32 NPC ID **0** is a snapshot request; ordinary nonzero IDs still use `NpcSet.SyncNpc(ID,...)`.
- Validates Player[nIndex], player NPC identity, MapId53/subworld, authoritative GS coordinates, 15s per-player cooldown, max32 existing nonplayer NPCs within |dx|,|dy| <=1536 legacy MPS. Responds using original `KNpc::SendSyncData(clientIdx)`. Does **not** synthesize NPC ID, teleport, item state, or login ACK.
- Source patch archived as `Client3D-LAN/qa_runs/chatgpt-cn02-independent/GP01_DEV_NPC_SNAPSHOT_SERVER.patch` and published to GitHub branch.

## Gateway companion (not live)
- `gs-bridge-m1/native_npc_aoi.py`: `build_dev_npc_snapshot_request()` emits exact legacy69B opcode73 + ID0 + 64 zero bytes; snapshot responses accepted only from authenticated backend after an explicit snapshot opt-in; ID/position/radius and max48 cached. No native NPC render emitted yet.
- `gs-bridge-m1/native_session.py`: after genuine native SyncEnd ACK, send snapshot request only if BOTH `GP01_NPC_DISCOVERY_ENABLED=1` and `GP01_DEV_NPC_SNAPSHOT=1` are set. Off by default. Backup pre-snapshot versions kept.
- `test_native_npc_aoi.py`: new negative/positive snapshot cases. `python -m unittest discover -s gs-bridge-m1 -p 'test_*.py' -q` => **178/178 PASS, exit0**; py_compile PASS, exit0.

## Build evidence and caveat
- Isolated output: `C:\Server-Game\SwordOnline\_AI_Builds\gp01-npc-bootstrap\gs`.
- First Core build compiled source but failed link because existing Engine.lib lacked `Utf8ToGbk`. Rebuilt Engine Release|x64 from same worktree => exit0; linked Core Server Release|x64 to isolated Engine.lib => exit0. Temporary Core.vcxproj library-path change reverted before commit. Build warnings remain (old encoding and TargetName mismatch).
- Candidate `CoreServer.dll` SHA256 **CC0435153CA828E353EFAA57181CD0AEE080756368334B0D5CFFE11B152D59D5**. Installed DLL hash **C4ADAD7B996712A3D489C9C6CA65B9D30963BA07389AE3544114A5464BAC82C7**.
- Both DLLs export exactly **1291** named symbols with **0 removed**, **0 added**, **0 ordinal changes**. Same export table does NOT prove gameplay/source parity.
- **Critical release gate:** worktree base `4c25bf7` may lag current deployed server fixes. Do NOT overwrite current CoreServer.dll/Engine.dll without independently reconciling later fixes and runtime P0 validations.

## Live state / next actions
- At investigation time client4016 Game.exe PID21188 and gateway PID3828 remained connected (TCP Established on 14180) and GS PID5036 remained active. No processes were stopped/restarted. Character DB unchanged.
- No GP01 snapshot command yet executed against GS, no real native NPC spawn, and no NPC-dialogue runtime PASS. Client native NPC98/84 marshalling remains unverified (`production_ready=False`). **Do not claim GP01 finished.**
- Next: integrate GP01 server patch into source corresponding exactly to installed DEV build; isolated QA on empty/no-player session; guarded runtime bootstrap confirms GS full NPC76 with actual runtime IDs; complete native4016 packet converter & model matching; real-client NPC render/click/dialogue; test movement/collision regression, reconnect, despawn; then rollout.
