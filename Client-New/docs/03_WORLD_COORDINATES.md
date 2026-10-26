# 03 — Bản đồ, tọa độ và chuyển động

## Phân biệt ba danh tính bản đồ

- **Legacy JX1 world map ID**: Map **53** là Ba Lăng huyện trong server JX1.
- **Unity scene**: client4016 phiên đã từng hiển thị active scene **BLH_Bake 3**.
- **Map list/city/village category**: số lượng thôn (người dùng từng thấy 19) KHÔNG phải tổng số bản đồ.

Không ánh xạ theo thứ tự index của danh sách UI. Dùng bảng map ID -> scene key -> asset dependencies, có version và bài test.

## Tọa độ

Legacy MPS đã quan sát theo hai trục số nguyên X/Y; Unity dùng thế giới 3D (X,Y,Z) với độ cao phụ thuộc mặt đất. Một anchor tham khảo trong nghiên cứu cũ:
- Legacy (52032, 101696)
- Unity position ~ (209.8, 5.904738, 103.71)

Anchor này là gợi ý **CHƯA ĐƯỢC CHỨNG MINH là phép biến đổi duy nhất/hợp lệ cho Client-New**.

## Cách hiệu chỉnh chuẩn

1. Lấy ít nhất **3 điểm không thẳng hàng** có thể định vị chính xác giữa bản JX1 và scene Unity thực, thêm điểm kiểm tra thứ 4.
2. Xác minh map north/rotation, đơn vị legacy, ngược trục, scale, offset, tọa độ âm và biên map.
3. Khớp phép biến đổi affine 2D: [u,v] = A * [legacyX,legacyY] + b; Unity Y (height) lấy từ terrain raycast/height data, KHÔNG lấy từ legacy Y.
4. Đo sai lệch tại các điểm kiểm tra; quyết định ngưỡng scene units bằng tiêu chí gameplay, không đoán.
5. Đóng băng coefficients có đơn vị, version, domain bounds trong cấu hình; test round-trip và out-of-bounds.
6. Không cho client tự đưa mapId/position tùy ý làm authoritative teleport.

## Collider & click-to-move

Offline movement contract:
- Input pointer (mouse/touch) -> camera.ScreenPointToRay -> Physics.Raycast trên **Ground layer**.
- Chỉ hit GroundCollider ở map đang hoạt động mới tạo MoveIntent.
- Nếu không hit: ghi safe diagnostic ("no ground hit"), không tự bay tới tọa độ tùy ý.
- NavMesh/pathfinding nếu được dùng phải bake từ vật cản scene tương thích; không bắt buộc dùng NavMesh để chỉ biểu diễn authoritative server path, nhưng click cần destination hợp lệ.
- Avatar collider không được chặn ray tới nền; UI raycast blocker không được nuốt click gameplay.
- Camera framing/clip planes và spawn bounds được đo trong Editor/PlayMode.

## Map load readiness

MapReady chỉ TRUE khi active scene đúng, root scene có renderer/collider dự kiến, asset status thành công, camera/target hợp lệ, WorldSnapshot đúng map/session, player entity ID được tạo. HUD "Ba Lăng huyện" không đủ.

## Server authoritative movement

1. Click sau khi vào scene ở thời điểm T1.
2. Client log numeric MoveIntent(seq, mapEpoch, target) an toàn.
3. Gateway map chuyển tọa độ có validate và forward tới original GS.
4. GS trả vị trí/tick/ack, client reconcile; đo delta từ T1, không lấy auto spawn movement.
5. Trường hợp rejected, timeout, mapEpoch đổi phải có giải thích, không silently succeed.

## Fixture và tests

- Golden 3+1 scene anchors (đến từ môi trường được kiểm chứng), không copy từ log tùy tiện.
- Test scale/offset/sign, ground height, bounds, missing collider, camera raycast và UI blocking.
- Test move seq replay, speed limits và authoritative correction.
- Map53 test đầu tiên; đa map là nhiệm vụ sau CN-05.

## Source identity audit — 2026-10-25

The historical single pair above is traced to Clent-3D/LAN/world_conversion.py and reference/world_baling.markers.json (Unity2022 world_baling reference). That source itself marks alignment as spawn_anchor and movement_calibrated=False; only BeginPoint01 is present. Default scale 1/32 is not independently surveyed terrain calibration. It cannot be silently reused for client4016 BLH_Bake 3 or the new synthetic BaLangHuyen scene. Numerical points calculated from this one transform are not new independent measurements.

Use the Anchor Evidence (3+1) Editor tool and source fingerprints described in docs/CN02_COORDINATE_EVIDENCE.md. No real calibration has been approved by this audit.
