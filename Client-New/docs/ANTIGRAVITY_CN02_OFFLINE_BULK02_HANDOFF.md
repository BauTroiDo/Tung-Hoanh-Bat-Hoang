# Lệnh bàn giao Antigravity — CN02-OFFLINE-BULK-02

**Ngày:** 2026-10-26 (Asia/Ho_Chi_Minh) | **Repo:** `E:\Games\VLTK3D\SwordOnline\Client-New` | **Baseline ChatGPT:** `9ac9b4c` (trước CN02-OFFLINE-02).
**Trạng thái:** **SERVER_PAUSED_BY_USER**. Chủ dự án đang tắt và sửa Bishop/GameServer/gateway. CẤM kết nối, kiểm tra cổng, khởi động hay thay đổi server/DB, đăng nhập client 4016 cho đến khi chủ dự án xác nhận sửa xong. Đây là lệnh **chuẩn bị bàn giao**, không phải bằng chứng Antigravity đã chạy/nhận việc.

## 0. Điều kiện vào việc

1. Đọc `AGENTS.md`, `README.md`, `TASK_BOARD.md`, `docs/CHECKPOINT.md`, `docs/NEXT_SESSION_HANDOFF.md`, `reports/CN02_OFFLINE01_CLIENT_HEALTH.md`.
2. Kiểm tra `git status --porcelain=v1` và claim file trong TASK_BOARD trước khi sửa; **không reset/revert/ghi đè** thay đổi của agent khác. Antigravity owner mặc định `UnityProject/**`, `Tools/**`; ChatGPT chỉ làm `docs/**`, `reports/**`, `tools/reference4016/**` trong claim của mình.
3. Tám material sau **đang modified chưa commit** và được giao cho Antigravity xử lý: `BLH_BoundaryRock.mat`, `BLH_GrassTerrain.mat`, `BLH_HouseWall.mat`, `BLH_PlazaTile.mat`, `BLH_StoneRoad.mat`, `BLH_TileRoof.mat`, `BLH_WoodTimber.mat`, `Avatar.mat`. Đánh giá từng diff, hoàn thiện rồi mới stage/commit trong lane Antigravity.
4. Unity Editor đã kiểm chứng: `C:\Program Files\Unity\Hub\Editor\6000.6.7f1\Editor\Unity.exe`. Baseline offline thật: **EditMode 29/29 PASS; PlayMode 18/18 PASS; Player Windows x64 build PASS**; Python static regressions 305/305 PASS. Build nguồn tại `.tmp/cn02-offline01/Builds/JXClientNew.exe` SHA256 `9806ABDE4A406ED68647E9505852C49F6AA72A372F4F084E2AA2821AFFEA57A2`.
5. **CN02-04 vẫn BLOCKED, CN03 CLOSED.** `BaLangHuyen.unity` chỉ là **synthetic fallback**, KHÔNG phải địa hình Map53 gốc đã khôi phục. Không lấy bài test mô phỏng làm GameServer-authoritative PASS.
6. Phối hợp: ChatGPT làm review độc lập và phát triển công cụ kiểm thử ngoại tuyến bên ngoài `UnityProject/**`. Chỉ một owner sửa một file một thời điểm.

## 1. CN02-OFFLINE-BULK-02-A — Mass asset/material and prefab health

Viết một Unity Editor Validator/Fixer quét `Assets/_JXClient/**` theo batch, trước tiên chỉ báo cáo, sau đó đề xuất sửa có kiểm soát:

- URP shader/material hồng, unsupported shader, thiếu texture, render queue/surface type, ánh sáng, phản chiếu, độ mờ alpha; không thay vật liệu chuẩn đã chốt khi chưa có bằng chứng.
- MeshFilter/MeshRenderer, renderer bật/tắt sai, mesh bounds, transform, scale bất thường; Collider/trigger/layer và prefab reference.
- Hierarchy bắt buộc của scene: `Ground`, `Environment`, `StaticProps`, `SpawnPoints`, `Colliders`, `Lighting`, `Nav`.
- Missing GUID/meta, duplicate GUID, missing script, nullable reference. Giữ nguyên source GUID gốc trong project.
- Tạo bảng trước/sau, xử lý hàng loạt nhưng chỉ thay đổi file Antigravity đã claim; không sửa qua YAML khi Unity API là nguồn đúng.

**Gate A:** Không còn lỗi P0 ảnh hưởng đến render/collider, missing reference chưa giải quyết được đều phải ghi BLOCKED có nguồn cụ thể. Chạy EditMode/PlayMode; không giảm assertion để PASS.

## 2. CN02-OFFLINE-BULK-02-B — Ba Lăng Huyện offline playable

- `Assets/_JXClient/Scenes/BaLangHuyen.unity` và code `World/**`, `Movement/**`: thiết lập terrain walkable, collision nhà/cổng/đá/tường/bậc thềm; dừng tại vật cản hoặc định tuyến đường đi hợp lệ bằng NavMesh. Không xuyên tường / rơi khỏi map / click qua UI.
- Camera: orbit, zoom, giới hạn clipping, camera theo player; xử lý camera bị vật cản che/đâm xuyên.
- Player spawn: đặt trên collider hợp lệ, không dùng `(0,0,0)` như dữ liệu bản đồ gốc. Chọn mẫu nhiều vùng; đo grounded/collider coverage và path reachability.
- Tách tuyệt đối offline `OfflineMovement` khỏi server-authoritative move; không fake server ACK.
- Thử UI/Touch/Mouse, click ngoài view, click vào UI và click qua wall; kiểm tra người chơi không dịch chuyển khi request bị từ chối.

