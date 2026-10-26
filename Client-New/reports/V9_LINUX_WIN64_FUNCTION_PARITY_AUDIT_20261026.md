# Audit độc lập — Parity hàm Linux JX V9 và GameServer Windows DEV
**Ngày:** 2026-10-26 · **Phạm vi:** khảo sát đọc dữ liệu, KHÔNG sửa server/client, KHÔNG restart.
**Phán quyết:** **NOT COMPLETE / NOT VERIFIED** — tồn tại một số Lua API mang tên V9 nhưng xử lý placeholder/no-op. Chưa thể xác định tổng số hàm Linux còn thiếu theo phương pháp so tên đơn thuần.

## 1. Nguồn được kiểm tra

**Linux V9:** `E:\Games\VLTK3D\SwordOnline\Source-KhamKhao\jxser_v9\server1\jx_linux_y`. ELF 32-bit i386, 8,931,808 bytes, không có ELF section headers (`e_shoff=0`, `e_shnum=0`). Không có bộ mã nguồn C++ Linux tương ứng trong thư mục tham chiếu được khảo sát; không thể liệt kê đầy đủ hàm nội bộ Linux qua .symtab của ELF.

**Windows DEV binary:** `C:\Server-Game\SwordOnline\ServerNew\_bin_v2_\gs\gs001.exe`; chức năng C++ ở `CoreServer.dll` (12,325,888 bytes, modified 2026-10-26 07:07); `CoreServer.map` (modified 2026-10-18, KHÔNG được dùng để chứng nhận chính xác mã DLL biên dịch ngày 26-10). C++ source `C:\Server-Game\SwordOnline\jxOnline\gameserver\Sources\Core\Src`.

**Runtime:** GS PID4020 từng chạy lúc khảo sát sớm, nhưng lúc kiểm tra sau **không tìm thấy tiến trình `gs001.exe`**. Bishop PID3212 và native bridge Python PID3828 có mặt. Không khởi động lại GS; chưa có E2E runtime V9 parity.

**Lua assets:** Linux tham chiếu có 7,872 files dưới `script`, Windows DEV có 11,199; `settings`: Linux 2,856, Win 5,367; con số chỉ là inventory, KHÔNG chứng minh tính tương đương chức năng.

## 2. API registration và giới hạn kỹ thuật

- Nguồn Win: `ScriptFuns_V9_Registration.h` có **1,135 cặp tên/binding**, **1,131 tên duy nhất** theo cú pháp `{"name", LuaHandler}`. Có vài alias/tên lặp. Ghi chú đầu file nói '1125 Tranches1–13' đã lạc hậu so với bảng thực tế gồm Tranche14+.
- Có **28 file** `ScriptFuns_V9_*.cpp` (không tính .h); các nhóm bao gồm Skill, Equipment, Dungeon, CrossServer, Partner, Pet, Timer, AntiBot, Achievement...
- `ScriptFuns.cpp` include `ScriptFuns_V9_Registration.h`. File .map 18-10 có symbol tên các handler kiểm tra; binary CoreServer.dll 26-10 chứa tên Lua binding dạng chuỗi. **Không đồng nghĩa mọi handler đã thực thi đúng.**
- Binary Linux không có bảng section/symtab ELF chuẩn: thiếu cơ sở để tự động nói `1131/XXXX` hay kết luận tất cả hàm khớp 1:1. Cần so sánh entrypoint, lua registration, disassembly và hành vi có kiểm thử.

## 3. Khoảng trống hành vi đã thấy trực tiếp trong Win C++

