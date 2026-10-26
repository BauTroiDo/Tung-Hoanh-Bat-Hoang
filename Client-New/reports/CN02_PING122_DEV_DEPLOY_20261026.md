# CN02 — Đã triển khai bản sửa gateway ping 122 trên DEV

**Ngày:** 26/10/2026, khoảng 11:42–11:55 giờ Việt Nam.
**Máy server:** WIN-33R6OP5GO1M. **Máy client QA:** DESKTOP-TPG2RNF.

## Tình trạng sau triển khai

**Gateway đã được restart riêng với bản sửa** (không restart Bishop/gs001, không sửa database).

- Gateway mới: PID **6324**, cổng **14180**, `--native-gs-bridge`, allowlist `192.168.1.13`.
- Bishop: PID **3212**, cổng 56722 tiếp tục hoạt động.
- GameServer: PID **4168**, cổng 51189 tiếp tục hoạt động.
- Trước khi restart: **0 kết nối ESTABLISHED** tại cổng gateway 14180.
- Native client QA đã thử greeting socket sau restart: đúng **34 byte**, prefix `22 00 20 00`. Không gửi credential hoặc lệnh nhân vật.
- Gateway post-deploy log: `C:\Server-Game\SwordOnline\Client3D-LAN\qa_runs\cn02-ping122-fix-20261026-1142\gateway-pingfix.log` xác nhận listening và native greeting; đóng bình thường sau phép thử.

## Nguyên nhân xác nhận và bản sửa

Bản cũ nhận native opcode 82, **5 byte**, rồi gửi sang original GS `b'\x7a' + clear[1:]`, chỉ **5 byte**. GameServer C++ yêu cầu opcode 122 `PING_CLIENTREPLY_COMMAND` **9 byte**, gồm 1 byte mã, uint32 timestamp server và uint32 timestamp client; còn kiểm tra timestamp server bằng giá trị cuối cùng đã gửi, nên thêm số 0 tùy tiện có nguy cơ ngắt kết nối.

Bản mới thêm `NativePingPair` trong `gs-bridge-m1/native_session.py`:

1. Khi nhận **GS S2C opcode 143** `PING_COMMAND`, 5 byte, lưu timestamp do server thực sự gửi, không chuyển raw sang client.
2. Khi nhận **native client opcode 82**, 5 byte, chỉ khi có pending server timestamp mới tạo gói **122, 9 byte**, giữ nguyên timestamp server và client theo cặp.
3. Không gửi ping 122 giả khi chưa có timestamp server; không gửi trùng response cho cùng một token.
4. GS phản hồi **S2C opcode 174** sẽ được chuyển về native opcode 131 như luồng hiện hữu để client nhận echo của timestamp.

**73/73 kiểm thử có mục tiêu PASS** cho ping, handshake, native relay, Map53/world scalar projection, region, GS preflight và các điều kiện sai. File test mới nằm tại `gs-bridge-m1/test_native_ping_pair.py`, có bản sao trong thư mục QA.

Đã kiểm tra fail-closed: sau sửa mã nhưng trước khi đổi SHA pin, `native_p0_readiness.evaluate()` báo `P0_PROTOCOL_EVIDENCE_MISSING_OR_STALE` do file nguồn thay đổi. Sau khi cập nhật **đúng một source hash** tương ứng và chạy test, gate trả `CONTROLLED_MAP53_PROTOCOL_TEST_READY` / `world_entry_enabled=true`, `m1_p0_pass=false`. **Không** coi trạng thái này là gameplay runtime PASS.

## Backup + rollback

Backup **trước triển khai**, bên ngoài Git:

`C:\Server-Game\SwordOnline\Client3D-LAN\qa_runs\cn02-ping122-fix-20261026-1142\`

- `native_session.py` cũ, SHA256 `D0EF2791F4B3F3CF2DE4AA0521E13C9D63A857D6314F5A7FB23F5DE0D7393B8C`.
- `native-p0-readiness.json` cũ, SHA256 `E26C1F9C6498232E3C488056CF101A16C9DCF7F7C24AA7B20569949B6BA8E9EE`.
- Bản mới `native_session.deployed.py`, SHA256 `715208D04F731CBD12DDF1C20F60C348D80CD34C57FE876AF895844A9BA3CD34`.
- Manifest mới `native-p0-readiness.deployed.json`, SHA256 `BBA60702EA27C6EB38D30F4E89212B93880B9C8DBAF24DE24E024686154F5A37`.
- Bộ test mới SHA256 `1CE4D18E7EC62349A47B37EE1F7712AE864ACBD7702AF73ABA1037A5D03462DD`.
- Lưu `gateway.pid.before-restart`, `gateway-new-pid.txt`, log mới riêng.

Rollback chỉ thực hiện khi không có phiên game đang hoạt động: dừng gateway PID đang hiện hành, copy **cả hai** bản backup `native_session.py` và `native-p0-readiness.json` trở lại `gs-bridge-m1`, xác minh readiness, khởi động lại gateway đúng với `--native-gs-bridge --allow 192.168.1.13`. Không dùng `Manage-Gateway.ps1 -Action restart` vì lệnh mặc định không bật native GS bridge. Không restart gs001/Bishop để rollback gateway.

## Cần kiểm thử trực tiếp tiếp theo

**Chưa có phiên đăng nhập/chơi lại sau khi patch ping**. Chỉ handshake và kiểm thử nguồn đã PASS, vì vậy **chưa thể khẳng định hết cảnh báo opcode122 trong runtime**. Cần một nhân vật QA đăng nhập mới, xác nhận:
- Bishop cấp permit; GS native sync ACK.
- GS S2C143 → native C2S82 → legacy C2S122 đủ 9 byte → GS S2C174 đúng echo; không có cảnh báo 9 vs 5 hoặc đóng phiên do timestamp sai.
- Còn giới hạn session 90 giây, 4096 frame/8MiB trong M1; đây là giới hạn thiết kế QA chưa được mở rộng. Không được tuyên bố đã sửa session disconnect chỉ từ ping.
- Scene Ba Lăng Huyện, NPC/AOI và di chuyển vẫn là **ba vấn đề riêng** chưa nghiệm thu.

## Lỗi Lua đăng nhập/đăng xuất

Log GS có `attempt to call a nil value` khi gọi OnLine/main/OnLogout. Rà soát trực tiếp thư mục `_bin_v2_\gs\script`, `vng_script` và các source `jxOnline\gameserver` liên quan: không tìm thấy loose `script/system/online.lua`, `script/system/player/playerlogin.lua`, `playerlogout.lua`; chỉ có script `activitysys/detailtype/playerlogin.lua` **khác chức năng**. `pak/script.pak` còn tồn tại, nhưng nguồn script runtime và traceback chính xác của callee `nil` chưa xác định. **Không thêm stub, không sửa script trong PAK, không sửa cơ chế lưu nhân vật** khi chưa có bằng chứng. Trạng thái Lua: **OPEN**.

**Tổng kết:** Gateway code + restart **DEPLOYED ON DEV**, focused unit **73/73 PASS**, native TCP greeting **PASS**; **end-to-end ping PENDING QA**, Lua nil **OPEN**, Map53 terrain/NPC **BLOCKED**, CN02-04 **BLOCKED**, CN03 **CLOSED**.
