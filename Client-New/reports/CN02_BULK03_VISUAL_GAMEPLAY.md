# Báo cáo Nghiệm thu BULK-03 — Tự động hóa Kiểm thử Hình ảnh và Gameplay

- **Thời gian thực thi (UTC):** `2026-10-25T23:58:36.3391434Z`
- **Bản chạy thực tế:** `Builds/CN02-Windows/JXClientNew.exe` (`WindowsPlayer`, `Unity 6000.6.7f1`)
- **Thiết bị đồ họa:** `NVIDIA GeForce GTX 1080` (`Direct3D12`, `isBatchMode = false`, cửa sổ `1280x720`)
- **Hiệu năng đo thực tế:** `~1337..1446.6 FPS` (`Managed Memory = 2.57 MB`)
- **Kết quả kiểm thử tổng hợp:** `8/8 kịch bản gameplay PASS`, `9/9 điểm ground probe PASS`, `4/4 ảnh chụp góc camera thật PASS` (`ExitCode = 0`)
- **Kết luận BULK-03:** `PASS`

## 1. Hình ảnh Runtime Thực tế từ Bản Build Win64 (`4/4 PASS`)

| Góc Camera | File bằng chứng | Thông số Camera | Trạng thái quan sát trên ảnh |
|---|---|---|---|
| **Angle 1: South Spawn Overview** | `reports/evidence/cn02-bulk02/bulk03_view1_south_spawn.png` | `Yaw=0, Pitch=45, Dist=18` | Nhân vật tại điểm xuất phát `(0.00, 0.04, -3.00)`, trạng thái `Idle`, quảng trường trung tâm, tường chắn Tây, trụ đèn Đông và cổng Bắc |
| **Angle 2: East Road View** | `reports/evidence/cn02-bulk02/bulk03_view2_east_road.png` | `Yaw=90, Pitch=42, Dist=16` | Nhân vật đã đi bộ sang đường Đông `(5.93, 0.11, -0.04)`, nhìn rõ 2 nhà làng, hàng rào biên và sân đá kín `(16, 0, -16)` (`1347.1 FPS`) |
| **Angle 3: North Elevated Post-Avoidance** | `reports/evidence/cn02-bulk02/bulk03_view3_north_obstacle_routed.png` | `Yaw=180, Pitch=58, Dist=20` | Nhân vật đã tự động tìm đường A* (`3 waypoints`) đi vòng qua `Obstacle_WestWall` sang đích `(-8.47, 0.10, 2.07)` (`1430.5 FPS`) |
| **Angle 4: West Diagonal at North Gate** | `reports/evidence/cn02-bulk02/bulk03_view4_west_northgate.png` | `Yaw=240, Pitch=35, Dist=12` | Nhân vật đi bộ tới trước cổng làng phía Bắc `(0.00, 0.10, 15.92)` (`Intents 5, Rej 3`, `1433.3 FPS`) |

## 2. Kết quả 9 Điểm Kiểm tra Ground / Collider (`9/9 PASS`)

| Tên Probe | Tọa độ ngang | Hit Ground (Layer 8) | Điểm chạm | Pháp tuyến | Collider |
|---|---|---|---|---|---|
| `Probe_CenterPlaza` | `(0.00, 0.00, 0.00)` | `True` | `(0.00, 0.04, 0.00)` | `(0, 1, 0)` | `BLH_CenterPlazaGround` |
| `Probe_NorthGatePath` | `(0.00, 0.00, 18.00)` | `True` | `(0.00, 0.02, 18.00)` | `(0, 1, 0)` | `BLH_NorthSouthRoad` |
| `Probe_SouthGatePath` | `(0.00, 0.00, -18.00)` | `True` | `(0.00, 0.02, -18.00)` | `(0, 1, 0)` | `BLH_NorthSouthRoad` |
| `Probe_EastBridgeApproach` | `(18.00, 0.00, 0.00)` | `True` | `(18.00, 0.03, 0.00)` | `(0, 1, 0)` | `BLH_EastWestRoad` |
| `Probe_WestMarketApproach` | `(-18.00, 0.00, 0.00)` | `True` | `(-18.00, 0.03, 0.00)` | `(0, 1, 0)` | `BLH_EastWestRoad` |
| `Probe_SouthWestPlazaApproach` | `(-6.00, 0.00, -8.00)` | `True` | `(-6.00, 0.00, -8.00)` | `(0, 1, 0)` | `BLH_MainMeadowGround` |
| `Probe_NorthEastPlazaApproach` | `(6.00, 0.00, 8.00)` | `True` | `(6.00, 0.00, 8.00)` | `(0, 1, 0)` | `BLH_MainMeadowGround` |
| `Probe_NorthWestMeadow` | `(-16.00, 0.00, 16.00)` | `True` | `(-16.00, 0.00, 16.00)` | `(0, 1, 0)` | `BLH_MainMeadowGround` |
| `Probe_NorthEastMeadow` | `(16.00, 0.00, 16.00)` | `True` | `(16.00, 0.00, 16.00)` | `(0, 1, 0)` | `BLH_MainMeadowGround` |

