# JX1 3D / SwordOnline — HỒ SƠ BÀN GIAO PHIÊN SAU
## CN02 GLOBAL MAP COLLISION & MOVEMENT PARITY
**Chốt ngày:** 26-10-2026, giờ Việt Nam. **Chủ đề:** lỗi bị giật/kéo ngược vị trí khi chạy trên Map 53, kiến trúc sửa toàn bộ bản đồ.  
**Trạng thái:** NGUYÊN NHÂN SAI KHÁC COLLISION ĐÃ CÓ BẰNG CHỨNG, NHƯNG CHƯA CHỨNG MINH ĐƯỢC NGUỒN DỮ LIỆU NÀO ĐÚNG VỚI CẢNH 3D; CHƯA FIX/ACCEPT RUNTIME.  
**Gate:** `CN02-04 = BLOCKED`; `CN03 = CLOSED`. Không ghi PASS toàn dự án.

> **Đọc file này đầu phiên sau.** Cũng đọc `reports/CN02_MOVEMENT_RUBBERBAND_DEV_20261026.md`, `reports/CN02_MPS_UNITS_DEV_FIX_20261026.md` và cập nhật MỚI NHẤT từ Antigravity trước khi sửa. Tài liệu này là checkpoint ChatGPT độc lập, không được hiểu là xác nhận các thay đổi uncommitted của Antigravity đã hoàn thành.

## 1. Quyết định cuối phiên

- Người dùng nhận thấy **đường trong client 3D trông thông thoáng, không có vật cản**, nhưng server kéo người chơi ngược. Người dùng nghi GameServer đang dùng dữ liệu bản đồ JX1 cũ không tương thích client 3D mới.
- Qua log và trích dữ liệu: **có mismatch trong kết quả truy vấn va chạm của client đang chạy với lưới cản mà GameServer gốc sử dụng trên đúng hành lang bị kéo ngược**. Nhưng **chưa đủ bằng chứng để kết luận** server có phiên bản bản đồ lỗi thời so với PC_BLH_Bake, vì có thể truy vấn MapDataService bằng hệ tọa độ/API/semantics không tương đương hoặc chưa nạp collision đúng.
- **KHÔNG mặc định bên nào đúng.** Lấy geometry/navigation thật của client 4016 làm đối tượng audit và hiệu chuẩn; xác minh source của bản đồ/AssetBundle, rồi thiết lập một **versioned collision contract chung** cho Client-New + GS. Giữ GS authoritative, giữ speed/time/map/identity guards, NPC collision, không tắt legacy 206/native 120.
- Phạm vi được người dùng yêu cầu **TOÀN DIỆN mọi bản đồ tương thích**, KHÔNG chỉ Ba Lăng huyện. **Map 53 chỉ là pilot**: kiến trúc/code loader/converter/validator phải generic theo mapId + version; kiểm kê và chuyển đổi hàng loạt; nghiệm thu theo từng map, không bật toàn bộ chưa QA.

## 2. Máy, đường dẫn, nhánh

