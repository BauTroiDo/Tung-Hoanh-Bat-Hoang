# CHATGPT — Hướng dẫn tiếp nối và review Client-New

Nhiệm vụ: duy trì **một nguồn sự thật** cho hợp đồng, nghiệm thu và handoff; hỗ trợ Antigravity bằng phân tích chuyên sâu và code review dựa trên bằng chứng.

## Bắt đầu phiên

Đọc:
1. README.md, AGENTS.md.
2. docs/CHECKPOINT.md, TASK_BOARD.md.
3. docs/00_ROADMAP.md, gate hiện tại và báo cáo Antigravity.
4. Nếu review code: git status, diff của branch cần review, test outputs (không đoán từ mô tả).

## Quyền sửa

Owner mặc định docs/, reports/, templates/; với UnityProject/ và Tools/ phải nhận claim rõ từ Antigravity trong TASK_BOARD, dùng branch riêng nếu cùng máy.

## Cách nghiệm thu

- CN-00: kiểm tra Editor thực sự mở, compilation và Windows build; file có tồn tại/size, exit code và chương trình chạy được.
- CN-01: game offline với Ground Collider và raycast/click thực, không chỉ scene trống.
- CN-02: địa hình Map53 có renderer/collider, camera đúng bounds, source/license asset rõ.
- CN-03: giữ nguyên 3 nhân vật, Bishop/GS thật; không nhận login mock là PASS.
- CN-04: click sau khi vào game -> lệnh server -> vị trí GS thay đổi -> client áp dụng update cùng entity; có timestamp.
- CN-05: Win64 tự chạy, không cần Frida/4016.
- CN-06: Web/Android real build, transport và input thực; iOS khi có môi trường Mac.

Không có bằng chứng -> PENDING/BLOCKED. Mọi sửa source contract đều cần regression.

## Đầu ra mỗi phiên

1. Kết luận PASS/FAIL/BLOCKED ngắn gọn.
2. Số liệu thực tế, log, diff, commit.
3. Rủi ro và lệnh rollback.
4. File bàn giao trong docs/CHECKPOINT.md; cập nhật task owner/status.
5. Chính xác một bước tiếp theo, không tự mở loạt dự án khác.

## Bằng chứng JX1 hiện tại — legacy reference only

Client 4016 đã từng vào Unity Map53 và tạo avatar, active scene BLH_Bake 3, nhưng địa hình không hiện đúng và người dùng không điều khiển được. Một số vị trí thay đổi ngay lúc login KHÔNG phải click-to-move acceptance. Client-New bắt đầu từ chưa build; không kế thừa trạng thái PASS của client tham chiếu.

**Không copy code/asset proprietary từ runtime tham chiếu vào repo mới nếu chưa xác minh quyền và có manifest.**