**Gate B:** nhiều điểm xuất phát–đích trong scene synthetic PASS, vật cản không xuyên, sự kiện blocked/unreachable được quan sát thật. Vẫn gắn nhãn SYNTHETIC.

## 3. CN02-OFFLINE-BULK-02-C — Bulk PlayMode / visual runtime QA

- Unity Test Runner EditMode/PlayMode và một bản Win64 EXE thật, kiểm tra startup, scene, player, camera, collider, click-to-move, idle/walk/blocked.
- Viết Editor automation quét số lượng đối tượng/renderer/collider/active camera, nhiều rays/probes NavMesh, lưu CSV/JSON + NUnit XML.
- **Hình ảnh thật** từ Unity Player: bốn góc camera, spawn, click destination, blocked test. Không thay bằng ảnh giả hay chỉ render screenshot giả từ editor.
- Giữ tất cả log/testResult/bản build dưới thư mục tạm hoặc output có nhãn, không stage Build/Library/Temp/Logs lên Git.
- Thống kê FPS/memory nếu đo được, nói rõ GPU/runtime build/driver; không suy đoán nếu headless/nographics.

**Gate C:** runtime ảnh/log thực được gắn với cùng version+SHA build. Chưa có chứng cứ runtime thì ghi BLOCKED cho visual, không claim visual PASS.

## 4. CN02-OFFLINE-BULK-02-D — Win64 player nghiệm thu

- Build từ source UnityProject; có `JXClientNew.exe`, `UnityPlayer.dll`, `JXClientNew_Data/` và log exit0. Không dùng client 4016/V9/Frida để tạo sản phẩm.
- Chạy Player ở chế độ offline, kiểm tra scene startup và movement. Bảo đảm không buộc kết nối Bishop/GameServer. Không hardcode account/IP/network.
- Kiểm tra SHA256 binary và các hash vật liệu quan trọng; ghi nhận mọi regression so với EditMode29/29, PlayMode18/18.
- Có script rollback những thay đổi mới, tuyệt đối không xóa 8 material chưa commit của Antigravity.

**Gate D:** bản chạy offline thực cho user được, không cần server. Phân biệt `OFFLINE_PLAYABLE_PASS` với `ORIGINAL_MAP53_RESTORATION_NOT_VERIFIED` và `GS_AUTHORITATIVE_MOVEMENT_BLOCKED`.

## 5. Cách tổ chức và nghiệm thu

Chạy A → B → C → D theo thứ tự, tự sửa/build/test lỗi bình thường mà không hỏi người dùng sau mỗi lỗi. Mỗi pha cần claim riêng và commit riêng khi PASS, giữ git status rõ ràng. Không dùng commit/bài test của lane ChatGPT để tuyên bố kết quả mới của Antigravity.

**Tài liệu bàn giao cuối:** `reports/CN02_OFFLINE_BULK02_FINAL.md` gồm commit SHA, diff/stat, commands, exit code, NUnit XML, SHA build, ảnh thật, danh sách lỗi và giới hạn. Cập nhật `TASK_BOARD.md`, `docs/CHECKPOINT.md`, `docs/NEXT_SESSION_HANDOFF.md`. Nếu không có quyền sửa báo cáo chung, ghi báo cáo trong lane riêng và đề xuất hợp nhất.

## 6. Song song phần ChatGPT

ChatGPT sẽ **không đụng UnityProject/** trong thời gian Antigravity làm A–D. Tập trung kiểm tra bản Win64 hiện có, đối chiếu cấu hình scene startup, xây bộ kiểm tra độc lập và báo cáo riêng `CN02-OFFLINE-02`.

**Lệnh mở lại server:** chỉ khi chính chủ dự án nói server đã sửa xong; lúc đó bắt đầu một preflight mới, không tái sử dụng trạng thái PID cũ.

## 2026-10-26 06:52 — IMPORTANT CONCURRENT ANTIGRAVITY UPDATE

After ChatGPT captured the known Win64 **baseline executable** (SHA256 9806ABDE...A57A2), the current branch acquired **Antigravity commit `92df731`** (`feat(cn02-bulk01): batch audit and finalize URP materials, prefabs, and scene hierarchy`). Its tracked eight material diffs are **now committed**. Antigravity has also left `UnityProject/Assets/_JXClient/Movement/OfflineMovement.cs` modified and `OfflinePathfinder.cs` untracked, consistent with **BULK B still IN PROGRESS**. These are **Antigravity-owned**, do not add/revert/overwrite or run another Unity Editor/build concurrently. Source commit BULK A and report `reports/CN02_BULK01_ASSET_AUDIT.md` claim PASS; ChatGPT has **not independently rerun** the new Unity tests after its commit.

The 8 actual screenshots and movement smoke results in CN02-OFFLINE-02 belong to the **previous Win64 binary** at `.tmp/cn02-offline01/Builds/JXClientNew.exe`, built before the new Antigravity source changes; do not claim they validate the new OfflinePathfinder or material build. ChatGPT now ends its independent smoke without further UnityProject writes. **SERVER_PAUSED_BY_USER** remains in force.
