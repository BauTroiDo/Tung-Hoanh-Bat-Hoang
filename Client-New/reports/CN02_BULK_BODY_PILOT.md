# CN02-BULK-02 — Kiểm định phục hồi thân hàm World/Map/Network theo lô

**Ngày:** 26/10/2026 (Asia/Ho_Chi_Minh). **Máy:** DESKTOP-TPG2RNF. **Branch:** cg/cn02-anchor-evidence. **Baseline:** commit 1a6b250, CN02-BULK-01.
**Nghiệm thu:** PASS cho cơ chế lọc type, disassembly native bounded, cross-check với PE64; **chưa nghiệm thu tính chính xác/ngữ nghĩa của pseudo-code**.

## 1. Phạm vi và bằng chứng

Dữ liệu nguồn chỉ đọc: `E:\Games\VLTK3D\SwordOnline\Client-3D-2\update\GameAssembly.dll` và `Game_Data\il2cpp_data\Metadata\global-metadata.dat`, v39 Unity 6000.3.9f1, SHA256 lần lượt:

- `011357720B210AA1F392A8B62FF8DE090D3A895230A6E0135BCE707196871069`
- `5586F416AA26EBE57EA7F6580D0A53E5E5F29C50E425E64A4D3F2A95D6069E08`

3 nhóm loại được chọn: `MapService`, `GameWorld.SubWorld`, `Network.Header.ProcessGameUnity`; bộ lọc mở rộng sang 3 lớp nested do quan hệ nested, tổng sáu TypeDefIndex `637, 1989, 1990, 1991, 1992, 2007`.

| Kết quả type-only dump (il2cpp-dumper-rs 8.0.0) | Giá trị |
|---|---:|
| Command exit | **0** |
| Elapsed | **11,91 giây** |
| File `dump.cs` | **5.226.635 byte** |
| Bản ghi thân hàm có cả disassembly và C-like pseudo | **295/295** |
| RVA native phân biệt | **272** |
| Bindings trùng RVA do nhiều logical methods | **23** |
| Tổng instruction counts được báo cho các bản ghi | **33.764** (gồm RVA trùng) |
| Tổng bytes disassembly được báo | **145.714** (gồm RVA trùng) |
| Thân hàm có `[recovery: suspect]` | **14** |
| Thân hàm có `__unknown` | **5** |

**Không được coi 295 marker là 295 hàm đã phục hồi đúng ngữ nghĩa.** Mỗi marker chỉ chứng minh bộ công cụ đã xuất một phần C-like pseudo, có thể chưa đầy đủ, sai dữ liệu kiểu, hoặc chưa giải đúng điều kiện.

### Phát hiện filter OR quan trọng

Lượt chạy ban đầu đã truyền cả `--pseudo-include-assembly Assembly-CSharp` và `--pseudo-include-type`. Kiểm tra code của dumper tại `src/output/pseudo_c.rs:386` xác nhận **`hit_asm || hit_type`**, nên đây là phép hợp (OR), không phải giao (AND). Lượt đầu vô tình chọn rộng cả Assembly-CSharp và tạo `dump.cs` **192.911.813 byte**, trong 29,96 giây.

Đã sửa cách chạy thành **chỉ truyền ba `--pseudo-include-type`**. Công cụ `tools/reference4016/run_body_pilot.py` ngăn gửi assembly-filter, từ chối type filter rỗng/trùng, giới hạn 16.384 body bytes và timeout tối đa 600 giây, so SHA256 input, từ chối ghi đè và **chỉ cho output ngoài Git** dưới `Tool/_outputs/cn02-bulk/`.

## 2. Đối chiếu native/Ghidra Headless 12.2 chính thức

Sau khi phát hiện Ghidra `-noanalysis` tạo function chỉ gồm một địa chỉ, đã cải tiến `tools/reference4016/ghidra/CN02BodyPilot.java`:
- Xác định trước vùng native dựa trên PE64 `.pdata` có bằng chứng; riêng `LoadMap` gồm **hai unwind frames liên tiếp**.
- Luôn disassemble trong `AddressSet` bounded bằng `DisassembleCommand`, kể cả khi importer tạo sẵn Function.
- Thu thập instruction addresses thật vào `AddressSet`; cập nhật function body trước khi gọi decompiler.
- Chỉ ghi `RVA / decoded / nativeSpan / coveredBytes / status / chars` vào log, không xuất mã decompile vào Git.
- Chạy Ghidra với `-noanalysis -readOnly -deleteProject`, dữ liệu bên ngoài Git.

