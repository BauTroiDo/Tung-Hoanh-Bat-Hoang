# CN02-BULK-04 — Network Protocol / Scene-Addressables / Gameplay State

**Ngày:** 26/10/2026, thời gian Việt Nam. **Branch:** `cg/cn02-anchor-evidence`. **Baseline:** `a367f85`.
**Trạng thái:** BATCH STATIC PASS; semantics/runtime behavior **NOT VERIFIED**. Không đổi UnityProject, server, DB, binary tham chiếu.

## 1. Kiểm kê bộ công cụ người dùng cung cấp

Gốc đọc/chỉ kiểm kê: `E:\Hack Game\Cong-cu`.

| Thư mục | Phát hiện | Sử dụng trong CN02 |
|---|---|---|
| `IDA 7.3` | `ida64.exe`, `idat64.exe`, PE64 loader, script Python/IDC; Authenticode **NotSigned**. Không xác minh được DLL Hex-Rays x64 (`hexrays64.dll`/`hexx64.dll`). Có `ida.key`, nhưng **không đọc nội dung/chưa xác minh giấy phép**. Có thêm thư mục riêng ghi `Patched`. | **Không thực thi** bản IDA chưa rõ nguồn gốc; không dùng bản Patched. IDA 7.3 cũng cũ hơn công cụ Ghidra 12.2 đang hoạt động |
| `BinaryNinja` | Có `binaryninja.exe`, chữ ký Windows **Valid** | Ứng viên để đối chiếu decompiler; chưa xác minh quyền sử dụng/CLI, chưa chạy |
| `die_win64_portable_3.09_x64` | Có `diec.exe`; chữ ký NotSigned | Hỗ trợ phân loại PE/packer khi cần, không chạy file lạ |
| `dnSpy-net-win64`, `ILSpy_binaries_8.2.0.7535-x64` | Công cụ phân tích assembly .NET managed; `ILSpy.exe` Authenticode Valid | Hữu ích với DLL .NET/DummyDLL, **không** thay thế dịch ngược body IL2CPP native trong GameAssembly.dll |
| `X64dbg` | Có x64dbg.exe, chữ ký Valid | Chỉ dùng khi cần quan sát runtime trong môi trường QA, **chưa chạy** |
| `ghidra_11.1.2_PUBLIC` | Bản phát hành cũ | Duy trì **Ghidra 12.2 chính thức đã kiểm tra hash** tại `E:\Games\VLTK3D\SwordOnline\Tool\_built` thay vì hạ phiên bản |

Không sửa/copy bất kỳ công cụ ngoài nào. Không chạy `Game.exe` hay chương trình crack/patch. Kết quả kiểm kê chỉ chứng minh tệp tồn tại/chữ ký hiện tại, không bảo đảm độ an toàn toàn bộ bộ công cụ.

## 2. Phân tích 10 type theo nhóm

Input tham chiếu: `GameAssembly.dll` bản 4016 SHA256 `011357720B210AA1F392A8B62FF8DE090D3A895230A6E0135BCE707196871069`; `global-metadata.dat` SHA256 `5586F416AA26EBE57EA7F6580D0A53E5E5F29C50E425E64A4D3F2A95D6069E08`.

- `tools/reference4016/build_extended_manifest.py`: đọc SQLite tại `Tool/_outputs/cn02-bulk/method-index.sqlite`, kiểm tra SHA input, giải PE64 `.pdata` (330.024 unwind region entrypoints) và lập manifest theo 3 nhóm. Output full manifest (RVA/type signatures) giữ ngoài Git.
- `tools/reference4016/run_extended_batches.py`: sử dụng bộ runner `run_body_pilot.py` đã nghiệm thu: **type-only selector**, không thêm assembly filter vì dumper ghép hai filter bằng OR; mỗi batch giới hạn số type, timeout, body size, ghi output riêng không đè kết quả cũ.
- `tools/reference4016/probe_extended.py`: chọn mẫu RVA bounded từ manifest và dùng lại `CN02DynamicBatch.java` của Ghidra; khởi tạo JAVA_HOME/PATH xác minh, stdin=DEVNULL, readOnly/deleteProject.

| Hệ thống | Type chọn | ScriptMethod bindings trong SQLite | Bounded .pdata native entries | Pseudo markers đã xuất | Thời gian |
|---|---|---:|---:|---:|---:|
| Network Protocol | `BishopNetwork`, `GameServerNetwork`, `Network.Header.ProcessGameUnity` | 264 | 208 | **351** | 10,22s |
| Scene / Addressables | `MapDataService`, `VietKiem.Levels.Application.SceneLoadService`, `SceneLoader` | 108 | 100 | **175** | 10,40s |
| Gameplay State | `WalkState`, `AttackState`, `StateMachine`, `DeathState` | 65 | 61 | **71** | 10,99s |
| **Tổng** | **10 type** | **437 bindings** | **369 bounded entries** | **597 markers** | **31,61s** |