## 3. Bảng Kết quả 8 Kịch bản Gameplay Offline (`8/8 PASS`)

| Case ID | Mô tả kịch bản | Bắt đầu | Đích | Kết thúc | Trạng thái | Waypoints | Thời gian | FPS TB | Kết quả |
|---|---|---|---|---|---|---|---|---|---|
| `CASE-01-IDLE-SPAWN` | Kiểm tra trạng thái `Idle` ban đầu tại điểm spawn | `(0.00, 0.04, -3.00)` | `(0.00, 0.04, -3.00)` | `(0.00, 0.04, -3.00)` | `Idle` | `0` | `0.02s` | `-` | `PASS` |
| `CASE-02-WALK-EAST-PLAZA` | Đi bộ từ quảng trường sang đường Đông | `(0.00, 0.04, -3.00)` | `(6.00, 0.00, 0.00)` | `(5.93, 0.11, -0.04)` | `Walking -> Idle` | `1` | `1.66s` | `1337.7` | `PASS` |
| `CASE-03-PATHFIND-AROUND-WEST-WALL` | A* tìm đường vòng qua tường chắn `Obstacle_WestWall` | `(-2.00, 0.05, 2.00)` | `(-8.50, 0.00, 2.00)` | `(-8.47, 0.10, 2.07)` | `Walking -> Idle` | `3` | `2.71s` | `1369.9` | `PASS` |
| `CASE-04-PATHFIND-AROUND-EAST-PILLAR` | A* tìm đường vòng qua trụ đèn `Obstacle_EastLanternPillar` | `(2.00, 0.05, -3.50)` | `(7.50, 0.00, -3.50)` | `(7.43, 0.08, -3.46)` | `Walking -> Idle` | `7` | `1.58s` | `1346.4` | `PASS` |
| `CASE-05-BLOCKED-BY-WALL` | Chuyển động thẳng (tắt pathfinding) dừng an toàn trước tường | `(-2.00, 0.05, 2.00)` | `(-8.50, 0.00, 2.00)` | `(-4.42, 0.11, 2.00)` | `Blocked` | `1` | `0.62s` | `1288.6` | `PASS` |
| `CASE-06-UNREACHABLE-COURTYARD` | Từ chối đích không thể đến trong sân kín `(16, 0, -16)` | `(8.00, 0.08, -12.00)` | `(16.00, 0.00, -16.00)` | `(8.00, 0.08, -12.00)` | `Unreachable` | `0` | `0.03s` | `1381.4` | `PASS` |
| `CASE-07-UI-AND-OFFSCREEN-REJECTION` | Từ chối click trúng HUD UI shield và ngoài màn hình | `(8.00, 0.08, -12.00)` | `UI / Offscreen` | `(8.00, 0.08, -12.00)` | `Unreachable` | `0` | `0.01s` | `1381.4` | `PASS` |
| `CASE-08-WALK-NORTH-GATE` | Đi bộ dọc đường Bắc tới cổng làng `NorthVillageGate` | `(0.00, 0.05, 6.00)` | `(0.00, 0.00, 16.00)` | `(0.00, 0.10, 15.92)` | `Walking -> Idle` | `1` | `2.48s` | `1407.5` | `PASS` |