- **Máy client:** `DESKTOP-TPG2RNF` (`840e16b6-381e-4b6c-8ba8-69ffb8656284`), repo `E:\Games\VLTK3D\SwordOnline\Client-New`.
- **Bản QA THỰC SỰ đang chạy:** `E:\Games\VLTK3D\SwordOnline\Client-3D-2\update\Game.exe`, bản Unity native IL2CPP 4016, PID **21188** tại thời điểm audit; Map53 scene `PC_BLH_Bake` đã từng được xác minh loaded/visible. **KHÔNG phải binary UnityProject của Client-New.**
- **Client source chúng ta kiểm soát:** `Client-New\UnityProject`, Unity 6000.x; `Assets\_JXClient\Movement\OfflineMovement.cs`, `OfflinePathfinder.cs`, `Domain\Coordinates\AffineMapTransform.cs`, `World\BaLangHuyenSceneController.cs`. Các offline classes chưa đủ chứng minh gameplay online/GS ACK.
- **Máy GS DEV:** `WIN-33R6OP5GO1M` (`3de8f900-c81d-450d-9c64-69971de000fd`), binary `C:\Server-Game\SwordOnline\ServerNew\_bin_v2_\gs\gs001.exe` (PID **4020** khi kiểm tra), nguồn tham khảo `C:\Server-Game\SwordOnline\jxOnline\gameserver\Sources\Core\Src`. Bishop PID 3212 khi audit.
- **Native gateway:** `C:\Server-Game\SwordOnline\Client3D-LAN\gateway.py`, module `gs-bridge-m1`, cổng **14180**, PID **3828** ở log 15:23-15:45. PID động: luôn xác minh lại, không sử dụng số PID làm target dừng dịch vụ nếu không kiểm tra executable/path/command line/session.
- **Client repo branch khi bắt đầu checkpoint:** `cg/cn02-anchor-evidence`, HEAD `716a3b7`. Chưa có bằng chứng đã push GitHub. Đừng tự ý merge/push khi code đang thay đổi.
- DEV được phép build/restart khi cần với bảo vệ dữ liệu, nhưng **KHÔNG xóa/reset nhân vật/DB, KHÔNG làm hỏng server production**, không overwrite nguyên binary 4016, phải có backup/rollback.

## 3. Những việc đã làm và chứng cứ timeline

| Mốc | Bằng chứng | Phạm vi kết quả |
|---|---|---|
| 26-10 sáng | Bridge sửa MPS: GS region 512x1024, native 4016 region 1024x1024; `nativeX = legacyX*2`, `nativeY=legacyY`; inverse legacyX=floor(nativeX/2). Test bridge 129/129 PASS ở thời điểm triển khai. | Client 4016 hiện được Map53 `PC_BLH_Bake`, nhân vật thật và đi lại; KHÔNG chứng minh collision parity. |
| Sau đó | Ping 122 được pair đúng 9B (GS ping143 ↔ native ping82 ↔ GS reply174), runtime paired ping; right-drag camera adapter in-memory; traffic limit chuyển 10-second window, QA session >300s. | Ping/session/camera QA cải thiện. Camera adapter không persist sang Game.exe mới. |
| ~14:46:57 | `rubber-band-long-trace.json`: GS phát 4 forced-position thực `legacy206/native120` cho entity40720. | Xác nhận không phải do gateway tự tạo correction. |
| ~15:13 | Bridge native_movement.py + native_session.py: duplicate target 200ms, từ chối lệnh ngoài bán kính 4096 legacy MPS mà không disconnect; `148/148 PASS`, deployed gateway PID3828. | Chỉ giảm spam/mất session, **KHÔNG FIX RỦI RO GIẬT**. |
| 15:13:59.736 | GS force correction (52032,101696) ngay sau initial/move (50298,104351) của entity40723. | Lỗi sync spawn/login/GS state CŨNG CẦN tìm nguyên nhân riêng. Không quy toàn bộ về collision. |
| 15:23:13–26 | Gateway log entity40723 GS quanh legacy(50401,104402–104412), client vẫn đưa đích tiến (50399,104628); **15:23:26.241/.242** GS thực gửi forced legacy206 về (50401,104412), last target age 975ms. | Đúng thời điểm người dùng báo giật. |
| 15:24+ | Source Map53 GS thực: `maps\<tên CP936 giữ dưới dạng Windows PUA>\v_101\098_region_s.dat` và `v_102\098_region_s.dat`, section obstacle mỗi vùng 2048B/512 int32. Chính `KRegion.cpp::GetBarrier` áp cờ low4bits, triangle high4bits và tọa độ con ô. | Mẫu 4 legacy MPS dọc start(50401,104412)→goal(50399,104628): **40/55 mẫu BLOCKED**; block bắt đầu (50401,104416), băng qua boundary region101/102. Start và goal riêng lẻ đều walkable, nhưng segment cắt vùng cản. |
| 15:xx | Passive IL2CPP trên bản 4016 thật: `MapDataService` loaded/collisionActive, `GetObstacleInfo` và `GetObstacleShape` trả 0 tại 10 native MPS points quanh hành lang, ví dụ native(100802,104412),(100798,104628). | **Lệch kết quả truy vấn**. Chưa khẳng định API native đó là ground truth đi đường (có thể cần transform hoặc flags khác). |
| 16:00–16:35 | Antigravity bắt đầu mã collision contract + tooling + sửa offline pathfinding trong SAME WORKTREE. | Đang làm, các file uncommitted, chưa nghiệm thu/bật GS. |