**3/3 dumper batches exit code 0**. 597 pseudo markers **không phải** 597 MethodDefinitions, cũng **không chứng minh thân hàm đúng ngữ nghĩa**: filter có thể bao gồm class nested/generic/alias. Tổng kích thước dump.cs mới 7.243.718 bytes, lưu ngoài repo. Phân tích chất lượng: Network 9 marker suspect và 2 `__unknown`; Scene 4 suspect, 0 unknown; State 4 suspect, 12 unknown. Đây là số lần marker xuất hiện, không phải số hàm sai.

## 3. Kiểm tra Ghidra độc lập

Ghidra 12.2 official Headless auto chọn 3 mẫu ngắn từ manifest, giải mã đúng native `.pdata`, **exit 0 trong 43,29s**, log xác nhận `Discarded file import due to readOnly option`.

| Nhóm | Sample RVA | .pdata bytes | Disassembly / function body | Decompiler |
|---|---|---:|---:|---|
| Network | `0xC25B50` | 439 | 439/439 | Có pseudo-code |
| Scene | `0xC730E0` | 206 | 206/206 | Có pseudo-code |
| Gameplay State | `0x879EA0` | 541 | 541/541 | Có pseudo-code |

Ghidra không ghi nguồn pseudo-code hoặc binary vào Git. `-noanalysis` + bounded instruction decode chứng minh đã đọc đủ byte vùng được cấp, **không** chứng minh đủ body với hàm có nhiều unwind record, hoặc logic runtime chính xác.

## 4. Static native contract anchors

Đối chiếu **chỉ tên phương thức / RVA** trong index, không gửi packet và không khởi động client:

| Signature | Native RVA |
|---|---|
| `BishopNetwork.Login()` | `0x8F3750` |
| `GameServerNetwork.ConnectToServer()` | `0x92CA30` |
| `Network.Header.ProcessGameUnity.OnSyncWorld()` | `0xC38990` |
| `MapDataService.LoadForTemplate()` | `0x8C9BF0` |
| `SceneLoadService.ResolveVariantKeyAsync()` | `0xC74650` |
| `WalkState.Enter()` | `0x87B290` |
| `StateMachine.ChangeState()` | `0x878A30` |

**7/7 static anchors match**. Tên/RVA có giá trị định hướng điều tra, chưa chứng minh luồng packet thực, thứ tự login/scene hay gameplay state transition.

## 5. Kiểm thử, lưu trữ và tái lập

- `tools/reference4016/verify_extended_batches.py`: **27/27 PASS** — hash PE, schema, bounds .pdata, 3 nhóm/10 type, type-only, batch exit, số pseudo, 7 contract RVA, 3 Ghidra samples, readOnly, phạm vi output ngoài Git, và không gán nhãn semantic parity.
- Các kiểm thử cũ CN02-BULK-03 (24), BULK-02 (22), BULK-01 (17), legacy (100): **163/163** PASS khi tái kiểm. Tổng lũy kế **190/190** PASS sau khi chạy tất cả các nhóm.
- Raw output: `E:\Games\VLTK3D\SwordOnline\Tool\_outputs\cn02-bulk\extended\manifest-v1.json`, `runs-v1`, `ghidra-probe-v1`. Đây là dữ liệu riêng tư không đưa Git.
- File theo dõi Git: `reports/evidence/cn02-4016-il2cpp/extended-summary.json` (chỉ thống kê/định danh bằng chứng), mã chạy + tài liệu này.

Tái lập ở `Client-New` (đặt đường dẫn output mới, không ghi đè bản đã nghiệm thu):

```powershell
$base='E:\Games\VLTK3D\SwordOnline\Tool\_outputs\cn02-bulk\extended'
python tools/reference4016/build_extended_manifest.py --output "$base\manifest-v2.json" --summary reports/evidence/cn02-4016-il2cpp/extended-summary-v2.json
python tools/reference4016/run_extended_batches.py --manifest "$base\manifest-v2.json" --output "$base\runs-v2" --timeout 180
python tools/reference4016/probe_extended.py --manifest "$base\manifest-v2.json" --output "$base\ghidra-probe-v2" --timeout 240
python tools/reference4016/verify_extended_batches.py
```

Lưu ý: verifier mặc định kiểm tra baseline `manifest-v1`, `runs-v1`, `ghidra-probe-v1`, không tự chuyển sang dữ liệu mới. Repo có các file material UnityProject đang dirty từ luồng khác; tuyệt đối không stage, revert hay reset file đó.

## 6. Gate và việc tiếp theo

**CN02-BULK-04 đạt PASS về công cụ / phân tích metadata**, không phải client parity. **CN02-04 vẫn BLOCKED, CN03 CLOSED**: thiếu affine Map53 runtime sáu hệ số, 3+1 cặp điểm mốc cùng cảnh và kiểm thử render/movement thật. Bước tiếp theo nên ưu tiên **protocol semantic trace có bằng chứng thực** cho một luồng login→WorldSync→MapData/Scene→WalkState, đối chiếu packet và kết quả màn hình; dùng static anchors này và Ghidra/IDA có giấy phép hợp lệ khi cần, không chuyển pseudo vào production nếu chưa test.
