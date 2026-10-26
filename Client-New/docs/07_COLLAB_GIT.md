# 07 — Phối hợp ChatGPT / Antigravity, Git và tránh xung đột

## Một task board — một chủ sở hữu file

Tất cả phải đọc AGENTS.md, TASK_BOARD.md và docs/CHECKPOINT.md trước khi sửa. Bảng việc lưu mã task, scope, owner, trạng thái và review; không tự đổi PASS. Nếu tác vụ đang CLAIMED bởi một agent, agent còn lại chỉ review hoặc làm branch độc lập, không sửa trực tiếp.

## Quyền mặc định

| Vùng | Owner thực thi | Reviewer |
|---|---|---|
| UnityProject/Assets/_JXClient | Antigravity | ChatGPT |
| UnityProject/Packages, ProjectSettings | Antigravity | ChatGPT |
| Tools/ & scripts build | Antigravity | ChatGPT |
| docs/01-10, CONTRACT, TESTS | ChatGPT | Antigravity |
| reports/, CHECKPOINT, TASK_BOARD | Người kết thúc gate | Agent đối tác |
| Server/prod, Client-3D-2/update | **READ-ONLY** | Không ai được tự sửa |

File khác owner muốn sửa: ghi một entry "file claim" vào TASK_BOARD, lý do + dự kiến diff. Không được overwrite chồng.

## Git branches

- ag/cn00-bootstrap, ag/cn01-movement, ag/cn02-map53, ag/cn03-client-transport...
- cg/cn00-review, cg/protocol-v1, cg/test-world-coordinates...
- integrate/client-new-CNXX qua PR hoặc merge có review.

Không dùng git reset --hard/clean -fd, không rebase công việc agent khác đang mở. Trước thao tác git: status, branch, repo root; có worktree tốt hơn cùng folder khi song song. Không commit Library/Temp/Unity build/asset chưa có license.

## Quy trình claim/review

1. Tạo/nhận TASK ID; ghi owner + paths chính (ví dụ UnityProject/Assets/_JXClient/Movement).
2. Tạo branch; thực hiện test red/green khi sửa lỗi, ghi ảnh/log đúng.
3. Viết báo cáo vào reports/CN-NN_ACCEPTANCE.md; cập nhật docs/CHECKPOINT.md.
4. Reviewer kiểm tra diff và kiểm thử riêng, viết PASS/REQUEST_CHANGES/BLOCKED.
5. Integrator merge; chạy cả suite, lưu checkpoint/commit. Chỉ lúc đó mới mở next gate.

## Shared state / handoff format

Ghi docs/CHECKPOINT.md theo mẫu:
- Last verified timestamp/timezone, branch/commit, git status.
- Current gate, status and next exact action.
- Files changed, build/test commands+exit, hashes/artifact path.
- What is not proven, known blockers and rollback.
- Source references, security or asset license concerns.

Không gửi password, token, toàn bộ gói wire có thông tin riêng tư, hay raw DB nhân vật vào tài liệu.

## Trong trường hợp xung đột

Nếu cả hai AI sửa một file, dừng merge; xác định owner và so sánh diffs trong branch. Tạo một PR integration, giữ cả thay đổi hợp lệ qua merge có kiểm thử. Nếu không thể xác định tác giả/scope, coi BLOCKED cho tới khi người dùng chọn.

## Các chuẩn commit

- feat(cn00): bootstrap Unity editor project
- feat(cn01): ground raycast movement
- feat(cn02): editable map53 terrain
- test(cn04): authoritative position correlation
- docs(client-new): update protocol and handoff

Commit scope hẹp; không dùng "DONE" nếu gate chưa qua. Push chỉ khi remote/mục tiêu và quyền đã được người dùng duyệt.

## Chất lượng review giữa hai agent

- ChatGPT không ký build PASS bằng cách chỉ đọc code.
- Antigravity không ký protocol PASS khi chỉ có mock backend.
- Bất đồng phải ghi vào docs/09_RISKS_DECISIONS.md, quyết định bằng evidence hoặc hỏi người quản lý, không nhân đôi roadmap.
