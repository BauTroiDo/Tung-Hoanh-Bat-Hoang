# CN02 DEV runtime evidence plan

Goal: establish a real GS ping timestamp roundtrip and distinguish missing Lua hooks from Lua execution errors; keep CN02-04 blocked until scene/NPC/manual movement evidence exists.

Execution: native in this session, as authorized by the user. No database edits or fabricated hooks/entities.

1. Verify deployed NativePingPair and current process/source hashes. Run server unittest discovery from Client3D-LAN with PYTHONPATH set to that directory.
2. Add numeric logs only at GS143 receipt, legacy122 send and GS174/native131 echo. Back up native_session.py and native-p0-readiness.json. Run full bridge suite; confirm readiness rejects stale source; refresh only changed source hash; confirm ready with m1_p0_pass false. Restart gateway only with zero active gateway sessions, preserving native bridge/allowlist. Verify greeting and startup log.
3. Inspect real KSortScript/KPlayer/KPakFile loaders, script.txt, runtime script tree and PACK IDs. Do not add substitute Lua files. Record exact source branch responsible for errors.
4. Use one QA client and user-entered credentials. Observe scene key/load callbacks, packet timestamps, mirror GETs, NPC/AOI and one manual movement after scene ready. If no authenticated session, record PENDING rather than infer runtime success.
5. Re-run relevant checks, update checkpoint/report and commit only this claim's files. Record rollback and remaining blockers.