| Method (RVA) | Bounded bytes .pdata | Ghidra decoded/body | Ghidra |
|---|---:|---:|---|
| `MapService.Mps2WorldPos` `0x8DFD10` | 239 | **239/239** | DECOMPILED |
| `MapService.Map2WorldPos` `0x8DFBF0` | 276 | **276/276** | DECOMPILED |
| `MapService.UpdateWorldInfo` `0x8E0DD0` | 1.032 | **1.032/1.032** | DECOMPILED |
| `GameWorld.SubWorld.LoadMap` `0xC75DE0` | 12.996 | **12.996/12.996** | DECOMPILED |
| `Network.Header.ProcessGameUnity.OnSyncWorld` `0xC38990` | 1.112 | **1.112/1.112** | DECOMPILED |

Ghidra headless exit **0**, 43,97 giây; Ghidra ghi `Discarded file import due to readOnly option`; không ghi đè source. Decompiler trả về C-like pseudo không rỗng cho 5/5 RVA. **Lưu ý:** decoded bytes/function body coverage + return status không chứng minh 100% logic được phục hồi đúng; type, gọi gián tiếp, xử lý metadata và side effect vẫn phải thẩm định trước khi đưa vào C# mới.

Dumper ở giới hạn 8.192 chỉ xuất cho `LoadMap` **512 lệnh / 2.335 byte** trong khi bằng chứng unwind gốc ghi **2.925 lệnh / 12.996 byte**. Ghidra đã đi hết byte span được chỉ định; đây là lý do các hàm lớn cần đường phân tích Ghidra riêng.

## 3. Công cụ, lệnh tái lập và nơi lưu raw output

Gốc công cụ `E:\Games\VLTK3D\SwordOnline\Tool\_built`. Raw outputs chỉ có tại:

- `Tool/_outputs/cn02-bulk/body-pilot/type-only/Dump0/dump.cs` (lượt type-only đúng).
- `Tool/_outputs/cn02-bulk/body-pilot/Dump0/dump.cs` (lượt quá rộng để làm chứng cứ lỗi filter, **không dùng làm đầu vào tiếp**).
- `Tool/_outputs/cn02-bulk/ghidra-pilot-v3/console.log` (Ghidra bounded 5 RVAs; log UTF-16 do PowerShell redirection).
- `Client-New/reports/evidence/cn02-4016-il2cpp/body-pilot-summary.json` — thống kê thu gọn, không chứa raw pseudo/body.

Cách chạy phiên sau ở `Client-New`:

```powershell
$dst = 'E:\Games\VLTK3D\SwordOnline\Tool\_outputs\cn02-bulk\next-body-batch'
python tools\reference4016\run_body_pilot.py --output $dst --type MapService --type GameWorld.SubWorld --type Network.Header.ProcessGameUnity --max-body-size 8192 --timeout 180
python tools\reference4016\verify_body_pilot.py
```

**Chú ý:** wrapper từ chối output đã tồn tại; để tái lập, dùng một output path mới. `verify_body_pilot.py` hiện so trực tiếp *baseline pilot cụ thể* bên ngoài repo, không tự đổi sang thư mục batch mới.

Ghidra: `Ghidra/ghidra_12.2_PUBLIC/support/analyzeHeadless.bat` dưới Tool/_built, `-import GameAssembly.dll -noanalysis -postScript CN02BodyPilot.java -scriptPath Client-New/tools/reference4016/ghidra -readOnly -deleteProject`. Script hiện có tập 5 RVA đã xác minh và các length PE .pdata cố định; **chưa phải engine tự động tổng quát cho mọi RVA**.

## 4. Kiểm thử / checkpoint

- **22/22** kiểm thử mới `verify_body_pilot.py` PASS: cấu trúc pseudo/disasm, số lượng và aliases, SHA, đối chiếu byte opcode đầu với GameAssembly gốc, phân nhóm, suspect/unknown, LoadMap coverage, Ghidra 5 spans đầy đủ, readOnly, công cụ wrapper không lọc assembly hoặc ghi output vào repo.
- **17/17** test CN02-BULK-01 PASS; **100/100** test CN02 phân tích cũ PASS. Tổng **139/139 static checks PASS**.
- Dữ liệu proprietary `dump.cs`, `script.json`, SQLite, headers và pseudo-output không commit và không đẩy GitHub.
- Các thay đổi đồng thời trong `UnityProject/Assets/_JXClient/**/Materials/*.mat` thuộc luồng khác: không stage, không đụng.
- CN02-04 **BLOCKED** và CN03 **CLOSED**: chưa có affine Map53 runtime và 3+1 landmark nghiệm thu.

## 5. Hướng CN02-BULK-03

Tự động tạo batch manifest từ SQLite `Assembly-CSharp` theo nhóm Movement, NPC, Combat, Navigation; chỉ dùng các type có số RVA/dung lượng native bounded. Chạy type-only dumper trên nhiều type nhỏ theo nhóm; Ghidra lấy native spans tự động từ `.pdata` thay vì hardcode năm RVA, dùng log thống kê `decodedBytes/status/suspect` để phân loại. Không nâng kết quả pseudo-code thành mã gameplay khi chưa qua parity/test server-client.
