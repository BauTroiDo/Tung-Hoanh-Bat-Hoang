# JX1 3D — LỘ TRÌNH TRIỂN KHAI DUY NHẤT

**Trạng thái:** ACTIVE · **Phiên bản:** 1.0 · **Chốt ngày:** 2026-10-22
**Nguồn chuẩn duy nhất (source of truth):** `JX3D_MASTER_ROADMAP.md` trên nhánh `main` của [BauTroiDo/Tung-Hoanh-Bat-Hoang](https://github.com/BauTroiDo/Tung-Hoanh-Bat-Hoang).
**Mốc đang thực hiện:** **M1 / GAME-02 — Snapshot nhân vật/NPC thật từ Kingsoft GameServer sang WorldRuntime/AOI.**

> **Quy tắc cốt lõi:** hoàn thiện một bản **JX1 3D có thể chơi được trên Windows x64** trước; sau đó mới hoàn thiện gameplay, mở rộng Web/Mobile, chuyển Linux native và nâng cấp kiến trúc. Chỉ **một mốc chính ACTIVE** tại mỗi thời điểm. Không tạo thêm lộ trình song song.

## 1. Quyết định cố định

1. **Không dùng Client V9** cho JX1 3D. Dùng client **mới** hướng tới PC, Web, Android, iOS. Việc capture/khớp packet và handshake V9 **không còn là điều kiện nghiệm thu** đường client mới.
2. **Giữ gameplay Kingsoft C++ Win64 hiện có** làm nền cho bản chơi được đầu tiên. Không thay toàn bộ CoreServer/GameServer bằng WorldServer mới ở giai đoạn này. WorldRuntime/AOI chỉ tích hợp dần, có cơ chế mặc định tắt và rollback.
3. **Bishop hiện tại chỉ là thành phần legacy** của server đang hoạt động; đã được build/triển khai và xác minh các kết nối nội bộ. **Không đưa Bishop vào luồng xác thực của client mới** và không tiếp tục nâng cấp Bishop nếu không có lỗi DEV cụ thể liên quan nhiệm vụ hiện tại.
4. **349 UNKNOWN_HELPER** là phạm vi khôi phục gameplay của **Graviti**. Tiếp nhận các thay đổi đã có bằng chứng và kiểm thử, **không tự ý làm lại hoặc hợp nhất hàng loạt**. Báo cáo 349/349 của agent không thay thế kiểm toán runtime/gameplay.
5. **Công việc Linux x86_64, scale-out, tối ưu lớn, thay kiến trúc V2** thuộc giai đoạn sau khi game Windows chạy ổn định. Các PR Linux ABI/KTimer đã merge được giữ nguyên; **chưa có** GameServer ELF native được nghiệm thu.
6. Không reset/rebase/copy đè nguồn gốc `jxOnline` hoặc worktree tích hợp đang bẩn. Chỉ làm trong nhánh/worktree sạch từ GitHub `main`, kiểm thử độc lập rồi mới merge. Không deploy hay restart runtime đang chạy khi chưa đạt gate tương ứng.

## 2. Một chuỗi mốc, một bước tiếp theo

| Mốc | Kết quả phải bàn giao | Trạng thái / Gate |
| --- | --- | --- |
| **M0 — Chốt nền Win64** | Source có thể build, Bishop và GS legacy hoạt động; AuthTicket, SpatialAOI, World Adapter offline; điểm nối tick CoreServer | **ĐÃ CÓ NỀN / ĐÃ XÁC MINH TỪNG PHẦN**; không đồng nghĩa game mới chạy E2E |
| **M1 — GAME-02: Entity → World/AOI** | Snapshot ổn định người chơi/NPC, World ID, tọa độ, bản đồ/scene epoch từ tick Kingsoft sang WorldRuntime; adapter chỉ bật trong thử nghiệm | **ACTIVE — LÀM NGAY** |
| **M2 — NET-01: Client mới ↔ Gateway ↔ GameServer** | Đặc tả giao thức mới, codec C++/TS, TLS/WSS Gateway, xác thực tài khoản và ticket dùng một lần có kho phiên dùng chung, gắn phiên WorldServer thật | **CHỜ M1** |
| **M3 — PLAY-01: Phiên chơi đầu tiên** | Client mới đăng nhập → tạo/chọn nhân vật → vào map 3D → 2 người nhìn nhau/di chuyển → NPC/quái/kỹ năng → lưu và đăng nhập lại | **CHỜ M2** |
| **M4 — GAMEPLAY-01: Hoàn thiện + ổn định** | Quest, skill, item, trang bị, NPC/AI, giao dịch, bang hội, phụ bản, lỗi gameplay, persistence; kiểm thử hồi quy và tải | **CHỜ M3** |
| **M5 — CLIENT-02: Web/Mobile** | Cùng giao thức mới, xác thực và gameplay trên PC/Web/Android/iOS, kiểm tra riêng mạng/điều khiển/tải từng nền tảng | **CHỜ M4** |
| **M6 — PLATFORM-02: Linux native & nâng cấp** | Server ELF x86_64 native, tương đương Win64, CMake/CI, vận hành và tải; sau đó mới nâng cấp AOI/World/DB từng module | **HOÃN ĐẾN SAU BẢN CHƠI ỔN ĐỊNH** |

**Không được tuyên bố hoàn thành một mốc chỉ nhờ “build PASS” hoặc unit test PASS** nếu gate của mốc đó đòi hỏi client/server đang chạy thật.

## 3. Công việc DUY NHẤT đang mở: M1 / GAME-02

### M1.1 — Kiểm toán dữ liệu và thread ownership (read-only trước)

- Chứng minh `KSubWorldSet::MainLoop()` → `SubWorld[i].Activate()` là điểm đọc snapshot an toàn trên world owner tick. Phân biệt IOCP/worker và owner thread; không truyền con trỏ global sống qua thread.
- Xác định nguồn **server-authoritative** của ID nhân vật/NPC, map/SubWorld/instance/scene epoch, vị trí, hướng, trạng thái spawn/despawn, teleport và logout. Ghi mapping/đơn vị tọa độ rõ ràng, không suy đoán 2D sang 3D.
- Kiểm tra giới hạn số entity, kích thước snapshot, tần suất tick, race khi rời map/chết/disconnect. Không sửa gameplay ở bước audit.

### M1.2 — Snapshot bridge nhỏ, mặc định tắt

- Chụp snapshot dữ liệu cần thiết vào cấu trúc **giá trị sở hữu rõ ràng** với quota; không giữ `KPlayer*`, `KNpc*` hoặc dữ liệu global chưa được khóa.
- Dùng `Jx3dLegacyWorldTickBridge` đã merge (PR #16) làm mốc owner tick; nếu muốn đổi contract thì phải test hồi quy.
- Dùng `SpatialAoi` / `WorldServerIntegrationAdapter` (PR #14) để xử lý spawn/visibility/despawn/scene change. Các callback mạng chuyển qua hàng đợi owner thread, không ghi gameplay từ IO thread.
- Bật chỉ trong **test/worktree độc lập**. Mặc định OFF trong bản sản xuất/DEV hiện hữu.

### Gate M1 — chỉ được chuyển sang M2 khi đủ tất cả

- [ ] Có bảng nguồn mã `player/NPC → stable EntityKey, map/scene, tọa độ` và giải thích lifetime/ownership.
- [ ] Source Win64 `CoreServer` build đạt phạm vi nguồn liên quan; không phát sinh lỗi critical, không thay đổi gameplay khi tính năng tắt.
- [ ] Unit + regression Windows x64 và Ubuntu (sanitizer cho thành phần portable) PASS; trường hợp disconnect, map change, entity despawn và ID tái sử dụng có test âm tính.
- [ ] Có bài tích hợp với **dữ liệu snapshot gameplay thật trong harness/server thử**, xác nhận AOI spawn/despawn đúng và không đọc dangling pointer; chưa cần client đồ họa.
- [ ] PR độc lập được review/merge; rõ binary/runtime nào đã/chưa triển khai, lưu commit/SHA/test/rollback.

**Lệnh triển khai tiếp theo mặc định:** `GAME-02.1 — truy mã KPlayer/KNpc/SubWorld, lập bảng ID/toạ độ/thread ownership; sau đó patch snapshot theo bằng chứng`. Không tự chuyển sang Linux, Bishop, CGI/V9 hay thiết kế server V2.

## 4. Gate cho các mốc sau — không triển khai vượt thứ tự

### M2 / NET-01 — Giao thức **JX1 3D mới**, không packet V9

- **CG01-NEW:** lập danh sách thông điệp tối thiểu **của client mới**: handshake/version, sign-in, realm/role list, world join, spawn/despawn, movement, AOI ACK, combat, save/logout. *CG01-legacy* 0/10 capture gốc V9 chuyển **ARCHIVED / NOT REQUIRED**.
- **CG02/CG03/CG04:** **tái dùng** validator/schema và generator C++/TypeScript đã có kiểm thử cục bộ; chỉ PASS khi có golden fixtures C++ ↔ TS của chính schema JX1 3D mới, kiểm tra endian, chiều dài, overflow, fuzz. Không coi thử nghiệm cũ là pass giao thức mới.
- **CG05 + AUTH-02:** Gateway WSS/TLS và owner-queue có các bản thử cục bộ (CG05 110/110, owner 132/132) **chưa nối server thật**. Nối với AuthService xác minh tài khoản thật, kho ticket *dùng chung giữa các tiến trình* với consume nguyên tử; không dùng registry trong RAM riêng biệt của mỗi process để tuyên bố xác thực phân tán đã xong.
- **Gate M2:** kết nối TLS/WSS thực, hai session hợp lệ; replay/invalid/expired ticket bị từ chối; world owner nhận session do server xác định; packet phân mảnh/coalescing/disconnect có test; không có mật khẩu gửi vào GameServer.

### M3 / PLAY-01 — Phiên gameplay thật

- Client **mới** đăng nhập, tạo/chọn nhân vật, vào map 3D và thấy ít nhất 1 NPC + 2 nhân vật nhìn thấy nhau; di chuyển server-authoritative, đánh quái/kỹ năng cơ bản; save, disconnect và relog giữ dữ liệu.
- **Gate M3:** video/log/test script 2 client, binary SHA, source commit, rollback và toàn bộ server trạng thái ổn định. Chưa đạt nếu chỉ là event giả/synthetic frame trong CTest.

### M4 / GAMEPLAY-01 — Hoàn thiện hệ thống chơi

- Nghiệm thu từng gameplay module từ source Win64, Graviti 349-helper theo dữ liệu tham chiếu, khả năng save/atomic trade/anti-dup, pet, quest, skill, NPC, PvP, bang hội, phó bản.
- Sửa lỗi theo mức độ P0/P1; có regression trước khi đánh dấu PASS. Benchmark nhiều người và kiểm thử tải **sau khi** luồng chơi hoạt động; không hy sinh đúng nghiệp vụ để đổi kiến trúc.

### M5–M6 — Mở rộng sau playable

- **M5:** thêm Web/Android/iOS vào giao thức và thế giới đã nghiệm thu trên PC; test riêng theo nền tảng.
- **M6:** Linux native từ các lát cắt đã merge (ABI/KTimer), rồi Engine/CoreServer/GameServer ELF x86_64, Lua/DB/network, CI, 100/500 người, backup/rollback; không dùng build WSL một thư viện làm bằng chứng server Linux hoàn chỉnh. Chỉ tái cấu trúc lớn sau khi so sánh parity.

## 5. Bằng chứng nền hiện có (KHÔNG nhầm với runtime E2E)

| Nguồn chứng minh | Đã có | Chưa chứng minh |
| --- | --- | --- |
| PR #12 | AuthTicketAuthority C++17, one-use, local test 7/7 | AuthService network + shared ticket store |
| PR #14 | AOI budget/pool + WorldServer adapter mặc định OFF; 10/10 CTest | CoreServer/gameplay nối runtime |
| PR #15 | Hai session Auth → World → AOI trong harness; 11/11 CTest | Hai client thật / codec mạng |
| PR #16 | C ABI owner-tick CoreServer, Win64 `ClCompile` PASS, Windows/Linux/san test 1/1 | Entity snapshot, CoreServer DLL link mới, DEV rollout |
| PR #18 | GAME-02.1 lập bảng nguồn `KPlayer/KNpc/SubWorld`, ID/slot, tọa độ 2D và checkpoint owner-tick | Chưa chứng minh toàn bộ writer/IOCP và lifetime scene/instance |
| PR #19/#20 | AOI adapter có `erase` owner-only, từ chối generation cũ, giải phóng active quota sau despawn; Windows LLVM-MinGW full build + 11/11 CTest PASS | Chưa nối entity snapshot gameplay thật, thiếu sanitizer Ubuntu và build/link CoreServer toàn phần |
| PR #22 | Kingsoft `KSubWorldSet` snapshot giá trị 2D, quota 512, batch incomplete fail-closed, chỉ bật khi đủ 2 macro; MSVC x64 `ClCompile` PASS, Windows 3/3 CTest, Ubuntu GCC ASan+UBSan 3/3 PASS | Chưa có callback runtime/consumer AOI, chưa chứng minh tất cả writer/thread; thiếu EntityKey generation, sceneEpoch, Win64 DLL link và E2E gameplay |
| Bishop `Bishop_20261022.exe` | DEV `56722/56732` LISTEN và GS/S3Relay/AccountServer/RoleServer đã kết nối | Client JX1 3D mới sử dụng Bishop — **không đặt mục tiêu này** |
| Linux PR #5/#7 | Engine ABI / KNode/KList / Timer native slices có test | Native Linux GameServer ELF / E2E |

Nền gameplay Kingsoft Win64 hiện hữu dùng làm mốc chức năng; không lấy việc một tiến trình `gs001.exe` đang chạy làm bằng chứng bản client **mới** chơi được.

## 6. Quy tắc quản trị để chống làm lan man

- **Chỉ sửa việc của mốc ACTIVE.** Việc khác ghi vào bảng `PARKING LOT` bên dưới, không tạo PR triển khai trừ khi đó là lỗi P0 đe dọa dữ liệu, bảo mật hoặc khả năng khởi động hiện tại. Ngoại lệ P0 phải báo lý do, ảnh hưởng và rollback rõ ràng.
- **Một thời điểm một mốc; trong mốc có thể nhiều PR nhỏ nhưng không mở thêm roadmap.** Khi nghiệm thu đủ checklist mới chuyển trạng thái sang mốc kế tiếp.
- Mỗi PR/task bắt buộc báo: mốc, file thay đổi, nguồn bằng chứng, commit SHA, Windows/Linux build/test *đã thực sự chạy*, kết quả runtime nếu có, thay đổi DB/process, rollback, blocker. **Không PASS theo báo cáo tự khai nếu không có bằng chứng.**
- Nếu test/build FAIL: sửa đúng mốc đó. Nếu thiếu tài nguyên: ghi BLOCKED với nguyên nhân, không tự ý nhảy sang một hạng mục không liên quan.
- Các tài liệu `07_IMPLEMENTATION_ROADMAP_AND_TESTS.md`, `10_CROSS_PLATFORM_PORTING_PLAN.md`, CG01–CG05 và mọi checkpoint trước là **tài liệu lịch sử/chi tiết kỹ thuật**, không phải lịch triển khai cạnh tranh. Tiến độ duy nhất được cập nhật ở **bảng trạng thái trong file này**.
- **Không chạy nền tự động:** việc được thực hiện trong từng phiên/task đã yêu cầu. Không cam kết cập nhật hoặc chạy công việc khi không có phiên thực thi.

## 7. Parking lot — giữ lại, KHÔNG làm ngay

| Công việc | Thời điểm mở lại |
| --- | --- |
| CG01 capture/packet gốc V9 | Chỉ khi có lỗi cần tương thích legacy do người dùng yêu cầu riêng; không block JX1 3D |
| Bishop mở rộng, thay bằng server auth thế hệ 2 | Chỉ khi bug DEV cụ thể hoặc sau baseline chơi được; client mới không dùng Bishop |
| Linux filesystem/epoll/Lua/DB/ELF/CI + CORE-36..43 | M6 |
| Kiến trúc microservice/WorldServer V2, shard và scale-out | Sau M3/M4, theo benchmark thực |
| Thêm tính năng ngoài vertical slice: PvP/phó bản phức tạp/đồ họa phụ | M4/M5 |
| Graviti 349 UNKNOWN_HELPER | Lane độc lập do Graviti sở hữu; chỉ nhập kết quả đã nghiệm thu tại mốc gameplay M4 |

## 8. Cập nhật trạng thái sau mỗi mốc

Chỉ sửa **ba dòng** trong bảng điều hành này khi có bằng chứng:

- **ACTIVE:** `M1 / GAME-02`
- **NEXT:** `M2 / NET-01` **sau khi M1 PASS**
- **LAST VERIFIED BASELINE:** `main@a1d52e7531443ca2582d2bee8cfc9df9725ee737` (PR #18–#22; audit + AOI erase/quota + snapshot opt-in, vẫn chưa đạt Gate M1), 2026-10-22.

Khi merge các PR tiếp theo, cập nhật commit nền và checklist gate tương ứng trong chính file này, không tạo file lộ trình thứ hai.
