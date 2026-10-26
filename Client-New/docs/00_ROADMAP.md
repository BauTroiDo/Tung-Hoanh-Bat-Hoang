# 00 — Lộ trình duy nhất Client-New (CN-00 → CN-06)

**Quy tắc:** Làm đến đâu nghiệm thu đến đó. Đây là lộ trình client bên trong roadmap server JX1 đã chốt **M0 → M1/GAME-02 → M2**; không tự sinh dự án server thứ hai. Tất cả giai đoạn phải xuất thành artifact thực tế.

## Sản phẩm mục tiêu

- Một Unity Editor project chỉnh sửa được hoàn toàn ở UnityProject/.
- PC Win64 playable slice: một nhân vật thật, Ba Lăng huyện Map53, địa hình render, collider, camera, click-to-move authoritative.
- Kiến trúc port Web/Mobile không khóa phụ thuộc Windows.
- Không còn bắt buộc chạy Game.exe build4016, binary patch hoặc Frida khi phát hành.

## Milestones và acceptance gates

| ID | Nội dung giới hạn | Bằng chứng bắt buộc | Chuyển bước |
|---|---|---|---|
| **CN-00** | Kiểm kê, Unity Project sạch, asmdef, bootstrap scene và Win64 build | Editor/compile/build/test log + EXE chạy thật | Reviewer ký duyệt |
| **CN-01** | Camera/player/controller/ground collider **offline** | Click thực -> raycast hit -> avatar đổi vị trí, obstacle không xuyên | Không cần backend |
| **CN-02** | Map53 scene edit được: mesh, vật liệu, bounds, collider, spawn | Screenshot Scene/Game View, raycast/ground tests, asset provenance | Không dùng placeholder để giả map |
| **CN-03** | Login/auth/roster/world qua Gateway -> GS thật | Bishop token đúng, 1 existing role, Map53 authoritative entity/snapshot | Không dùng mock PASS |
| **CN-04** | Di chuyển authoritative + reconnect safety | Mouse click timestamp -> MoveRequest -> GS confirm -> client interpolate | Kết quả một click độc lập sau login |
| **CN-05** | Win64 release smoke, perf, tài nguyên và debug | EXE độc lập chạy sau fresh install (không dựa client4016) | PC vertical slice PASS |
| **CN-06** | Web + Android port (iOS gated) | Web/Android builds thực, input, transport, performance, regression | Không tuyên bố iOS PASS thiếu macOS |

## Definition of Ready (DoR)

Trước mỗi CN: có task rõ, owner, file sở hữu, phụ thuộc, test hiện tại, cấu hình môi trường, rollback. Không triển khai song song nhiều CN trong cùng một nhánh.

## Definition of Done (DoD)

1. Có mã/tài nguyên đúng phạm vi, không để TODO làm giả chức năng cốt lõi.
2. Editor compile và build đúng nền tảng; lệnh + exit code có log.
3. Acceptance test có số liệu và bằng chứng nguồn; lỗi hồi quy được kiểm tra.
4. File mới có source/provenance; không lộ credentials hoặc asset thiếu quyền.
5. TASK_BOARD.md cập nhật, reports/CN-NN_*.md, docs/CHECKPOINT.md, commit SHA.
6. Một reviewer khác xác nhận gate; không tự đổi trạng thái PASS khi chưa xem bằng chứng.

## Thứ tự tối ưu

CN-00 → CN-01 → CN-02 và kiểm thử offline Map53 → CN-03 → CN-04 → CN-05 → CN-06.

Một số phân tích asset read-only và thiết kế contract có thể chạy song song, nhưng không coi như gate chính đã xong. Nếu phục dựng asset hợp lệ bị thiếu: ưu tiên scene thử nghiệm **chỉ để kiểm tra gameplay**, đồng thời giữ CN-02 BLOCKED cho địa hình Ba Lăng huyện thật.

## Những việc KHÔNG nằm trong slice đầu tiên

Combat, spells/aura, quest, trade/gold, NPC AI, mining, housing, dungeon/instance, pet, 19 thôn hay toàn thế giới; chỉ thêm sau khi PC slice PASS. Mọi đề nghị mở scope phải qua docs/09_RISKS_DECISIONS.md và TASK_BOARD.

## Chuyển tiếp với server roadmap

M0: Bishop/Auth/Gateway và AOI nền đã có theo các checkpoint cũ, **không giả định hiện tại production đủ chuẩn cho client mới**.
M1/GAME-02: đồng bộ thực KPlayer/KNpc/SubWorld/AOI — Client-New xác nhận hiện thực bằng Map53 và movement.
M2: gameplay nền — Client-New mở rộng sau PC slice, theo hợp đồng server được nghiệm thu.

**Trạng thái ban đầu 25/10/2026:** CN-00 NOT STARTED; không có Editor/build đã xác minh. Các gate khác PENDING.
