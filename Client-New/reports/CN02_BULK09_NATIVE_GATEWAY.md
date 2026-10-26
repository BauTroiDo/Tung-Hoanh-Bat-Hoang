# CN02-BULK-09 — Gateway native đã bật, client 4016 chạy QA, Bishop từ chối GameServer permit

**Ngày:** 26/10/2026 (UTC+7). **Branch:** `cg/cn02-anchor-evidence`. **Baseline:** `cdef6da`.
**Trạng thái:** Gateway native và handshake **PASS**; Bishop xác thực **đã quan sát thành công**; Bishop cho phép vào GameServer **FAIL**; scene và movement **NOT VERIFIED**. CN02-04 **BLOCKED**, CN03 **CLOSED**.

## 1. Bảo vệ môi trường và nguồn bằng chứng

- Máy client QA: `DESKTOP-TPG2RNF`, IPv4 `192.168.1.13`.
- Máy server: `WIN-33R6OP5GO1M`, IPv4 `192.168.1.69`.
- Trước khi bật gateway: Bishop `Bishop_20261022.exe` PID **3212**, port `56722`; GameServer `gs001.exe` PID **1608**, port `51189`; port gateway `14180` chưa mở; không có gateway process/phiên client khác trên cổng.
- Bộ `native_p0_readiness.evaluate()` trên server: `CONTROLLED_MAP53_PROTOCOL_TEST_READY`, `world_entry_enabled=True`, source hash không lệch. **51/51 unit tests native handshake/relay/Map53 selector PASS**, exit 0. Đây là unit tests cục bộ, không được xem là runtime acceptance.
- Nguyên `Game.exe` SHA256 `1CAE3982C04BE9D6F8F7EA8CE272600400655D40FBF0671818EFF1D7A79750EE`, `GameAssembly.dll` SHA256 `011357720B210AA1F392A8B62FF8DE090D3A895230A6E0135BCE707196871069`; metadata v39 gốc SHA256 `5586F416AA26EBE57EA7F6580D0A53E5E5F29C50E425E64A4D3F2A95D6069E08` vẫn còn nguyên sau khi chạy client. Không restart/chỉnh Bishop, gs001, DB, gateway source, original game binaries, PAK hay `UnityProject/**`.

## 2. Gateway được kích hoạt đúng native mode

Gateway được khởi động có điều kiện, chỉ khi cổng trống, Bishop/GS đang lắng nghe và readiness/hashes PASS, bằng Python của server cùng source gateway.py không thay đổi:

```text
python.exe -u gateway.py --allow 192.168.1.13 --native-gs-bridge --log-file <qa-run-dir>\gateway.log
```

Kết quả: gateway **PID 7600**, port `192.168.1.69:14180`, cờ `--native-gs-bridge` thực tế được kiểm tra từ command line, allowlist **chỉ** `192.168.1.13`. PID được đưa vào `Client3D-LAN/gateway.pid` để script quản trị `Manage-Gateway.ps1 -Action status/stop` nhận diện đúng tiến trình; PID cũ được giữ riêng trong QA log folder.

Thư mục bằng chứng riêng trên server:
`C:\Server-Game\SwordOnline\Client3D-LAN\qa_runs\cn02-bulk09`

Gồm `gateway.log`, `gateway-pid.txt`, `gateway.pid.before.txt`, `sanitized-gateway-snapshot.json` và hướng dẫn dừng đúng process. Snapshot lúc **06:06:26+07:00** xác nhận gateway listener/PID đúng, manager trả `RUNNING PID=7600`.

## 3. Kết nối thực từ máy client

Kiểm tra TCP trên chính máy client: gateway gửi **34 byte native greeting**, LE16 size 34 và prefix `22 00 20 00` khớp. Kết thúc kết nối ngay sau greeting, **không gửi tên đăng nhập, password, token hoặc vé GS**. Log gateway ghi nhận kết nối từ IP được cho phép.

Mirror cố định `127.0.0.1:14186` được bật bằng wrapper ở:
`Tool/_outputs/cn02-bulk/map53-qa09/mirror_wrapper.py`

Wrapper import **read-only** source `LAN-LOCAL/m1_map53_asset_server.py`, chuyển LOG trong bộ nhớ tới output QA ngoài Git, phục vụ **1.127** địa chỉ có source, không thay code hay file tài nguyên. Mirror PID **13100**, health `catalog_1.0.hash` `200 / 32 byte`.

Client 4016 được khởi động bằng launcher thử nghiệm hiện có `LAN-LOCAL/Start-M1-Map53-EarlyRoot.mjs`, **không thay metadata**; chỉ hook runtime in-memory của phiên thử:

- `Game.exe` PID **8328**, launcher Node PID **25636**.
- Log launcher: `mountedBeforeFirstPakRead=5`, `firstNativeReadLength=572`, `lanRootBeforeFirstPakRead`, `lanRootAfterServerList`, `localServerReady=true`, `contentReady=true`, `loadReady=true`, `channelHasTabs=1`.
- Adapter LAN: `nativeLanLoginFormOpened`, `accountSubmission=false` tại sự kiện mở biểu mẫu. Đây **không** là bằng chứng người dùng vừa bấm nút đăng nhập; không lưu credential.
- Mirror lúc snapshot chỉ ghi **1 GET catalog hash 200**. **Chưa có GET nội dung bundle PC Ba Lăng Huyện trong cùng phiên snapshot**, và chưa có Unity scene-ready evidence.

## 4. Lỗi thực tế được xác định tại Bishop permit

Log gateway tại 05:59:48–05:59:49 có một chuỗi liên tục:

1. `Native Bishop authentication success roles=3`.
2. `Native user role selection verified; identity_present=True`.
3. `Native character selection failed category=PermissionError`.
4. Sau đó ngắt kết nối.

Source gateway.py gọi `BishopConnection.select_role(name)`, rồi **mới** gọi `NativeGsDispatch.issue(...)`. Trong `legacy_bishop.py`, kết quả Bishop `frame[0] == 52`, đủ **184 byte**, nhưng `frame[177] != 1` sẽ ném `PermissionError('Bishop did not permit GameServer entry')`. `NativeGsDispatch.issue` chỉ tạo permit sau khi Bishop trả hợp lệ; không được bỏ qua bước này. Do đó lỗi tương ứng với Bishop không cấp quyền vào GS, **không thể kết luận từ đây rằng converter MapCopyID đã sai**. Cần tìm điều kiện cụ thể gây từ chối ở Bishop/GS (vai trò đang online, quyền vào thế giới, trạng thái server, phiên cũ, hoặc lý do khác), tránh đoán.

Snapshot phía server (counters **cộng dồn**, không phải 8/6 phiên thành công):
- **8** native greetings.
- **6** lần Bishop authentication thành công.
- **1** lần nhận selection hợp lệ về khuôn dạng.
- **1** `PermissionError` tại selection.
- **0** GS permit gửi thành công.

Không đọc/log tên tài khoản, tên nhân vật, mật khẩu, GUID, ticket hoặc raw payload. Số lượng auth tích lũy có thể chứa retry/timeout, **không** được ghép chung các kết nối để tạo một phiên runtime hợp lệ.

## 5. Giới hạn nghiệm thu và bước tiếp theo

**Đã PASS:** gateway native listener có allowlist, handshake 34 byte thật, client gốc khởi chạy sẵn login LAN, metadata/binary hash nguyên bản, mirror catalog hoạt động.

**Chưa PASS:** `Bishop → GS permit` (Bishop từ chối trong ít nhất một lần selection), `WORLD_SYNC`, scene BaLangHuyen render, cùng actor/spawn, movement user-click → GS ACK → client echo, sáu active affine + 3+1 paired landmarks. **CN02-04 BLOCKED / CN03 CLOSED**.

Tập trung điều tra **Bishop GameServer permit denial** ở role-select. Đọc log Bishop/GS theo đúng timestamp, kiểm tra trạng thái đăng ký GameServer và trạng thái nhân vật, nhưng không reset online-state, không tạo vé giả/ACK giả, không sửa DB hoặc bypass quyền. Sau khi Bishop phát permit hợp lệ, chỉ khi đó mới thử một phiên mới đầy đủ với `check_single_session.py`.

## 6. Các tiến trình đang chạy và rollback có kiểm soát

Ở thời điểm snapshot: **gateway PID 7600**, **mirror PID 13100**, **Game.exe PID 8328**, **EarlyRoot Node PID 25636**. Chúng được để mở cho người vận hành kiểm tra. Cần kiểm tra PID hiện thời trước khi dừng, vì PID có thể thay đổi.

- **Client:** đóng `Game.exe` bằng thao tác thoát game bình thường trước; launcher/Frida sẽ detach khi game kết thúc. Không kill game đang có kết nối/nhân vật chưa lưu.
- **Mirror:** sau khi client đóng, dừng đúng PID đọc từ `Tool/_outputs/cn02-bulk/map53-qa09/mirror-pid.txt`; chỉ dừng nếu đó vẫn là Python `mirror_wrapper.py` của QA.
- **Gateway:** sau khi không còn client session, trên server dùng `Client3D-LAN/Manage-Gateway.ps1 -Action stop` (script đã xác thực PID/command) và kiểm tra port 14180 đóng. **Không dùng `restart`**, vì manager restart mặc định bỏ cờ native GS.
- **Bishop, gs001, DB và tám file material của Antigravity:** tuyệt đối không thay đổi/khởi động lại trong rollback này.

Checkpoint repo: `reports/evidence/cn02-4016-il2cpp/cn02-bulk09-live-qa-summary.json` chứa **chỉ trạng thái tóm tắt**, không chứa dữ liệu xác thực hoặc bundle.
