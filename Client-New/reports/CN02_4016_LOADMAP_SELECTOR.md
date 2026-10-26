# CN02-04 — Phục hồi bộ chọn Map / Template / Scene trong client 4016

**Ngày:** 2026-10-25 (Asia/Ho_Chi_Minh). **Branch:** `cg/cn02-anchor-evidence`. **Checkpoint trước:** `e79746b`.

**Kết luận phạm vi:** Đã xác định bằng mã máy và metadata gốc rằng **Template ID**, không phải **SubWorld ID**, điều khiển lời gọi nạp gói map và chọn scene của client 4016. **Không** xác minh asset key scene Map 53 đã tồn tại/tải thành công, các hệ số tọa độ đang dùng hoặc nghiệm thu runtime.

## 1. Tệp nguồn và giới hạn thao tác

- Chỉ đọc `Client-3D-2/update/GameAssembly.dll` (SHA256 `011357720B210AA1F392A8B62FF8DE090D3A895230A6E0135BCE707196871069`) và `global-metadata.dat` (SHA256 `5586F416AA26EBE57EA7F6580D0A53E5E5F29C50E425E64A4D3F2A95D6069E08`).
- Không chạy, patch hoặc sửa client 4016; không sao chép asset/binary vào repository. Không chỉnh UnityProject, scene, Bishop/GameServer, gateway, DB hoặc nhân vật.
- Phân tích tĩnh bằng `pefile`, `capstone`, giới hạn vào PE64 unwind entries và ánh xạ MethodDef đã kiểm tra theo `Assembly-CSharp.dll`.

## 2. Phát hiện quan trọng — thân hàm LoadMap có hai đoạn

Bản phân tích trước chỉ nhận ra `SubWorld.LoadMap` tại RVA `0xC75DE0` với 485 byte đầu. Hàm có **hai** entry liền nhau trong `.pdata`, không có phương thức khác được đăng ký xen vào:

| Entry | RVA bắt đầu | RVA kết thúc (exclusive) | Dung lượng | Lệnh đã giải mã |
|---|---|---|---:|---:|
| Đoạn 1 | `0xC75DE0` | `0xC75FC5` | 485 byte | 92 |
| Đoạn 2 | `0xC75FC5` | `0xC790A4` | 12.511 byte | 2.833 |

Tổng cộng 12.996 byte và 2.925 lệnh có thể giải mã trong hai vùng. Hai vùng cùng thuộc một phương thức theo bằng chứng con trỏ CodeGenModule và tiếp nối điều khiển; **không** phải hai API riêng.

## 3. Phân biệt chính xác SubWorld ID và Template ID

`Network.Header.WORLD_SYNC` có các trường đã xác nhận ở checkpoint trước. Luồng gọi thực tế:

```text
WORLD_SYNC.MapCopyID (+0x14) ---> LoadMap(nTemplateId) ---> edi
WORLD_SYNC.SubWorld  (+0x04) ---> LoadMap(nMapId)      ---> r14d
WORLD_SYNC.Region    (+0x08) ---> LoadMap(nRegion)     ---> ebp
                                   nMpsX = 0, nMpsY = 0
```

Hai lệnh ghi vào instance `GameWorld.SubWorld` trong LoadMap:

| RVA lệnh ghi | Bên nhận | Nguồn | Field gốc xác nhận |
|---|---|---|---|
| `0xC7628F` | `[rsi+0x1C]` | `r14d` — `nMapId` | `SubWorldID` |
| `0xC76326` | `[rsi+0x20]` | `edi` — `nTemplateId` | `MapTemplateID` |

Đối chiếu **độc lập** bốn accessor IL2CPP gốc:

- `SubWorld.get_SubWorldID` (`0x96AE10`) đọc `[rcx+0x1C]`; setter `0xA7DC10` ghi vào offset đó.
- `SubWorld.get_MapTemplateID` (`0x82CAA0`) đọc `[rcx+0x20]`; setter `0x7E7630` ghi vào offset đó.

## 4. Chọn dữ liệu và scene bằng Template ID

Các lời gọi quan trọng trong phần thân LoadMap được xác nhận cả địa chỉ codegen lẫn tham số metadata:

