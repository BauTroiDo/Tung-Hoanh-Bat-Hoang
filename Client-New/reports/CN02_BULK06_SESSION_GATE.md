# CN02-BULK-06 — IDA 7.3 Python 2.7 x64 and single-session runtime evidence gate

**Date:** 2026-10-26, DESKTOP-TPG2RNF. **Previous checkpoint:** af17bb6.
**Status:** IDA Python dependencies staged; IDA batch blocked by license acceptance. QA evidence-contract validator **19/19 synthetic tests PASS**, actual original-client / GameServer / Map53 scene gate **BLOCKED**.

## 1. IDA Python 2.7 integration

User explicitly authorized the existing \`E:\Hack Game\Cong-cu\IDA 7.3\`, and supplied \`C:\Users\QT\Downloads\python27 (1)\python27.dll\`. On this machine the download directory has no literal " 64" suffix; the DLL itself is PE64 (\`0x8664\`), v2.7.3, SHA256 \`2F4FDB8C45F3DDA412E2C0599F016F2D17689E3EDB447FBCE3DD239AFB9E04FF\`. It is unsigned and downloaded from an unaffiliated DLL collection. Microsoft Defender custom scan completed without a reported hit; this is **not** independent provenance verification.

Initial temporary placement of the supplied DLL into IDA root changed IDAPython failure from "python27.dll can't load" to \`ImportError: No module named site\`: the download was only one DLL, not Python's standard library. For a complete/runtime version the official **Python 2.7.18 x64 MSI** was downloaded directly from Python.org to:

\`E:\Games\VLTK3D\SwordOnline\Tool\_downloads\python-2.7.18.amd64.msi\`

Official MSI: 20,598,784 bytes, Authenticode **Valid**, MD5 \`A425C758D38F8E28B56F4724B499239A\`, SHA256 \`B74A3AFA1E0BF2A6FC566A7B70D15C9BFABBA3756FB077797D16FFFA27800C05\`. The SHA matches independent package checksum and MD5 matches Python.org. Download source: \`https://www.python.org/ftp/python/2.7.18/python-2.7.18.amd64.msi\`. Python 2.7 has reached end-of-life; **only use for this isolated legacy IDA 7.3 toolchain**.

The MSI was **administratively extracted** without global installation to:
\`E:\Games\VLTK3D\SwordOnline\Tool\_runtime\Python27-2.7.18-x64\`.
\`python.exe -c "import site,sys;print(sys.version);print(sys.prefix)"\` PASS, version 2.7.18 AMD64, local prefix.

With IDA processes stopped, copies created within \`E:\Hack Game\Cong-cu\IDA 7.3\`:
- \`python27.dll\`: replaced the user-supplied v2.7.3 with official v2.7.18 binary, SHA256 \`5845EDC96B8A12626F2752518D5AA80113CE4E075B41CEB53BAD701FD939B0B0\`, matching extracted official.
- \`Lib/**\`: Python 2.7.18 standard library including \`site.py\`; new directory, no previous collision.
- \`DLLs/**\`: companion Python extension DLLs; new directory, no previous collision.

Original user-provided v2.7.3 DLL backed up at
\`Tool\_runtime\ida73-user-python27-v2.7.3-backup.dll\`.
**Do not use the original 32-bit DLL in \`C:\Users\QT\Downloads\python27\`; all IDA executables/plugins inspected are 64-bit.** No Python 2 global PATH / PYTHONHOME / registry edits, no other user Python versions changed, no patch/crack installed, no original GameAssembly or client modified.

### Headless diagnostic

Isolated test binary: \`%TEMP%\jx1-ghidra-smoke-20261025\smoke.exe\` (130,560-byte synthetic Rust program), **not** the client. Script/logs in \`Tool\_outputs\cn02-bulk\semantic-trace\`.

1. Before supplied DLL: IDAPython plugin could not load \`python27.dll\`.
2. Supplied DLL alone: \`ImportError: No module named site\`.
3. Full official Python runtime in IDA root: no missing-Python diagnostic, but \`idat64.exe -A -L...\` logged **"License not yet accepted, cannot run in batch mode"** and returned exit **1**.

**Status:** Python dependency installation complete, IDA **Headless NOT PASS** pending user's legitimate GUI license/terms acceptance. Do not bypass license prompts or label batch decompilation successful. Open the same IDA 7.3 through its normal GUI under the account that will use it, accept legitimate terms if prompted, then repeat synthetic \`idat64.exe\` test. Ordinary IDA GUI behavior was not tested or changed in this session; user reports it works. Ghidra 12.2 remains the tested automated fallback.

## 2. CN02-BULK-06 single-session QA evidence contract

Added:
- \`tools/reference4016/check_single_session.py\`: strict **sanitized JSON session** structural validator.
- \`tools/reference4016/verify_single_session.py\`: **19/19 synthetic-only tests PASS**.

Expected JSON envelope:
- \`schema="cn02-single-session-v1"\`, \`sessionId="qa-<pseudonym>"\`, \`actorId="actor-<pseudonym>"\`, \`mode="live"\`.
- Ordered events with monotonically increasing \`seq\`/\`tsMs\`, matching session/actor, and expected claimed \`origin\`:
  \`bishop_auth\`, \`gs_sync\`, \`world_sync\`, \`load_map\`, \`asset_verified\`, \`scene_ready\`, \`user_move_request\`, \`gs_move_ack\`, \`unity_move_echo\`, plus \`affine\`, \`landmarks\`.
- \`WORLD_SYNC\` fields match SubWorld 53, MapCopyID 53, Region 6,488,114; \`load_map\` inputs (templateId 53, mapId 53, region 6,488,114, mps 0/0).
- Catalog/resource integrity, visible avatar, explicit **user click after scene-ready**, linked movement request/GS ACK/client echo and finite coordinates.
- Six finite **active runtime** affine coefficients and four actual paired same-scene original MPS↔Unity landmarks; first three must be noncollinear; fourth holdout residual computed, but a reviewer must supply an approved tolerance.
- Four SHA256 claims for provenance: Bishop, original GameServer, original client, scene screenshot. These digests are **not themselves independently authenticated** by this parser.
- Raw credentials, tickets, network packets, account names or IPs must **not** be embedded in this JSON. Input/output only under \`Tool\_outputs\cn02-bulk\session-qa\`, outside Git.

Example execution after a **genuine fresh sanitized QA session** has been collected and cross-checked separately:

\`\`\`powershell
cd E:\Games\VLTK3D\SwordOnline\Client-New
python tools\reference4016\check_single_session.py --input "E:\Games\VLTK3D\SwordOnline\Tool\_outputs\cn02-bulk\session-qa\<new-session>\events.json" --output "E:\Games\VLTK3D\SwordOnline\Tool\_outputs\cn02-bulk\session-qa\<new-session>\assessment.json"
python tools\reference4016\verify_single_session.py
\`\`\`

**Important:** Even when every structural field is valid, the validator sets \`runtimeEndToEndAccepted=false\` and \`independentReviewRequired=true\`. It cannot independently prove a claimed Bishop/GameServer/client log is authentic, and must never turn a **synthetic fixture into gameplay evidence**. The previous historical logs were from separate sessions and are not adequate to close this gate.

## 3. Remaining gates

- **IDA 7.3 Headless:** BLOCKED by user license acceptance. Python runtime dependencies READY.
- **CN02-BULK-06 runtime QA:** BLOCKED: a single new session with independently verified Bishop/GS, correct scene ready, user-clicked movement with original GS ACK and echoed client position, plus real active Map53 matrix + 3+1 anchors is still missing.
- **CN02-04:** BLOCKED; **CN03:** CLOSED.
- \`UnityProject/**\` is Antigravity-owned. Eight pre-existing material modifications remain untouched. No server/DB/client changes or production restart.

## 4. Rollback of the IDA Python addition

Only if needed, after fully closing IDA:
1. Copy the backed-up user DLL \`Tool\_runtime\ida73-user-python27-v2.7.3-backup.dll\` back into \`IDA 7.3\python27.dll\`.
2. Remove only the **newly created** \`IDA 7.3\Lib\` and \`IDA 7.3\DLLs\` directories, after verifying no other tools have added files to them.
3. Do not touch \`IDA 7.3\python\` or plugins, and do not delete the source Downloads DLL.

No rollback was performed; current official Python 2.7.18 integration remains in place.

**Regression tổng kết:** 13 bộ kiểm thử CN02, **238/238 PASS** (19 bài thử schema tổng hợp mới + 219 bài thử cũ). Đây là kiểm tra mã và tính nhất quán của hồ sơ, **không** chứng minh cảnh thật đã render hoặc được GameServer xác nhận di chuyển.