Các bằng chứng nguyên bản:
- Server `C:\Server-Game\SwordOnline\Client3D-LAN\qa_runs\cn02-move-repeat-20261026\gateway-rangeguard.log`.
- Server `...\cn02-move-repeat-20261026\probe_map53_segment.py` (script read-only phân tích triangle) và `inspect_map53_obstacle_20261026.py`, `map53_obstacles_96_100_100_103.json`.
- Client `Client-New\.tmp\cn02-dev-resume-20261026\obstacle-live-98_101.json`; `rubber-band-long-trace.json`.
- Client reports `reports/CN02_MPS_UNITS_DEV_FIX_20261026.md`, `reports/CN02_CAMERA_TRAFFIC_DEV_20261026.md`, `reports/CN02_MOVEMENT_RUBBERBAND_DEV_20261026.md`.

**Kỹ thuật phụ:** Windows GS thư mục tên gốc GBK được ánh xạ kiểu PUA (byte >=128 thành codepoint U+EF00+byte), không dựa vào tên tiếng Hoa decode Unicode trực tiếp để dò file. GS `KRegion::LoadServerObstacle` nạp ô 16x32 từ `region_s.dat`. Chỉ số collision int32 lưu theo `grid[x * 32 + y]`. Map53 `.wor` rect gốc [82,89,114,108]. Trước đây có kết luận quá sớm rằng server luôn đúng; phiên này đã sửa lại: **phải kiểm chứng đúng theo geometry thật client**.

## 4. Antigravity WIP — CỰC KỲ QUAN TRỌNG TRÁNH ĐÈ NHAU

Khi ChatGPT kiểm tra lúc ~16:39, `git status --short` **KHÔNG sạch**. Antigravity đã claim công việc `CN02-COL-01..12` trong `TASK_BOARD.md` (owner Antigravity, branch `cg/cn02-anchor-evidence`), đang sửa:
- Modified: `TASK_BOARD.md`; `UnityProject/Assets/_JXClient/Movement/JXClient.Movement.asmdef`; `OfflineMovement.cs`; `OfflinePathfinder.cs`; `World/BaLangHuyenSceneController.cs`.
- Untracked: `UnityProject/Assets/_JXClient/Domain/Collision/MapCollisionContract.cs`; `Movement/MapCollisionProvider.cs`; `UnityProject/Assets/_JXClient/Resources/**`; `reports/evidence/cn02-collision-parity/map53-collision-divergence.json`, `map53-collision-overlay.png`, `map53-shared-collision-contract.json`; `tools/reference4016/audit_map53_collision_parity.py` và `tools/reference4016/__pycache__`.
- `MapCollisionContract.cs` đã lớn ~26.8KB và có dữ liệu Resources >1MB; đây là **code/dữ liệu của Antigravity đang triển khai**, chưa được kiểm chứng. Không stage/revert/commit thay họ; không coi file tồn tại là PASS.
- Antigravity claim cả `docs/CHECKPOINT.md`, `docs/NEXT_SESSION_HANDOFF.md`, `TASK_BOARD.md`. Để tránh race, checkpoint ChatGPT được lưu **riêng** bằng tên file hiện tại. Sau khi Antigravity chốt commit, mới hợp nhất pointer/checkpoint nếu cần. **Không xóa file untracked/pycache bằng lệnh dọn rộng**.
- Kiểm tra lại Git branch, HEAD, Git diff, version hash, test output và report Antigravity mới nhất trước khi thực hiện công việc kế tiếp; không dựa mù vào snapshot này.

