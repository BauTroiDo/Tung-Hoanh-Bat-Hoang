# CN02-BULK-01 — Toàn bộ IL2CPP v39: batch metadata + searchable RVA index

**Ngày:** 26/10/2026 (Asia/Ho_Chi_Minh). **Máy:** DESKTOP-TPG2RNF. **Branch:** cg/cn02-anchor-evidence.
**Trạng thái:** **PASS cho batch metadata và cross-check**, **NOT VERIFIED cho full native decompilation/Unity gameplay**.

## 1. Đầu vào giữ nguyên (read-only)

- `E:\Games\VLTK3D\SwordOnline\Client-3D-2\update\GameAssembly.dll`, SHA256 `011357720B210AA1F392A8B62FF8DE090D3A895230A6E0135BCE707196871069` (107.323.392 B).
- `E:\Games\VLTK3D\SwordOnline\Client-3D-2\update\Game_Data\il2cpp_data\Metadata\global-metadata.dat`, SHA256 `5586F416AA26EBE57EA7F6580D0A53E5E5F29C50E425E64A4D3F2A95D6069E08` (20.101.168 B).
- Client Unity `6000.3.9f1`, IL2CPP v39 (metadata variable-width indices).
- Không chạy/chỉnh Game.exe, không sửa UnityProject (Antigravity owner), server, nhân vật hoặc DB.

## 2. Kết quả thực thi hai công cụ độc lập

| Phần | Kết quả |
|---|---|
| `il2cpp-dumper-rs 8.0.0` metadata-only (không pseudo/native bodies) | **PASS**, exit 0, 21,20 giây |
| Dumper TypeDefinition | **21.485** |
| Dumper MethodDefinition | **161.267** |
| Dumper `script.json` ScriptMethod (gồm nhiều generic instantiation) | **414.519** — *không đồng nghĩa 414.519 MethodDef* |
| Dumper xuất `dump.cs` | 53.321.175 B, **đã tạo** |
| Dumper xuất `script.json` | 265.692.264 B, **đã tạo** |
| InspectorRedux 2026.2 | **Loaded metadata v39, xuất C# stub và báo `Export finished`** |
| Inspector `cs/il2cpp.cs` | 26.748.175 B, **đã tạo** |
| Inspector process | Có hiện tượng giữ process sau khi xuất; đã dừng **chính process CLI của phiên này**, không được ghi là Inspector exit 0 |
| CodeRegistration (hai công cụ khớp) | `0x184ADEF50` |
| MetadataRegistration (hai công cụ khớp) | `0x1856DE9A0` |

Dumper cảnh báo `Failed to write diffable cs ... os error 123`, đồng thời `No exact match for IL2CPP APIs ... partial`. **Dump.cs/script.json chính vẫn được tạo và công cụ exit 0.** Chưa xem các cảnh báo này là đã sửa. Inspector không có exit 0 xác nhận do CLI giữ tiến trình sau khi log `Export finished`; C# stub đã kiểm tra tồn tại và được đối chiếu.

Inspector và dumper cùng ghi nhận type `MapService` `TypeDefIndex 637`, `GameWorld.SubWorld` `2007`, `Network.Header.WORLD_SYNC` `13018`. Inspector source stub có 21.364 nhãn `TypeDefIndex` khác nhau, trong khi dumper có 21.485; **121 nhãn không xuất hiện trong stub Inspector đều là vị trí bắt đầu một image trong danh sách image dump** (122 image headings). Đây là lệch biểu diễn xuất file, **không đủ bằng chứng để nói thiếu 121 type thực tế**.

## 3. Full-client index để tra cứu ngay

Tạo `method-index.sqlite` (286.601.216 B) bằng `tools/reference4016/bulk_index.py` từ `script.json`:

- **414.519** bản ghi phương thức/instantiation.
- **198.707** RVA native dương phân biệt.
- **39.504** nhóm kiểu `assembly/type`, thuộc **120** nhóm assembly.
- Có chỉ mục theo RVA, assembly/type và .NET signature; DB chứa tên/signature proprietary **chỉ giữ ngoài Git**.

Đối chiếu có bằng chứng chính xác:

| Method | RVA đã kiểm tra qua SQL |
|---|---|
| `MapService.Map2WorldPos` | `0x8DFBF0` |
| `MapService.Mps2WorldPos` | `0x8DFD10` |
| `MapService.UpdateWorldInfo` | `0x8E0DD0` |
| `GameWorld.SubWorld.LoadMap` | `0xC75DE0` |
| `Network.Header.ProcessGameUnity.OnSyncWorld` | `0xC38990` |

Tra cứu nhanh (đứng tại `Client-New`):

```powershell
$db = 'E:\Games\VLTK3D\SwordOnline\Tool\_outputs\cn02-bulk\method-index.sqlite'
python tools/reference4016/query_bulk_index.py --database $db --assembly Assembly-CSharp --type MapService --method WorldPos --limit 10
python tools/reference4016/query_bulk_index.py --database $db --rva 0xC75DE0
```

Bộ dữ liệu và log nằm tại `E:\Games\VLTK3D\SwordOnline\Tool\_outputs\cn02-bulk`; **không commit `dump.cs`, `script.json`, `il2cpp.h`, .dll hoặc SQLite**. Repo chỉ lưu tiện ích, báo cáo và JSON tổng hợp không chứa full method signatures.

## 4. Lệnh nguồn đã chạy

Dumper:
```powershell
$tool = 'E:\Games\VLTK3D\SwordOnline\Tool'
$ref = 'E:\Games\VLTK3D\SwordOnline\Client-3D-2\update'
& "$tool\_built\il2cpp-dumper-rs\il2cpp_dumper.exe" --binary "$ref\GameAssembly.dll" --metadata "$ref\Game_Data\il2cpp_data\Metadata\global-metadata.dat" --output "$tool\_outputs\cn02-bulk\dumper" --method-body-mode off --no-generate-dummy-dll --no-il-recovery --no-dump-static-metadata --no-pseudo-prompt
```

InspectorRedux:
```powershell
& "$tool\_built\Il2CppInspectorRedux\Il2CppInspector.Redux.CLI.exe" process "$ref\GameAssembly.dll" "$ref\Game_Data\il2cpp_data\Metadata\global-metadata.dat" --output "$tool\_outputs\cn02-bulk\inspector" --output-csharp-stub
```

Index và kiểm thử:
```powershell
python tools/reference4016/bulk_index.py --source "$tool\_outputs\cn02-bulk\dumper\Dump0\script.json" --database "$tool\_outputs\cn02-bulk\method-index.sqlite" --summary reports/evidence/cn02-4016-il2cpp/bulk-summary.json --binary "$ref\GameAssembly.dll" --metadata "$ref\Game_Data\il2cpp_data\Metadata\global-metadata.dat"
python tools/reference4016/verify_bulk_index.py
```

Indexer cố tình từ chối ghi đè SQLite nếu đã tồn tại: nếu chạy lại, dùng **thư mục/đích mới**, không xóa baseline ngầm.

## 5. Kiểm thử và gate

- `verify_bulk_index.py`: **17/17 PASS**, gồm SHA, số lượng row, code-registration hai công cụ, metadata v39, five RVA/parity anchors, type-index boundary discrepancy, schema SQL, cảnh báo còn tồn tại.
- Chạy lại bảy bộ hồi quy CN02 cũ: **100/100 PASS**. Tổng **117/117 kiểm thử tĩnh PASS**.
- **KHÔNG** khẳng định 161.267 native bodies đã được decompile thành C# chạy lại được; chế độ batch đầu tiên tắt recovery để tạo baseline an toàn.
- CN02-04 **BLOCKED** (chưa có sáu affine runtime Map53 + 3+1 paired anchors), CN03 **CLOSED**.

## 6. Hướng thực thi kế tiếp

**CN02-BULK-02:** ưu tiên xuất Ghidra metadata/code map từ `script.json` và decompile thử **bounded** một nhóm nhỏ `Assembly-CSharp` (World/Map/Network), log CPU/RAM/bộ nhớ và tỷ lệ body thành công/thất bại. Không bật full 414.519 body recovery vô điều kiện. Sau khi chứng minh chất lượng pseudo-code, tăng phạm vi theo batch/chức năng. Kiểm kê AssetRipper read-only riêng và lưu xuất liệu ngoài repo; điều kiện quyền sử dụng asset vẫn giữ nguyên.

**Không cần tiếp tục mổ từng hàm thủ công để tìm type/RVA; dùng SQLite trước rồi mới chọn vùng native cần decompile.**
