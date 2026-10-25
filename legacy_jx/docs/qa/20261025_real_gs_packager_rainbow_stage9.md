# JX273 Issue #27 — Stage 9 QA: real GameServer packager -> Rainbow -> Goddess (2026-10-25)

**Disposition: packet/transport integration QA PASS; NOT production release acceptance. Keep Draft PR #29, P0 Issue #27 OPEN.**

## Purpose and scope

Earlier Goddess/Rainbow tests fabricated GameServer frame headers. This Stage 9 test uses the **original GameServer CPackager implementation and linked source** (gameserver/Sources/MultiServer/Common/Buffer.cpp, Exception.cpp, NodeList.cpp, Utils.cpp), instead of hand-building protocol frames. The QA client sends raw packed TProcessData (14-byte header) + 704-byte synthetic TRoleData + 36-byte JXREV001 trailer using three AddData() calls exactly as the GameServer save path. CPackager.GetHeadPack()/GetNextPack() emits real Rainbow frames. It is tested against the real isolated Goddess Stage 7 executable and Rainbow.dll over 127.0.0.1:15001.

## Native source

- tests/native/real_gs_packager_retry_qa.cpp: link real CPackager source and compare byte-exact frame segmentation/reassembly at sizes 14, 510, 511, 754, 1019, 4096 and 131072 bytes, first/middle/tail flags, user-data bytes, immutable copy and matching nonce/role revision removal from retry cache.
- tests/native/qa_packager_versioned_save_client.cpp: Rainbow client using real GameServer CPackager rather than a manually constructed six-byte transport header; tests server CAS and ACK.
- Base QA versioned client retained unchanged in tests/native/qa_versioned_save_client.cpp.

## Verified execution

- MSVC x64 native real CPackager/RoleSaveRetryCache QA: **1445/1445 assertions PASS**. One compile attempt including redundant legacy CriticalSection.cpp failed because the header already provides definitions; corrected QA link set excludes CriticalSection.cpp, original source untouched. Final compile and executable exit 0.
- Native Rainbow client compiled with real Buffer.cpp + other original dependencies: **PASS, 0 errors**.
- Goddess isolated x64 binary reused byte-for-byte from Stage7, QA listener strictly bound to loopback, GUI process ownership and port verified.
- Real CPackager + Rainbow -> Goddess: **5/5 PASS**: post-trade save (2->3), same nonce replay (no extra commit), collision -13, stale revision -17, valid progression (3->4).
- Stop/restart Goddess on the same disposable SQLite database: **2/2 PASS**, replay last receipt returns 1/revision4, older receipt returns -17.
- Offline SQLite read-only verification after restart: integrity_check ok; exactly 2 ordinary-save receipts on QA_A with nonces 500001 and 500003, trade_receipts exactly 2; revisions QA_A=4 QA_B=2; blobs QA_A gold72 length704, QA_B gold230 length664. These are synthetic values and DO NOT demonstrate real inventory/gold conservation across live player accounts.

QA artifacts/evidence: C:/Server-Game/SwordOnline/maintenance-builds/jx273_role_protocol_stage9_20261025. No QA executables, SQLite files or binaries committed to Git.

## Still not a real end-to-end login acceptance

This links the authentic GameServer packet framing source but **does not launch gs001.exe, CoreServer.dll, Bishop login session, two active player clients, or actual KPlayer serialized item inventory**. The real GS/CS/Goddess integration still requires test accounts on an entirely disposable QA environment and a controlled relog/resync handshake after exhausting retries. Do not label P0 resolved or deploy. Intentional role deletion and fenced-old-role migration require separate acceptance. Production binaries and player database were not changed.
