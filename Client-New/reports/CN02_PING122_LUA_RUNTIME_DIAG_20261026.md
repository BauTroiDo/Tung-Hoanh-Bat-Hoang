# CN02 — Native ping122 size mismatch and separate Lua nil failures

Date: 2026-10-26 Asia/Ho_Chi_Minh. **Original repaired server and active QA client were NOT restarted or patched** during this investigation. Raw input log includes account/character names; do not embed those in commits.

## Verified packet root cause

1. Live gateway source: `C:\Server-Game\SwordOnline\Client3D-LAN\gs-bridge-m1\native_session.py`. Handler:
   `elif len(clear) == 5 and clear[0] == 82: backend.send(b'\x7a' + clear[1:])`.
   This *always* transmits a **5-byte opcode122** packet to legacy GS.
2. Original native GameServer source: `jxOnline/gameserver/Headers/KProtocolDef.h`: `c2s_ping` is opcode **122**; `KProtocol.h`: `PING_CLIENTREPLY_COMMAND` is packed **9 bytes**:
   `BYTE ProtocolType; uint32_t m_dwReplyServerTime; uint32_t m_dwClientTime;`.
3. `jxOnline/gameserver/Sources/Core/Src/KProtocol.cpp` size table has `sizeof(PING_CLIENTREPLY_COMMAND)` for `c2s_ping`. `CoreServerShell.cpp` compares calculated size against received size and emits exact `wCheckSize != nSize` warning.
4. `KSOServer.cpp::ProcessPingReply` additionally checks the first timestamp equals last genuine `m_nSendPingTime`. Wrong length or mismatched timestamp may result in client shutdown. Its second field is echoed in `s2c_replyclientping`. **Adding four zero bytes without protocol analysis is unsafe.**
5. The JSON `Client3D-LAN/reference/native-layout.json` is not authoritative for C2S ping; its entry `122` describes a *different* S2C layout. Always use the actual GS C2S source/headers for this correction.

**Repair candidate prepared offline only:** `tools/reference4016/native_ping_candidate.py` and `verify_native_ping_candidate.py`, **10/10 tests PASS**. Converts exact Unity opcode82 five-byte reply to a *structural* opcode122 nine-byte buffer, preserving four-byte server token and supplying an explicitly identified local monotonic gateway time proxy as the legacy client-time field. This is NOT end-to-end certified: confirm live token matches backend's last sent ping, the outbound native counterpart correctly carries/decodes ping and eventual S2C reply is correctly projected. Do NOT deploy to running gateway from this artifact alone.

**Important gateway activation gate:** `gs-bridge-m1/native_p0_readiness.py` checks SHA-256 of `native_session.py` against `native-p0-readiness.json`. Patching source without new tests and reviewed proof can prevent future native world entry. No such source/proof was changed. Restart of gateway during player's active session was avoided.

## Separate Lua problem

The runtime log shows `attempt to call a nil value` during `\script\System\player\playerlogin.lua` main and `playerlogout.lua` OnLogout, plus repeated `\script\system\online.lua` script errors. Source `jxOnline/gameserver/Sources/Core/Src/KPlayer.cpp` uses these entrypoints, and `KPlayerSet.cpp` calls OnLogout.

Inspection of installed server `ServerNew/_bin_v2_/gs` shows **none of these three filenames exists as a loose file** under the expected script/system paths (including Windows case-insensitive spellings). The directory contains `script/system/serverscript.lua`. `gs/pak/script.pak` is present, but `gs/package.ini` lists only map-related packages; **this alone cannot prove the runtime script was not loaded from another archive/loader**. `Logs/script_errors.txt` contains repeated errors (including numerous malformed-looking script paths) but no usable Lua stack trace identifying the nil callee in this inspection. No player Lua files were recreated or stubbed, to avoid corrupting login/logout effects or character saves.

**Next Lua acceptance:** locate actually mounted script source or package mapping and exact line-level traceback; determine absent function/global or encoding dependency; fix real source and regression-test OnLine/main/OnLogout with disposable QA character or sandbox, without modifying live player DB. Account archive messages indicate save success; they do not mean Lua hooks succeeded.

## Priority and gates

1. **P0:** code-review legacy ping token + native response semantics; make tested nine-byte C2S packet and complete ping echo translation, source proof manifest update, controlled gateway restart only after QA character logs out. Confirm no more opcode122 9≠5 warnings.
2. **P0:** script loader/virtual package inspection, targeted Lua nil traceback and fix; ensure required scripts are present in runtime script mounts and no nil execution during OnLine/main/OnLogout.
3. **Existing CN02-04:** separate missing original Map53 Addressables scene, omitted NPC/AOI and actual UI movement acceptance. Fixing ping does **not** automatically show terrain/NPCs. `reports/CN02_MAP53_RUNTIME_BLOCKER_20261026.md` is the companion report.
4. Bishop/GameServer original processes, account state/DB, original GameAssembly and client 4016 were not modified. **CN02-04 BLOCKED, CN03 CLOSED.**

Status: **ROOT CAUSE OF 122 LENGTH VERIFIED; STRUCTURAL PATCH PROTOTYPED AND 10/10 TESTED; PRODUCTION GATEWAY UNCHANGED. LUA EXACT NIL TARGET NOT YET ISOLATED.**

**Independent regression after staging this diagnostic:** `python -m unittest discover -s tools/reference4016 -p 'verify_*.py'` on the current workspace returned **241/241 PASS, exit0** (one preexisting SQLite ResourceWarning). The 10 ping tests are included in the 241; this count does NOT represent a live gateway integration test.
