# CN02-BULK-03 — Gameplay batch manifest / native .pdata / Ghidra dynamic

**Thời điểm:** 26/10/2026 (Asia/Ho_Chi_Minh), branch `cg/cn02-anchor-evidence` từ checkpoint `58b25a0`.
**Nghiệm thu kỹ thuật:** PASS — metadata-driven four-group batches, native unwind-span samples, automated Ghidra Headless. **Không nghiệm thu tính đúng ngữ nghĩa của pseudo-code hoặc khả năng chơi game.**

## 1. Khối đầu vào đã xác minh

- PE64 `E:\Games\VLTK3D\SwordOnline\Client-3D-2\update\GameAssembly.dll`, SHA256 `011357720B210AA1F392A8B62FF8DE090D3A895230A6E0135BCE707196871069`.
- Metadata v39 tại `Game_Data\il2cpp_data\Metadata\global-metadata.dat`, SHA256 `5586F416AA26EBE57EA7F6580D0A53E5E5F29C50E425E64A4D3F2A95D6069E08`.
- SQLite indexed từ CN02-BULK-01: `E:\Games\VLTK3D\SwordOnline\Tool\_outputs\cn02-bulk\method-index.sqlite`.
- Script `tools/reference4016/build_gameplay_manifest.py` đọc metadata index read-only và **trực tiếp giải mã PE64 Exception Directory (DataDirectory #3)**, nhận diện **330.024 .pdata/unwind entrypoints**. Không đồng nghĩa có 330.024 phương thức C# độc lập, hoặc các region này bao phủ toàn bộ phương thức nhiều unwind frame.

## 2. Manifest — bốn nhóm gameplay

Đã tạo file manifest đầy đủ **ngoài Git**: `Tool/_outputs/cn02-bulk/gameplay-batches/manifest-v1.json`.

Sử dụng quy tắc phân nhóm class/type theo tên, ưu tiên type gameplay nền, chỉ chọn method RVA trùng **đầu .pdata entry** và độ dài region tối đa 8.192 byte. **Không tự nối các region liền kề** (đặc biệt `SubWorld.LoadMap` đã chứng minh có 2 unwind region). Giới hạn <=100 native entrypoints mỗi type, hai class đại diện cho mỗi nhóm.

| Nhóm | Hai type batch đầu | Method bodies sinh pseudo | Thời gian |
|---|---|---:|---:|
| Movement | `JXMovement`, `PlayerMovement` | 74 | 9,72s |
| NPC | `NPCInfoService`, `NPCSelectionService` | 100 | 10,55s |
| Combat | `CombatPerformanceGuard`, `UIAM_Combat` | 27 | 10,59s |
| Navigation | `AutoFindPathManager`, `AutoFindPath` | 61 | 10,15s |
| **Tổng** | **8 type** | **262 markers** | **41,01s** |

**4/4 batch chạy thành công với exit code 0.** Bốn file `dump.cs` có tổng 3.833.667 byte, nằm trong `Tool/_outputs/cn02-bulk/gameplay-batches/pilot-runs-v1/<GROUP>/Dump0`.

Cảnh báo chất lượng pseudo từ các lượt dump:

| Nhóm | Marker `[recovery: suspect]` | Số lần xuất hiện `__unknown` |
|---|---:|---:|
| Movement | 2 | 1 |
| NPC | 2 | 0 |
| Combat | 2 | 0 |
| Navigation | 1 | **50** |

Đây là **số marker và số lần xuất hiện chuỗi**, không phải tỷ lệ hàm sai/chính xác. Đặc biệt Navigation cần đánh giá thủ công + Ghidra/packet parity trước khi dùng làm mã chạy.

**An toàn khi chạy:** `tools/reference4016/run_manifest_batches.py` đọc manifest, gọi `run_body_pilot.py` với **type-only filter** để tránh lỗi dumper OR giữa assembly/type, giới hạn runtime, body bytes, kiểm tra SHA256 và từ chối ghi đè hoặc ghi trong Git.

## 3. Ghidra dynamic từ .pdata, không cần nhập RVA tay

Tạo `tools/reference4016/launch_ghidra_manifest.py` tự chọn **một native entrypoint được giới hạn vùng / mỗi nhóm** từ manifest. Script Ghidra `tools/reference4016/ghidra/CN02DynamicBatch.java` chấp nhận `RVA:SPAN`, kiểm tra 1–8 hàm/đợt, 64–2.048 byte/hàm, tối đa 8.192 byte tổng. Disassemble chỉ vùng bounded, mở rộng function body và báo thống kê, không export pseudo.

| Nhóm | RVA | Vùng PE64 .pdata | Ghidra decoded/body |
|---|---|---:|---:|
| Movement | `0x821410` | 286 B | **286/286** |
| NPC | `0xDFB950` | 695 B | **695/695** |
| Combat | `0x7D28F0` | 520 B | **520/520** |
| Navigation | `0xE841D0` | 344 B | **344/344** |

Ghidra 12.2 official Headless **exit 0**, **42,48s**; 4/4 RVA cho status `decompiled=true` và ký tự pseudo không rỗng. `-noanalysis -readOnly -deleteProject`; log xác nhận **Discarded file import due to readOnly option**.

Lượt chạy tự động lần đầu bị thiếu Java trong môi trường con (User PATH/JAVA_HOME của Windows không tự cập nhật sang tiến trình Remote Desktop Commander đã mở). Chương trình `analyzeHeadless.bat` hiện thông báo `Press any key`; đã dừng **đúng cmd.exe con do script này tạo**, giữ nguyên log lỗi. Sửa launcher để xác minh JDK 25 đã cài, truyền `JAVA_HOME` + Java bin PATH tường minh và `stdin=DEVNULL` để không treo. Chạy lại vào **thư mục mới** `ghidra-dynamic-v3-auto`: PASS.

## 4. Lệnh tái lập

Tại `E:\Games\VLTK3D\SwordOnline\Client-New`:

```powershell
$tool='E:\Games\VLTK3D\SwordOnline\Tool'
$batch="$tool\_outputs\cn02-bulk\gameplay-batches"

python tools/reference4016/build_gameplay_manifest.py --output "$batch\manifest-new.json" --summary reports/evidence/cn02-4016-il2cpp/gameplay-manifest-summary-new.json

python tools/reference4016/run_manifest_batches.py --manifest "$batch\manifest-v1.json" --output-root "$batch\pilot-runs-v2" --timeout 180 --max-body-size 8192

python tools/reference4016/launch_ghidra_manifest.py --manifest "$batch\manifest-v1.json" --output "$batch\ghidra-dynamic-next" --timeout 240
```

**Quan trọng:** các công cụ cố tình **từ chối ghi đè output** và chạy chỉ trong `Tool/_outputs/cn02-bulk/gameplay-batches`; đổi tên lần chạy khi tái lập. Tài liệu JSON tổng hợp có thể đặt đường dẫn mới để không đè baseline. `verify_gameplay_manifest.py` đang kiểm chứng baseline nguyên bản `manifest-v1`, `pilot-runs-v1`, `ghidra-dynamic-v3-auto`.

## 5. Nghiệm thu / bảo vệ các lane khác

- **24/24** test CN02-BULK-03 PASS; đối chiếu manifest, 8 type riêng, .pdata exact-start, giới hạn kích thước, 4 batch exit 0, native samples Ghidra, read-only, chống ghi vào repository.
- Test tổng kết lại gồm 22 test CN02-BULK-02 + 17 test CN02-BULK-01 + 100 test các pha trước = **163/163 regression tĩnh PASS** (sau khi chạy lại cùng phiên).
- Full raw pseudo/dump/SQLite/manifest chứa provenance phương thức nằm **ngoài Git**. Commit chỉ script, report và JSON số liệu, không chép mã binary hay tài nguyên client gốc.
- Tám file `UnityProject/Assets/_JXClient/**/Materials/*.mat` đang dirty do lane Unity khác: giữ nguyên, không stage/reset/revert.
- **CN02-04 BLOCKED, CN03 CLOSED**: native extraction chưa tạo ra sáu hệ số affine Map53 runtime, 3+1 landmark ghép cặp, hoặc kiểm thử gameplay.

## 6. CN02-BULK-04

Mở rộng manifest theo nhóm **Network Protocol / Scene / Gameplay** bằng ngân sách method/CPU/RAM rõ ràng. Thêm bằng chứng semantic parity cho một vài luồng thực (movement/nav packets, state transitions) trước khi chuyển pseudo vào `Client-New`. Mọi Ghidra code cần đối chiếu IL2CPP metadata/registry và test thực khi map53 runtime được nghiệm thu.
