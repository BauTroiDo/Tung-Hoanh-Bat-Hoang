# Latest session handoff — source d1ccdc1, 2026-10-25

Read **docs/NEXT_SESSION_HANDOFF.md** first, then current checkpoint/board/Git. CN00 and CN01 are PASS; CN02 scene, lifecycle fixes and anchor workflow are implemented. Latest suites: 29 EditMode / 18 PlayMode PASS. The remaining blocker is verified real 3+1 coordinate evidence. Earlier CN01-review/CN00 instructions below are historical and already completed. Do not restart those milestones or open CN03 prematurely.

---
# Current handoff — 2026-10-25: Review CN01 → implement CN02

**The CN00 instructions below are historical, already completed. Do not recreate the project or return to the old CN00 branch.**

Read `tasks/ANTIGRAVITY_CN02_HANDOFF.md` as the current task, then latest `docs/CHECKPOINT.md` and `TASK_BOARD.md`. Source baseline is `755747f` on `cg/cn01-offline-movement`, plus newer handoff documentation commits. User accepted the playable result and requested continuation. Independently review CN01 first; after PASS, implement the CN02 World/Scene scope. Codex owns coordinate contract/review; exact boundaries are in the handoff. Preserve current source and all uncommitted work.

---
# ANTIGRAVITY — BẮT ĐẦU CN-00 NGAY

**Đọc trước:** AGENTS.md, docs/CHECKPOINT.md, TASK_BOARD.md, tasks/CN-00.md.

## Nhiệm vụ của bạn

Bạn là **implementation owner** của UnityProject, gameplay, scene, build và test. ChatGPT là architecture/review partner, phối hợp qua cùng Task Board và Checkpoint. Không chờ ChatGPT lập kế hoạch lại; roadmap đã chốt trong docs/00_ROADMAP.md.

**NHIỆM VỤ ĐẦU TIÊN:** hoàn thành CN-00 (environment + clean Unity project + Win64 build). Chưa khởi tạo CN-01→CN-06 cùng lúc.

### Các bước theo thứ tự

1. Kiểm tra thư mục Client-New và Git, tuyệt đối không overwrite file hiện có; đọc docs/06_REFERENCE_ENV.md.
2. Kiểm tra Unity Hub/Editor. Phiên bản editor mục tiêu ban đầu: 6000.3.9f1 nếu có sẵn, hoặc cài bản 6000.3 phù hợp qua Unity Hub chính thức. Không đổi phiên bản ngầm.
3. Kiểm tra Windows Build Support, Unity license và quyền thực thi batch build. Nếu thiếu, báo BLOCKED rõ, không ghi CI PASS.
4. Nếu đủ môi trường: tạo project Unity 3D URP tại Client-New/UnityProject; giữ source .meta, Packages/manifest.json và ProjectSettings. Không bê nguyên binary client 4016.
5. Tạo Bootstrap.unity đơn giản: nền kiểm thử, camera, UI trạng thái, không kết nối GameServer thật trong CN-00.
6. Bổ sung asmdef, unit test tối thiểu, README module; dùng assembly separation theo docs/01_ARCHITECTURE.md.
7. Compile thật; mở Editor; build Win64 vào Builds/Windows (Git ignored). Kiểm tra EXE mở được, không phụ thuộc client 4016 hay Frida.
8. Cập nhật TASK_BOARD.md, docs/CHECKPOINT.md, báo cáo trong reports/CN-00_ACCEPTANCE.md, commit vào nhánh ag/cn00-bootstrap (sau khi chắc chắn không làm mất thay đổi).
9. Đưa reviewer lệnh build/test, exit code, logs, hash EXE, ảnh Editor và phần chưa đạt; chờ gate acceptance mới sang CN-01.

## Tránh làm sai

- Không chạy/patch lại client tham chiếu hoặc phát triển trên nhánh jxOnline server.
- Không tạo mock network chỉ để báo login PASS.
- Không tải/trích xuất toàn bộ asset chưa kiểm tra quyền; inventory read-only là đủ CN-00.
- Không đưa một hệ thống WebSocket thật vào production trước khi contract docs/02_PROTOCOL.md được chấp thuận.
- Không reset Bishop, gs001.exe hay ba nhân vật.
- Không tạo project Unity bằng cách chỉ viết vài file cấu hình rồi tuyên bố Editor/Windows build PASS.

## Mẫu báo cáo tối thiểu

- Gate: CN-00; status: PASS / FAIL / BLOCKED.
- Files changed, Unity version và module install path.
- Lệnh compile/build/test với exit codes; test count và log paths.
- Windows binary path, byte size, SHA256 (nếu build thật).
- Các blocker và rollback.
- Commit SHA, branch, git status sạch hoặc lý do khác.

**Next handoff:** reviewer đối chiếu CN-00 theo docs/05_ACCEPTANCE_TESTS.md và ký duyệt trong TASK_BOARD.md.
