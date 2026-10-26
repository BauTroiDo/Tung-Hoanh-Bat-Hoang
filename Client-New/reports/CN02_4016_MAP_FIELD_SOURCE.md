# CN02-04 — 4016 MapService field source and default calibration values

**Ngày:** 2026-10-25, giờ Việt Nam. **Branch:** `cg/cn02-anchor-evidence`. **Baseline:** `9ab0f74`.

## Tình trạng và ranh giới

Hoàn tất phân tích tĩnh vị trí khởi tạo và tên trường gốc của phép biến đổi MPS → Unity. Kết quả là bằng chứng **giá trị mặc định trong chương trình 4016**, **chưa phải** giá trị xác minh khi Map 53 đang chạy. CN02-04 = BLOCKED / NOT VERIFIED; CN03 = CLOSED.

Chỉ đọc hai tệp của client gốc:
- `Client-3D-2/update/Game_Data/il2cpp_data/Metadata/global-metadata.dat` — SHA256 `5586F416AA26EBE57EA7F6580D0A53E5E5F29C50E425E64A4D3F2A95D6069E08`
- `Client-3D-2/update/GameAssembly.dll` — SHA256 `011357720B210AA1F392A8B62FF8DE090D3A895230A6E0135BCE707196871069`

Không chạy client/DLL, không kết nối server, không thay đổi UnityProject, scene 4016, dữ liệu tài khoản hay asset.

## Trường gốc từ metadata v39

`MapService` có `type index 637`, các field index `5555..5587` (33 trường). Sáu trường đầu được đọc tại metadata FieldDefinition section 11, có tên gốc:

| Field index | Tên trường C# | Offset static | Hex float32 khởi tạo | Giá trị gần đúng |
|---:|---|---:|---|---:|
| 5555 | `m00` | +0x00 | `C01A0275` | -2.4064 |
| 5556 | `m01` | +0x04 | `40656042` | 3.5840 |
| 5557 | `m10` | +0x08 | `4079096C` | 3.8912 |
| 5558 | `m11` | +0x0C | `4054FDF4` | 3.3280 |
| 5559 | `offsetX` | +0x10 | `C361851F` | -225.52 |
| 5560 | `offsetZ` | +0x14 | `C4B33AE1` | -1433.84 |

Sáu lệnh ghi mặc định được nhận diện trong `MapService..cctor` (RVA `0x8E1B40`) tại các RVA `0x8E1BB5, 0x8E1BC9, 0x8E1BDE, 0x8E1BF3, 0x8E1C08, 0x8E1C1D`. Giá trị giải mã theo IEEE-754 single precision. Field khác của lớp: `_invDet`, `_mpsWorldScale`, `CurrentSubworld`.

Công thức **mặc định** của `MapService.Mps2WorldPos` khi sáu trường chưa bị đổi:

```text
u = MpsX / 512
v = MpsY / 512
UnityX = m00*u + m01*v + offsetX
UnityY = suppliedHeight
UnityZ = m10*u + m11*v + offsetZ
```

Giá trị bốn tile scene 4016 từ báo cáo trước không phải tọa độ MPS tương ứng, nên không thể dùng chúng để tuyên bố sáu giá trị mặc định đã khớp với địa hình Map 53.

## Luồng ghi đè và nguồn dữ liệu chưa tìm được

- `MapService.Calibrate`, native RVA `0x8DD750`, chuẩn bị hai hệ 3×3, gọi `MapService.Solve3x3` tại `0x8E0870` **hai lần** (lệnh gọi tại RVA `0x8DD904`, `0x8DD99E`). Kết quả có thể ghi đè sáu trường đầu.
- Quét các section thực thi tìm lệnh direct `CALL rel32` tới `0x8DD750` thấy **0 candidate**. Kết quả này **không** loại trừ delegate, reflection, indirect call hay cơ chế gán dữ liệu khác. Không được suy ra hàm Calibrate không bao giờ chạy.
- `MapService.UpdateWorldInfo` có xử lý ngữ cảnh SubWorld và các trạng thái bên trong. Chưa xác minh được toàn bộ đường dữ liệu hay các giá trị runtime theo MapId=53.
- Dữ liệu 4016 đang kiểm tra là giá trị ở **khởi tạo lớp**. Chưa có snapshot runtime Map 53 hoặc bằng chứng độc lập bốn điểm tương ứng legacy ↔ Unity.

## Công cụ và nghiệm thu

Đã mở rộng `tools/reference4016/il2cpp/map_field_source.py` để tái lập liên kết tên FieldDefinition → cấu trúc field → lệnh ghi bit float32 tại `.cctor`; đồng thời ghi dấu hai lời gọi `Solve3x3` và kết quả quét direct caller giới hạn. Output:

`reports/evidence/cn02-4016-il2cpp/map-field-source.json`

Lệnh đã chạy trên máy Windows của dự án:

```powershell
python tools/reference4016/il2cpp/map_field_source.py --metadata "E:\Games\VLTK3D\SwordOnline\Client-3D-2\update\Game_Data\il2cpp_data\Metadata\global-metadata.dat" --binary "E:\Games\VLTK3D\SwordOnline\Client-3D-2\update\GameAssembly.dll" --module reports/evidence/cn02-4016-il2cpp/pe-module-candidates.json --out reports/evidence/cn02-4016-il2cpp/map-field-source.json --scan-callers
python tools/reference4016/il2cpp/verify_map_field_source.py
```

**Kết quả:** bộ kiểm thử mới 10/10 PASS, hai bộ hồi quy đã có cũng được chạy lại (11/11 và 10/10): **31/31 kiểm thử tĩnh/fixture PASS**, cả ba process exit 0. Đây **không** phải Unity PlayMode hoặc nghiệm thu Map53. Không chạy lại Unity vì không sửa mã dự án Unity.

## Bước tiếp theo cụ thể

Kiểm tra các call gián tiếp/delegate tới Calibrate và nguồn cung cấp hệ số theo map; xác minh sáu giá trị khi client 4016 thực sự ở Map53 bằng bằng chứng hợp lệ. Sau đó đối chiếu 3 điểm không thẳng hàng với Unity world positions của **cùng landmark**, giữ lại điểm thứ tư để đánh giá holdout. Chỉ khi được review độc lập mới đưa phép đổi tọa độ vào Client-New và tiến hành ground/collision tests.

Cấm copy asset gốc chưa xác nhận quyền sử dụng. Giữ nguyên runtime/server, không tự mở CN03.
