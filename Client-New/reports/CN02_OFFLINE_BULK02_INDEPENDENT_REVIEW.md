# Nghiệm thu độc lập — CN02-OFFLINE-BULK-02

**Ngày:** 2026-10-26, Asia/Ho_Chi_Minh
**Đối tượng:** Antigravity BULK-01 `92df731`, BULK-02 `7389ca4`, BULK-03 `40b2b5b`, BULK-04 `cb7c940` trên branch `cg/cn02-anchor-evidence`.
**Phương thức:** ChatGPT đọc trực tiếp Git, nguồn client, báo cáo, NUnit XML, JSON runtime, hash binary và ảnh trên DESKTOP-TPG2RNF; chạy lại Python unittest. **Không kết nối server / thay UnityProject / rebuild source.**

## Kết luận

**ACCEPTED — OFFLINE GAMEPLAY GATE, WITH CLEAR SCOPE LIMITS.**

- 4 commit SHA tồn tại và checkout sạch tại thời điểm review.
- `Builds/CN02-Windows/JXClientNew.exe`: **667.136 B**, SHA256 `9806ABDE4A406ED68647E9505852C49F6AA72A372F4F084E2AA2821AFFEA57A2`; hash khớp báo cáo.
- `JXClient.Movement.dll`: **19.968 B**, hash `A0CE944F2B8D9A0ECA9B6D917F8D82C2B4A7E2423300FF4CD9E6B0D4D9E563E2`; `JXClient.World.dll`: **41.472 B**, hash `6F468897F47F7D1DFA8F1ADCA72C8A3966832252CC5BC0F3945AB505F4B85207`. **EXE hash trùng launcher bản trước không có nghĩa source cũ; phải kiểm cả DLL/level0.**
- NUnit XML trong `.tmp/cn02-bulk02`: `bulk04-editmode.xml` **30/30 Passed, failed0 skipped0**; `bulk04-playmode.xml` **22/22 Passed, failed0 skipped0**.
- Tự chạy trên checkout mới nhất: `python -m unittest discover -s tools/reference4016 -p 'verify_*.py'` với `PYTHONDONTWRITEBYTECODE=1`: **231/231 PASS**, process exit **0**, thời gian ~12,1s. Có một `ResourceWarning` về kết nối sqlite3 không đóng trong quá trình test; chưa ảnh hưởng kết quả nhưng nên dọn ở phiên hardening.
- Đọc file `reports/evidence/cn02-bulk02/bulk03-gameplay-results.json`: schema `cn02-bulk03-runtime-gameplay-v1`, platform `WindowsPlayer`, GPU GTX1080/D3D12, `isBatchMode=False`, `allPassed=True`, tổng **8 case / 8 passed / 0 failed**, **9/9 groundProbes hitGround**. Có case A* vòng `Obstacle_WestWall` (3 waypoints), `Obstacle_EastLanternPillar` (7), direct movement dừng trước tường, sân kín báo `Unreachable`, click UI/offscreen bị chặn và đi tới cổng Bắc.
- Bốn ảnh `bulk03_view*.png` có tồn tại, kích thước **1280×720**, hash SHA256 trùng chính xác toàn bộ bốn giá trị trong final report: `C67E84279AF1944B46A06CD13722C3B36DA71EA65BC31FD789C0DA7C8F2027D4`, `D4A364DC4692F40BFFF148FBA522111D250409889F82075C86F758F0B594F65A`, `8C537E317B0376F8C667CC59D0B510E32E9CDA620B4B06590AA912A588BFB4F1`, `0E595A677EE0231C9A426658BAA9D8BDA131D005615F546FC967EF31584BFC0A`.
- Đã mở xem trực tiếp ảnh view1 và view3: scene 3D có đường, tường chắn, mô hình kiến trúc dạng khối, nhân vật, HUD ghi `SYNTHETIC FALLBACK`; nội dung thực, nhưng **không phải mô hình/địa hình Ba Lăng Huyện gốc hoàn chỉnh**.

## Giới hạn và việc cần làm

1. **FPS `1446.57` là giá trị harness đo trên scene synthetic rất nhẹ, chạy uncapped**. Không xem đây là FPS người chơi thông thường, benchmark stress, hay mục tiêu tối ưu cho game MMORPG. Cần đo GPU/frame time/CPU memory theo nhiều tình huống có lượng NPC/đối tượng thực.
2. **8 case runtime được tự động hóa**; chưa có nghiệm thu thao tác OS mouse/keyboard/touch dài hạn, camera xuyên tường, pathfinding đối với nhiều vật cản động, va chạm ở map lớn, lỗi spawn/truy cập vùng cao thấp. Đề xuất thêm property-based/seeded scenario và soak.
3. Source `OfflinePathfinder` hiện chạy A* trên lưới 0,5m theo collider Physics; openList chọn phần tử f nhỏ nhất bằng linear scan, query capsule trả về bộ đệm 16 collider. Đây là **nguy cơ về hiệu năng/giới hạn độ bao phủ trong bản đồ dày vật cản**, chưa thấy bug runtime từ 8 case. Nên thiết kế kiểm thử stress hơn trước khi mở rộng map.
4. `OfflineReady` và `CanSendMoveRequest=false` đúng kiến trúc offline, **không cho phép tự mở CN02-04/CN03**.
5. **Original Map 53 restoration NOT VERIFIED:** scene vẫn `IsSyntheticFallback=True`, `CalibrationStatus=Unverified`; không có 6 affine coefficients hoạt động hay 3+1 landmarks MPS↔Unity thật.
6. **SERVER_PAUSED_BY_USER**: do người dùng đang sửa server, không kiểm tra cổng/kết nối/restart Bishop/gs001/gateway, không thay DB hoặc chạy Game.exe tham chiếu; chỉ tiếp tục client offline.
7. Antigravity đã thực hiện và commit BULK-01..04. ChatGPT **không** thay source, không đưa ảnh/binary thô vào Git ở phiên review này. Không có remote Git được cấu hình theo checkpoint trước.

**Định hướng tiếp:** CN02-OFFLINE-HARDENING — bộ kiểm thử đường đi 50–100 tình huống tái lập, đường hẹp/vật cản di động, corner-cutting và buffer 16, click UI/OS thực, camera clipping, đo frame-time với tải nhân vật/NPC mẫu offline. Chỉ triển khai vật liệu/địa hình gốc khi có quyền và chứng cứ đúng asset. Khi server được chủ dự án thông báo sửa xong mới tiến hành nghiệm thu Map53 runtime, không dùng lại log lịch sử làm bằng chứng online.