## 5. Lệnh của người dùng đã giao Antigravity (tóm tắt đầy đủ)

Lệnh chính: `CN02-COL-01..12`, tự thực hiện liên tục có gate:
1. COL-01..03: audit scene `PC_BLH_Bake`, native 4016 `MapDataService`/collider/NavMesh/click-movement; GS `maplist.ini`, `.wor`, `region_s.dat`, `KRegion::GetBarrier`, `KProtocolProcess::NpcRunCommand`; tạo overlay path và tọa độ cản, xác định real root cause chứ không mặc định server cũ hoặc native đúng.
2. COL-04..05: collision contract versioned có mapId, checksum SHA256, scale, origin, bounds, region, triangle, data-validation, no-bypass. Quyết định trusted source sau khi đủ bằng chứng, không suy từ screenshot. Hỗ trợ map catalog/batch converter.
3. COL-06..07: Client-New provider + pathfinder (offline/online tách biệt), detect blocked segments, tìm đường vòng hoặc từ chối unreachable; không giả lập server ACK.
4. COL-08..10: GS DEV loader/validator chỉ mở thử Map53, giữ GS authoritative, session/speed/time/map/position constraints; Gateway giữ MPS conversion đúng, no hidden suppression of forced-position. Backup khi cần sửa/restart.
5. COL-11..12: tests regression + runtime user path có obstacle/no obstacle, route around, map transitions, auth, reconnect, forced corrections; chưa runtime thì vẫn BLOCKED.
6. Update `TASK_BOARD.md`, `docs/CHECKPOINT.md`, `docs/NEXT_SESSION_HANDOFF.md`, tạo `reports/CN02_COLLISION_PARITY_IMPLEMENTATION.md` với test exit codes/hashes/deploy/rollback/commit.

**YÊU CẦU BỔ SUNG CỦA USER:** CN02-COL là **GLOBAL MAP COLLISION & MOVEMENT PARITY**, không chỉ Map53. Code contract/coordinate/pathfinding/validator/config phải generic theo `mapId`. Kiểm kê mọi map GS và map 3D client thực có; phân nhóm match/mismatch/missing/unverified; export + verify hàng loạt từng map; map pilot 53 trước rồi bật từng nhóm sau QA. Báo cáo riêng `core complete`, `data converted per map`, `runtime verified per map`. Không tuyên bố toàn diện nếu chỉ pilot đạt.

## 6. Chưa đạt / các blocker khác

- **CN02 collision mismatch root-version vẫn chưa được kết luận tuyệt đối**: phải xác minh source/transform/variant của Map53 và API returned-zero; so sánh trực quan, raycast/collider, map geometry thật. Các tester snapshot native only 10 points chưa đủ khẳng định toàn bộ collision data.
- **Lệch spawn ngay sau login**: GS force (52032,101696) dù initial (50298,104351); cần trace NPC spawn/script/world/state, độc lập với route obstacle.
- **3+1 landmarks độc lập cho affine/world↔MPS** chưa được nghiệm thu; mặc dù MPS x2 trên wire đã giúp Map53 hiện, không chứng minh biến đổi mọi bản đồ.
- **NPC/AOI bridge omitted**, một số legacy opcode 75/80 logs spam, Lua login/logout `attempt to call a nil value` vẫn chưa xử lý; đừng giả hoàn thành các mục khác cùng lúc.
- **Client-New offline source không giống client4016 đang chạy**: dù Unity PlayMode/EditMode PASS thì đó là Client-New, không chứng minh user đang chơi 4016 hết giật. Bản 4016 temporary in-memory camera adapter cần reapply sau restart.
- Hiện **chưa có bản sửa global collision được triển khai và nghiệm thu**. Không disable GS map barrier hay luôn gửi ACK giả.

