# Báo cáo Nghiệm thu BULK-02 — Hoàn thiện Ba Lăng Huyện Offline Có Thể Chơi

- **Ngày thực hiện:** `2026-10-26`
- **Scene mục tiêu:** `UnityProject/Assets/_JXClient/Scenes/BaLangHuyen.unity` (`SYNTHETIC FALLBACK — NOT ORIGINAL MAP 53 RESTORATION`)
- **Kết quả Unity PlayMode:** `22/22 PASS` (`ExitCode = 0`, bao gồm `18` bài regression cũ + `4` bài kiểm thử đa điểm `Cn02Bulk02OfflineGameplayTests` mới)
- **Kết luận BULK-02:** `PASS`

## 1. Các hạng mục đã triển khai

1. **Hệ thống tìm đường tránh vật cản A* Offline (`OfflinePathfinder.cs`):**
   - Lưới 2D (`GridSize = 56m x 56m`, `CellSize = 0.5m`, `ClearanceRadius = 0.52m`) truy vấn trực tiếp vật lý Unity (`Layer 8 Ground` + `Physics.OverlapCapsuleNonAlloc` loại trừ vật cản `Layer 0 Default`).
   - Tìm đường 8 hướng (Octile A*) chống cắt góc vật cản, kết hợp thuật toán làm mượt đường đi (`SmoothWaypoints` line-of-sight string-pulling với biên an toàn `+0.08m`) giúp nhân vật đi vòng mượt qua tường đá (`Obstacle_WestWall`), trụ đèn (`Obstacle_EastLanternPillar`), nhà làng (`VillageHouse_1..4`) và cổng làng (`NorthVillageGate`, `SouthVillageGate`).
   - Tự động phát hiện và từ chối các mục tiêu không thể đến (`MotionState = "Unreachable"`, `LastInput = "Unreachable target"`), bao gồm điểm nằm trong vật cản đặc hoặc vùng mặt đất bị bao kín bởi tường rào (`Obstacle_EnclosedCourtyard` tại `(16, 0, -16)`).
2. **Tích hợp chuyển động Offline (`OfflineMovement.cs`) tách biệt hoàn toàn với Server-Authoritative:**
   - Hỗ trợ cả click chuột/cảm ứng (`TryPointer(Vector2 screen)`) và điều hướng tọa độ thế giới offline (`TryMoveToWorld(Vector3 worldTarget)`).
   - Duy trì hàng đợi `ActivePath` (`WaypointCount`, `CurrentWaypointIndex`, `FinalDestination`), chuyển tiếp mượt giữa các waypoint cho tới đích cuối.
   - Giữ nguyên tương thích ngược 100% với toàn bộ 18 bài kiểm thử PlayMode của `CN-01` và `CN-02` trước đó.
   - Tách biệt tuyệt đối với giao thức mạng: `SessionState` luôn ở `GameSessionState.OfflineReady` (`GameSessionRules.CanSendMoveRequest == false`).
3. **Hoàn thiện vật cản biên, vùng kín và Camera (`BaLangHuyenSceneController.cs`):**
   - Bổ sung hàng rào biên Đông/Tây (`Perimeter_WestFence`, `Perimeter_EastFence`) ngăn nhân vật đi rơi khỏi địa hình 60m x 60m.
   - Bổ sung khu sân đá kín 4 mặt (`Obstacle_EnclosedCourtyard` tại `(16, 0, -16)`) có mặt đất `Layer 8 Ground` nhìn thấy được nhưng không có lối vào để kiểm thử tự động trạng thái `Unreachable`.
   - Kiểm tra 9 điểm raycast mặt đất trải rộng khắp bản đồ và kiểm tra camera 4 hướng (`Yaw = 0, 90, 180, 270`), zoom (`6..24m`), pitch (`25..75 độ`) trong giới hạn `CameraBounds`.

## 2. Kết quả kiểm thử tự động PlayMode (`22/22 PASS`)

| Bộ kiểm thử | Số lượng | Kết quả | Nội dung xác minh chính |
|---|---|---|---|
| `Cn01SceneTests` | `2/2` | `PASS` | Scene `MovementTest.unity` khởi chạy, di chuyển cơ bản và HUD |
| `OfflineMovementTests` | `8/8` | `PASS` | Click-to-move, tốc độ giới hạn, bước nhỏ, chuột InputSystem, thiếu mặt đất, chắn UI, dừng trước tường, camera orbit/zoom |
| `Cn02OwnershipRegressionTests` | `4/4` | `PASS` | Chặn MoveIntent trước `WorldReady`, epoch/snapshot, single-flight scene load |
| `Cn02WorldSceneTests` | `4/4` | `PASS` | Catalog `BaLangHuyen` synthetic, hủy load an toàn, 7 root + 5 ground probes, dừng trước tường khi tắt pathfinding |
| `Cn02Bulk02OfflineGameplayTests` | `4/4` | `PASS` | 9 điểm ground/collider toàn bản đồ; A* đi vòng qua `Obstacle_WestWall`, `Obstacle_EastLanternPillar`, `VillageHouse_1`; từ chối đích `Unreachable` trong sân kín `(16,0,-16)` và trong nhà; camera 4 hướng + zoom + click-to-move |
