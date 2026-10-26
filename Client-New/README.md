# SwordOnline / JX1 3D — Client-New

**Một Unity project có source C# do nhóm kiểm soát**, phục vụ game JX1 3D dùng nguyên Bishop và GameServer đã có. Đây là thư mục hợp tác giữa **Antigravity (triển khai Unity)** và **ChatGPT (kiến trúc, hợp đồng giao thức, kiểm tra chất lượng và hỗ trợ tích hợp)**.

**Trạng thái 25/10/2026: CN-00 PASS, CN-01 PASS; CN-02 PARTIAL / hiệu chỉnh thật chưa xác minh.** Có scene BaLangHuyen synthetic chơi offline, fix lifecycle/collider và công cụ Anchor Evidence (3+1). Kiểm thử mới nhất: 29/29 EditMode, 18/18 PlayMode; build Win64 source-controlled. Chưa có real 3+1 anchors hoặc kết nối server mới. Xem docs/CHECKPOINT.md và reports/CN02_ANCHOR_WORKFLOW.md.

## Bắt đầu từ đâu

- **Phiên chat mới:** đọc docs/NEXT_SESSION_HANDOFF.md trước, sau đó kiểm tra Git, checkpoint và claim mới nhất.

- **Antigravity:** đọc ANTIGRAVITY_START_HERE.md và docs/NEXT_SESSION_HANDOFF.md; CN00/CN01 đã PASS, công việc còn lại là bằng chứng/hiệu chỉnh CN02 theo gate hiện tại.
- **ChatGPT hoặc AI reviewer:** đọc CHATGPT_REVIEW_GUIDE.md, AGENTS.md, docs/CHECKPOINT.md và TASK_BOARD.md.
- **Người quản lý dự án:** xem docs/00_ROADMAP.md cùng TASK_BOARD.md trước để biết việc nào đã được chứng minh bằng build/test thật.
- **Tất cả:** cùng sử dụng một roadmap CN-00 → CN-06, một bảng việc và một checkpoint. Không tách thêm lộ trình cạnh tranh.

## Mục tiêu đầu tiên (vertical slice)

1. Có Unity Editor mở và biên dịch được project mới.
2. Tạo Ground + Collider + Avatar + Camera, click-to-move được trong scene thử nghiệm **offline**.
3. Phục dựng hoặc tái tạo hợp lệ scene **Ba Lăng huyện / JX1 Map ID 53**: mặt đất, vật cản, điểm spawn, collider đúng.
4. Qua Gateway đến Bishop/GS thật, hiện **một nhân vật sẵn có** trong Map 53.
5. Click một lần sau khi vào map; **gs001.exe xác nhận thay đổi vị trí**, Unity nhận lại PositionUpdate.
6. Xuất bản Win64 có thể chạy **không phụ thuộc Game.exe 4016, Frida hoặc bản vá nhị phân**.

Web, Android và iOS được xét sau khi bản PC qua nghiệm thu; kiến trúc phải không khóa đường đa nền tảng.

## Cấu trúc tài liệu

| Tài liệu | Vai trò |
|---|---|
| AGENTS.md / GEMINI.md | Quy tắc bắt buộc cho cả hai AI, lối vào Antigravity |
| ANTIGRAVITY_START_HERE.md | Lệnh bắt đầu thực thi CN-00 |
| CHATGPT_REVIEW_GUIDE.md | Cách review và bàn giao |
| TASK_BOARD.md | Trạng thái task, người giữ quyền sửa |
| docs/CHECKPOINT.md | Sự thật phiên gần nhất, thay đổi và điểm chặn |
| docs/00_ROADMAP.md | Lộ trình duy nhất CN-00→CN-06 và gate |
| docs/01_ARCHITECTURE.md | Module Unity / giao tiếp server |
| docs/02_PROTOCOL.md | Hợp đồng mạng v1 đề xuất, mapping legacy |
| docs/03_WORLD_COORDINATES.md | Map53, tọa độ, scene, di chuyển |
| docs/04_ASSET_SCENE.md | Nhập/phục dựng asset và quản lý quyền |
| docs/05_ACCEPTANCE_TESTS.md | Bằng chứng PASS/FAIL và lệnh kiểm thử |
| docs/06_REFERENCE_ENV.md | Môi trường thực tế, hash, tham chiếu |
| docs/07_COLLAB_GIT.md | Quyền sở hữu, branch, review, tránh xung đột |
| docs/08_PLATFORMS_BUILD.md | PC/Web/Mobile, build và phát hành |
| docs/09_RISKS_DECISIONS.md | Quyết định đã chốt, rủi ro và thay đổi |
| docs/10_SAFETY_ROLLBACK.md | Bảo vệ server/nhân vật/bí mật |
| tasks/CN-00.md / tasks/CN-01_TO_CN-06.md | Task thực thi đầu tiên và đặc tả các gate sau |
| reports/BASELINE_2026-10-25.md | Bằng chứng kiểm kê khi tạo tài liệu |
| templates/* | Mẫu báo cáo, handoff, nguồn gốc asset |
| reports/DOCUMENTATION_AUDIT_2026-10-25.md | Biên bản kiểm tra bộ tài liệu |

## Vùng làm việc và phạm vi

Workspace: E:/Games/VLTK3D/SwordOnline/Client-New

Không tùy tiện tạo lại repository server hay sửa client tham chiếu. Nội dung trong Client-New phải do nhóm chủ động viết, hoặc được nhập với quyền sử dụng đã kiểm tra.

**Server hiện có:** 192.168.1.69; Bishop 56722; gs001.exe 51189; gateway thử nghiệm 14180. Các cổng cần kiểm tra lại tại thời điểm chạy; thông tin này không cấp phép chỉnh sửa server production.

**Điều kiện hoàn tất:** có dữ liệu build/test thật, hash/commit và dấu mốc trong docs/CHECKPOINT.md. Tài liệu không thay thế sản phẩm.

## Bộ công cụ phân tích tham chiếu cho CN02-BULK

Trên máy Windows được kết nối: `E:\Games\VLTK3D\SwordOnline\Tool`. Có source và ZIP của Il2CppInspectorRedux, il2cpp-dumper-rs, Ghidra và AssetRipper. Đây là nguồn công cụ cho hướng phân tích tự động IL2CPP v39, **không mặc định đã build sẵn**. Bắt đầu bằng `docs/23_REFERENCE_TOOLCHAIN.md` và `Tool/README_CLIENT_NEW.md`; mỗi phiên kiểm tra trạng thái môi trường/công cụ trước khi sử dụng.