| RVA call | MethodDef gốc | Tham số gốc | Nguồn trong LoadMap |
|---|---|---|---|
| `0xC76170` | `NPCManager.OnMapTemplateChanged` | `newTemplateId` | `edx = edi = nTemplateId` |
| `0xC762AC` | `MapDataService.LoadForTemplate` | `templateId` | `ecx = edi = nTemplateId` |
| `0xC7638A` | `GameManager.LoadScene` | `mapId` | `edx = edi = nTemplateId` |

**Chú ý tên gọi:** chữ `mapId` trong chữ ký `GameManager.LoadScene(mapId)` không có nghĩa nguồn là `WORLD_SYNC.SubWorld`. Lệnh máy tại điểm gọi **truyền `nTemplateId` / `MapCopyID`** vào API này.

Luồng còn bao gồm `MapDataService.UnloadPack`, `MapDataService.ResetTemplateTracking`, các thao tác làm sạch NPC/items/missile/VFX trước khi nạp mới, rồi `SubWorld.FindRegion`, `KRegion.Load`, `KRegion.Init`. Các lời gọi trùng tên có thể xuất hiện trên nhiều nhánh.

Có phân tích thêm một bước vào `MapDataService.LoadForTemplate` (458 byte, 83 lệnh) và `GameManager.LoadScene` (189 byte, 41 lệnh). Từ dữ liệu tĩnh hiện tại, chưa đối chiếu đầy đủ được **scene key**, đường dẫn Addressables/bundle hay kết quả tải thực tế; không được tuyên bố Map53 đã được render.

## 5. Ý nghĩa với Map 53 và Client-New

Log test harness lịch sử ghi `SubWorld=53`, `MapCopyID=53`, `Region=6488114`, vì thế hai loại ID **trùng giá trị** trong ví dụ này. Bộ kiểm thử âm sử dụng cặp giả lập khác nhau chỉ để chống nhầm dữ liệu; không xem đó là bằng chứng runtime.

Điều phải giữ khi xây dựng client có source:

- Duy trì hai trường độc lập **SubWorld ID** và **Map Template ID**; không ép chúng bằng nhau.
- Nạp map data, chọn scene và gửi tín hiệu thay đổi template theo **Template ID**.
- Quản lý instance thế giới theo **SubWorld ID**; `Region` được chuyển riêng.
- Không gắn các tham số mặc định MPS=0 từ `LoadMap` thành tọa độ spawn thực sự; yêu cầu đo trong runtime riêng.
- Không sao chép trực tiếp hệ số affine MapService khởi tạo sang scene tổng hợp Client-New.

## 6. Evidence và nghiệm thu

- Công cụ mới: `tools/reference4016/il2cpp/loadmap_selector.py`.
- Kiểm thử: `tools/reference4016/il2cpp/verify_loadmap_selector.py`.
- Bằng chứng JSON: `reports/evidence/cn02-4016-il2cpp/loadmap-selector.json`.

Chạy trên máy Windows tham chiếu:

```powershell
python tools/reference4016/il2cpp/loadmap_selector.py --binary "E:\Games\VLTK3D\SwordOnline\Client-3D-2\update\GameAssembly.dll" --metadata "E:\Games\VLTK3D\SwordOnline\Client-3D-2\update\Game_Data\il2cpp_data\Metadata\global-metadata.dat" --module reports/evidence/cn02-4016-il2cpp/pe-module-candidates.json --worldsync reports/evidence/cn02-4016-il2cpp/worldsync-contract.json --out reports/evidence/cn02-4016-il2cpp/loadmap-selector.json
python tools/reference4016/il2cpp/verify_loadmap_selector.py
```

Bộ kiểm thử mới **19/19 PASS**; năm bộ kiểm thử tĩnh từ các bước trước cũng được chạy lại **19/19 + 14/14 + 11/11 + 10/10 + 10/10 PASS**. Tổng **83/83 PASS**, tất cả tiến trình kết thúc với exit 0. Đây là kiểm thử phân tích tĩnh **không phải** Unity PlayMode hoặc nghiệm thu gameplay.

**Bước kế tiếp:** truy `GameManager.LoadScene` tới nguồn tạo scene key/Addressables và `MapDataService.LoadForTemplate` tới map-pack manifest; sau đó đối chiếu scene Map53 và 3+1 điểm mốc độc lập. **CN02-04 BLOCKED; CN03 CLOSED.**
