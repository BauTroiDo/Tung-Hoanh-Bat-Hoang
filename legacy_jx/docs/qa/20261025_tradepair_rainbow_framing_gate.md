# Phase 3 ? Rainbow to Goddess trade-pair framing gate (2026-10-25)

## Verdict
**Native transport/negative ACK PASS; full GameServer-to-Goddess release gate NOT PASSED.**
Branch: checkpoint/jxonline-273-triage-20261024; previous HEAD e74c58e.

## Root causes proven in QA
- Initial loopback Goddess could not enter WinMain because its runtime folder omitted libssl-3-x64.dll and libcrypto-3-x64.dll (dependency chain via libmysql.dll). Native thread stack indicated Windows 0xC0000135 DLL_NOT_FOUND. Restored only into isolated QA.
- A loopback QA variant passed inet_addr("127.0.0.1") to OpenService, but legacy CSocket::InternetAddress also calls htonl. Fixed only the QA variant by passing INADDR_LOOPBACK and port 15001.
- Isolated Goddess opened **127.0.0.1:15001**, created a disposable role.db and received the native Rainbow.dll client connection.
- The real network ACK, opcode 57, returned -15 for intentional invalid-generation requests; TxID matched both times. The same raw request *without* a CPackager wrapper had no ACK.
- Thus the Core journal sender was incorrect: DispatchTradePairToGoddess sent a raw TTradePairSaveRequest, while Goddess LargePackProcess expects the 6-byte CPackager header.
- A first 8192-byte fragment caused the native Rainbow test executable to fail with Windows 0xC0000409. Rainbow's client source allocates a fixed 1024-byte send buffer. The patch now fragments the trade pair at **510 application bytes per fragment**, matching CPackager's legacy GetHeadPack(...,512).
- Native loopback transport regression: one-fragment 106-byte request -> ACK -15; 20,200-byte multi-fragment request -> ACK -15; raw unframed request -> no ACK; process exit 0.
- Network test evidence: maintenance-builds/jxonline273_network_qa_20261025/rainbow-framed-510byte.log.

## Source and regression test files
1. gameserver/Sources/Core/Src/KPlayerTradeJournal.cpp: sends header/middle/tail CPackager fragments, 510-byte payload, 6-byte CPackager prefix, propagates failed HRESULT. Keeps existing role names, TxID, CRC and post-trade blobs intact.
2. test_phase3_final_runtime.cpp: mock reassembles atomic-pair fragments before parsing; adds payload length checks.
3. gameserver/Sources/test_phase3_persistence_e2e.cpp: same reassembly and bounds checks.
4. gameserver/Sources/test_phase3_runtime_verified.cpp: mock retains last transport fragment but separately assembles a complete trade-pair request for assertions and crash replay.

## QA builds and test evidence
- Full CoreServer x64 rebuild PASS, zero compiler/link errors. Isolated CoreServer.dll SHA256 96D776D65F3A3E61CDBE4D3C652C9F78CF08CD5CD3F2BA43015386FAA2C1903D.
- Isolated gs001 x64 rebuild PASS, linked to the rebuilt CoreServer.lib, zero compiler/link errors. SHA256 63BE695EB2917927982CFFD9B1ADD67963E4DEB12564E695DDB0C25F2AF3EB07.
- Loopback Goddess QA executable built PASS, not a deployable production release. SHA256 1F241E7CAAA68B3E14850DC052D106B88CA70710D48176B48B1601A19F5FC89B.
- Fresh CoreServer DLL in QA:
  - FinalRuntime **60/60 PASS**;
  - PersistenceE2E **27/27 PASS**;
  - RuntimeVerified **60/60 PASS**;
  - Combined **147/147 PASS**, three exit codes 0.
- QA result: maintenance-builds/jxonline273_framing_core_20261025/qa/framing-v2-results.json.
- Previous native Goddess SQLite method checks were 53/53 PASS. These were not rerun here and are not full network positive-commit coverage.

## Outstanding release blockers
- Test a real isolated GameServer process with synthetic roles against Goddess QA. Confirm **positive ACK, exactly two committed role snapshots, one receipt, two per-role fences**, and no extra money or items.
- Test dropped ACK, NACK, timeout, restart/relog, delayed replay and recovery of character controls without duplicate trade settlements.
- Bounded CPackager receive allocation is approximately 128 KiB by default; verify largest legitimate serialized dual-role packets. Oversized packets may otherwise be discarded without ACK.
- Earlier P0 concerns remain: legacy non-trade single-role saves can overwrite a newer atomic trade; 64-bit Snowflake-style TxID ordering requires clock/instance discipline across restarts.
- Legacy encoding/PDB compiler warnings remain.
- Do not merge or deploy until all full integration gates pass. Repo has no Git remote, so no push performed.

## Safety and checkpoint
- All QA builds and runtime files are under C:/Server-Game/SwordOnline/maintenance-builds; original projects' PostBuildEvents were disabled in disposable project copies.
- QA Goddess was stopped after tests, and no listener remains on 15001.
- Production Goddess, gs001, Bishop, S3Relay and S3RelayServer were preserved. No real account, character, DB, production binary or live config was changed.