## 7. Lộ trình tiếp tục phiên sau — hành động cụ thể

1. Mở file này; kiểm tra Git trạng thái mới nhất và Antigravity report/commit (có thể đã thay đổi sau khi ghi). **Không làm đồng thời lên cùng writable paths khi Antigravity đang sửa**.
2. Độc lập review `audit_map53_collision_parity.py`, PNG/JSON overlay, contract source, source data provenance; đặc biệt phân biệt original JX1 GS raw collision với real 4016 visible-world collision; xác minh mesh/collider/regions và đúng transform. Cần đo holdout anchors và các ô cạnh ranh giới.
3. Kiểm tra thiết kế collision shared generic mapId/version/hash, no invalid data/default walkable, atomic activation per map, GS authoritative validate. Dữ liệu Map53 chỉ pilot, không hardcode map53 trong core.
4. Đòi test đơn vị triangle và path crossing segment trên cùng dữ liệu, negative hash/region tests, speed hack/teleport guards, lag/reconnect regression. Audit kết quả thực từ Antigravity, không chỉ đọc claim.
5. Chỉ khi có dữ liệu đúng geometry 3D và tests PASS mới triển khai GS DEV từng map: backup bản gốc/manifest, giữ DB, kiểm tra thời điểm restart, thực hiện smoke login→run through problem corridor, blocked/no-block, 5–10 phút movement active, log native120/legacy206, vùng khác; báo cáo mỗi map VERIFIED / NOT VERIFIED.
6. Nếu Antigravity chưa xây được online client4016 patch mà mới sửa Client-New, báo thẳng **pilot Client-New only**, chưa giải quyết hành vi 4016 người dùng đang chạy; có thể cần porting/collision import vào 4016 QA hoặc chuyển QA sang Client-New online khi sẵn sàng.
7. Khi toàn bộ gate đạt: update chính thức docs/TASK_BOARD và commits, push GitHub theo quy tắc repo; giữ `CN02-04 BLOCKED` cho đến khi user kiểm tra được và bằng chứng runtime đủ.

## 8. Prompt phục hồi cho chat tiếp

```text
Tiếp tục JX1 3D SwordOnline Client-New, mốc CN02 global collision & movement parity.
Đọc E:\Games\VLTK3D\SwordOnline\Client-New\docs\CN02_GLOBAL_COLLISION_SESSION_HANDOFF_20261026.md.
Kiểm tra Antigravity WIP/commit/report CN02-COL-01..12 mới nhất, review độc lập không đè worktree.
Mục tiêu sửa toàn bộ bản đồ (Map53 pilot) đồng bộ collision/pathfinding/client-3D/GS authoritative.
User quan sát đường client thông thoáng nhưng GS kéo ngược; GS legacy region_s.dat có 40/55 mẫu blocked trên đường entity40723 15:23:26, MapDataService client reported0 tại điểm sample, nhưng chưa chứng minh chính xác lý do mismatch/nguồn nào đúng.
Đừng mặc định GS cũ hay client đúng, đừng vô hiệu hóa server collision/forced correction/DB protection.
Kiểm thử, deployment DEV an toàn, chỉ báo đạt khi thực runtime có chứng cứ; CN02-04 BLOCKED, CN03 CLOSED cho tới khi nghiệm thu.
```

**Ghi chú lưu trữ:** checkpoint này chỉ là tài liệu phục hồi phiên, không sửa mã nguồn client/server, không restart dịch vụ; nếu Git commit sẽ chỉ stage đúng file này, không stage thay đổi Antigravity.