| Mức | Nhóm | Hàm / nguồn Windows | Bằng chứng |
|---|---|---|---|
| P0 | Phó bản động | `ScriptFuns_V9_Dungeon.cpp:LuaPreApplyDungeonMap` | Khi input hợp lệ vẫn `return 0`; không thực hiện đặt trước map thực |
| P0 | Phó bản động | `LuaApplyDungeonMap` | Chỉ tăng `g_nNextDynamicMapId` và lưu `std::map<int,KDungeonMapInstance>`; không gọi tạo `KSubWorld`/instance runtime trong hàm |
| P0 | Liên máy chủ | `ScriptFuns_V9_CrossServer.cpp:LuaTripC_Apply` | Chỉ `g_PlayerTripState[].bInTrip=1` và lưu tên server, trả 1; không triển khai kết nối/chuyển phiên trong hàm |
| P1 | Đồng hành | `ScriptFuns_V9_PartnerExtended.cpp:LuaPartnerCallout/PartnerBeDrawBack` | Trả 1 khi có player, không gọi tạo/xuất/thu hồi companion |
| P1 | Đồng hành | `LuaPartnerBloodFallQuickly/LuaPartnerKillBoss/LuaPartnerKillPartner` | Đơn thuần `Lua_PushNumber(L,1)`, không đổi trạng thái tương ứng |
| P1 | Trang bị | `ScriptFuns_V9_EquipmentExtended.cpp:LuaInlayStarStone` | Trả 1 khi hai index dương, không làm thao tác inlay lên item/stone |
| P1 | Trang bị | `LuaGetFusionInEquipInfo` | Với input hợp lệ tạo bảng Lua trống; comment xác nhận thiếu carrier cho dữ liệu fusion |
| P1 | Skill | `ScriptFuns_V9_SkillsExtended.cpp:LuaGetSkillCount/LuaGetTotalSkill/LuaGetAllSkillCurInc` | Với player hợp lệ vẫn luôn push 0 |
| Đã cải tiến | Skill | `LuaGetSkillMaxLevel` | Đọc `g_SkillManager.GetSkillMaxLevel` thay hằng 20; đây là ví dụ hàm đã có xử lý thật hơn |

**Linux script evidence:** `script/activitysys/config/48/extend.lua:121` gọi `GetSkillCount(1)`; một script NPC gọi `GetTotalSkill()`; `script/event/equip_publish/mingfeng/refine_equip.lua:55` và `tenglong/item2equip.lua:304` đọc `GetFusionInEquipInfo` bằng `next(...)`. Bộ Lua tham chiếu kỳ vọng dữ liệu có ý nghĩa. Những callsite chứng minh sự phụ thuộc, không tự thân chứng minh chính xác hành vi native ELF.

**Lưu ý:** Các vấn đề `NPC/AOI bridge`, `Lua login/logout`, `CN02 collision` là những blocker riêng của pipeline client mới; không nên tự động gán chúng là 'hàm thiếu của CoreServer' hoặc suy ra Linux V9 chắc chắn vận hành tốt.

## 4. Đề xuất tiếp theo

1. **Chốt GS runtime/build provenance:** khôi phục khả năng chạy GS DEV trong khung kiểm thử (chỉ sau khi xác minh không có hoạt động Antigravity), lưu hash/PE-PDB GUID để đối chiếu source đúng binary.
2. **Audit tất cả binding** từ `ScriptFuns_V9_Registration.h`, phân nhóm REAL / PARTIAL / NO-OP / UNVERIFIED, kèm call graph state changes, negative tests, Lua return shape. Không chỉ grep tên.
3. **Đối chiếu bản Linux bằng static evidence an toàn:** xác minh binary `jx_linux_y` ELF stripped; extract dynamic symbols/strings/call references khi khả thi; dùng scripts tham chiếu để suy ra API contract; không công bố tỷ lệ 100% nếu không có chứng cứ.
4. **Ưu tiên sửa chức năng theo mức độ ảnh hưởng:** dynamic dungeon, chuyển server (yêu cầu protocol/state/session), item/inlay/fusion, partner, skill count. Với tác vụ cấp phát, giao dịch, inventory, đảm bảo server authoritative và SQL/persistence; không thay fake-success bằng thao tác nửa chừng.
5. **Test trên QA data trước khi deploy**; không sửa character DB thật, không bypass validation, backup/rollback từng module. Chỉ chốt parity khi test semantic và runtime Linux/Win (nếu có môi trường Linux test) đạt.
6. Giữ `CN02-04 BLOCKED`, `CN03 CLOSED`. Map53 global collision pilot vẫn là nhánh việc riêng Antigravity đang làm; không overwrite uncommitted files.

## 5. Kết luận

**Có nhiều nhóm API V9 đã được port về Windows, nhưng chưa đạt parity chức năng.** Đã phát hiện ít nhất các trường hợp thiếu xử lý thực tế trong dungeon, liên máy chủ, đồng hành, trang bị và kỹ năng. **Không thể công bố danh sách 'toàn bộ hàm Linux còn thiếu' hay % hoàn thành chính xác** chỉ dựa trên các thông tin này. Cần audit chức năng theo API contract và bổ sung kiểm thử.
