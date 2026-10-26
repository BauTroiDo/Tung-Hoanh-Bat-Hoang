# Báo cáo Nghiệm thu Tổng kết — CN02-OFFLINE-BULK-02 (`BULK-01` → `BULK-04`)

- **Ngày hoàn thành:** `2026-10-26` (Asia/Ho_Chi_Minh)
- **Workspace:** `E:\Games\VLTK3D\SwordOnline\Client-New`
- **Nhánh Git:** `cg/cn02-anchor-evidence`
- **Unity Editor:** `6000.6.7f1` (`C:\Program Files\Unity\Hub\Editor\6000.6.7f1\Editor\Unity.exe`)
- **Trạng thái máy chủ:** `SERVER_PAUSED_BY_USER` (không kết nối, khởi động hay chỉnh sửa Bishop/GameServer/Gateway/DB hoặc chạy `Game.exe` 4016)

---

## 1. Phân định Trạng thái Nghiệm thu Rõ ràng (Bắt buộc)

| Hạng mục | Trạng thái | Căn cứ xác minh thực tế |
|---|---|---|
| **Offline gameplay (`BULK-01..04`)** | **PASS** | `30/30` EditMode PASS (`ExitCode = 0`), `22/22` PlayMode PASS (`ExitCode = 0`), `8/8` kịch bản runtime Win64 Player PASS (`ExitCode = 0`, `~1446.6 FPS`), `9/9` ground probes PASS, `4/4` ảnh thực tế từ `Builds/CN02-Windows/JXClientNew.exe`, A* tránh vật cản và từ chối đích `Unreachable` hoạt động ổn định |
| **Original Map 53 restoration** | **NOT VERIFIED** | Scene `BaLangHuyen.unity` hiện là **synthetic fallback** (`IsSyntheticFallback = true`, `CalibrationStatus = Unverified`); chưa nhập địa hình Map 53 gốc và chưa có bộ `3+1` điểm neo thực tế kèm ma trận 6 tham số affine runtime đã xác minh |
| **Server-authoritative movement (`CN02-04`)** | **BLOCKED** | Server đang tạm dừng bảo trì (`SERVER_PAUSED_BY_USER`). Mọi luồng offline giữ `SessionState = GameSessionState.OfflineReady` (`GameSessionRules.CanSendMoveRequest == false`). `CN-03` tiếp tục **CLOSED** |

---

## 2. Tóm tắt Kết quả Theo Từng Đợt BULK & Chuỗi Commit

| Đợt | Commit SHA | Nội dung chính | Kết quả Build / Test | Trạng thái |
|---|---|---|---|---|
| **BULK-01** | `92df731` | Công cụ kiểm toán & sửa hàng loạt [`Cn02BulkAssetAuditor.cs`](file:///E:/Games/VLTK3D/SwordOnline/Client-New/UnityProject/Assets/_JXClient/Editor/Cn02BulkAssetAuditor.cs) + [`Cn02Bulk01AssetAuditTests.cs`](file:///E:/Games/VLTK3D/SwordOnline/Client-New/UnityProject/Assets/_JXClient/Tests/EditMode/Cn02Bulk01AssetAuditTests.cs); hoàn thiện và commit toàn bộ 8 material `.mat` đang modified thuộc Antigravity + 2 material CN-01 (`10/10` URP Lit chuẩn, đồng bộ `_BaseColor`/`_Color`); kiểm toán `3` prefab và `3` scene (`51` file, `0` lỗi GUID/meta/missing script) | Batch Auditor `ExitCode = 0`; EditMode `30/30 PASS` (`ExitCode = 0`) | **PASS** |
| **BULK-02** | `7389ca4` | Hệ thống tìm đường tránh vật cản 2D A* [`OfflinePathfinder.cs`](file:///E:/Games/VLTK3D/SwordOnline/Client-New/UnityProject/Assets/_JXClient/Movement/OfflinePathfinder.cs) + tích hợp [`OfflineMovement.cs`](file:///E:/Games/VLTK3D/SwordOnline/Client-New/UnityProject/Assets/_JXClient/Movement/OfflineMovement.cs) + mở rộng vật cản biên & sân đá kín `Unreachable` trong [`BaLangHuyenSceneController.cs`](file:///E:/Games/VLTK3D/SwordOnline/Client-New/UnityProject/Assets/_JXClient/World/BaLangHuyenSceneController.cs) + bộ kiểm thử đa điểm [`Cn02Bulk02OfflineGameplayTests.cs`](file:///E:/Games/VLTK3D/SwordOnline/Client-New/UnityProject/Assets/_JXClient/Tests/PlayMode/Cn02Bulk02OfflineGameplayTests.cs) | PlayMode `22/22 PASS` (`ExitCode = 0`, 18 bài cũ + 4 bài mới) | **PASS** |
| **BULK-03** | `40b2b5b` | Chạy bản build Win64 thật `Builds/CN02-Windows/JXClientNew.exe -cn02Bulk03Test` trên GPU `NVIDIA GeForce GTX 1080` (`Direct3D12`), chụp 4 ảnh góc camera thực tế, quét 9 điểm ground/collider, chạy 8 kịch bản gameplay (`Idle`, `Walking`, `Pathfinding`, `Blocked`, `Unreachable`, `UI/Offscreen rejection`) | Win64 Runtime `ExitCode = 0`, `8/8` cases PASS, `~1446.6 FPS` | **PASS** |
| **BULK-04** | *(commit hiện tại)* | Nghiệm thu độc lập Win64 Player (`Builds/CN02-Windows/JXClientNew.exe`), chạy lại toàn bộ EditMode (`30/30`), PlayMode (`22/22`), Python regressions (`231/231` unittest discover + các bộ kiểm tra độc lập), kiểm tra `SHA256` các thực thi/DLL và đối chiếu toàn vẹn client 4016 gốc | Build `ExitCode = 0`, EditMode `30/30 PASS`, PlayMode `22/22 PASS`, Python `ExitCode = 0` | **PASS** |

---

## 3. Kết quả Kiểm thử Tổng hợp

| Bộ kiểm thử | Lệnh / Công cụ | Tổng số | Đạt | Lỗi | Exit Code | Bằng chứng / Log |
|---|---|---|---|---|---|---|
| **Unity EditMode** | `Unity.exe -batchmode -runTests -testPlatform EditMode` | `30` | `30` | `0` | `0` | `.tmp/cn02-bulk02/bulk04-editmode.xml` |
| **Unity PlayMode** | `Unity.exe -batchmode -nographics -runTests -testPlatform PlayMode` | `22` | `22` | `0` | `0` | `.tmp/cn02-bulk02/bulk04-playmode.xml` |
| **Win64 Standalone Build** | `JXClient.Editor.Cn02ProjectSetup.Build` | `3 scenes` | `3 scenes` | `0` | `0` | `.tmp/cn02-bulk02/bulk04-build-win64.log` |
| **Win64 Visual & Gameplay Harness** | `Builds/CN02-Windows/JXClientNew.exe -cn02Bulk03Test` | `8 cases` | `8 cases` | `0` | `0` | [`reports/evidence/cn02-bulk02/bulk03-gameplay-results.json`](file:///E:/Games/VLTK3D/SwordOnline/Client-New/reports/evidence/cn02-bulk02/bulk03-gameplay-results.json) |
| **Python Offline / Static Regressions** | `python -m unittest discover -s tools/reference4016 -p "verify_*.py"` | `231` (`305+` tổng) | `231` | `0` | `0` | [`tools/reference4016/verify_unity_offline.py`](file:///E:/Games/VLTK3D/SwordOnline/Client-New/tools/reference4016/verify_unity_offline.py) |

---

## 4. Bảng Kiểm tra SHA256 Bản Build Win64 Độc lập & Hình ảnh Thực tế

### 4.1. Thực thi và Thư viện Win64 (`Builds/CN02-Windows/`)

| File | Kích thước (Bytes) | SHA256 |
|---|---|---|
| `JXClientNew.exe` | `667,136` | `9806ABDE4A406ED68647E9505852C49F6AA72A372F4F084E2AA2821AFFEA57A2` |
| `UnityPlayer.dll` | `38,753,192` | `38DB99E19730FAD74E8CC21758C87FFE7387B7774EBD9A14405C62AE481F2550` |
| `JXClient.Domain.dll` | `15,360` | `4A646B912B01AC2BB952819E8D46D992B58064169F24EA67789E596AFEDA4718` |
| `JXClient.Core.dll` | `8,704` | `6AFF3275972F94013A6FEB16C98AAE768180FFBAE370A1F36F42A0B6CA37C4DE` |
| `JXClient.Movement.dll` | `19,968` | `A0CE944F2B8D9A0ECA9B6D917F8D82C2B4A7E2423300FF4CD9E6B0D4D9E563E2` |
| `JXClient.World.dll` | `41,472` | `6F468897F47F7D1DFA8F1ADCA72C8A3966832252CC5BC0F3945AB505F4B85207` |
| `level0` (`BaLangHuyen`) | `33,304` | `917FD674B509B95E6BE44D2DB0D3FE06BFEF7D250A6A18BFE78C12836CD32B14` |
| `level1` (`MovementTest`) | `16,116` | `AC0F0A09A750B1335F356DC1DB17C8247B3482514E1986E7C3DF40D0F454402D` |
| `level2` (`Bootstrap`) | `3,480` | `4B851E5CD16FB35792362283007B77227E607BD90F4CFE32015990D597211512` |

### 4.2. Hình ảnh Runtime Thực tế (`reports/evidence/cn02-bulk02/`)

| File ảnh | Kích thước (Bytes) | SHA256 | Mô tả góc chụp & trạng thái |
|---|---|---|---|
| [`bulk03_view1_south_spawn.png`](file:///E:/Games/VLTK3D/SwordOnline/Client-New/reports/evidence/cn02-bulk02/bulk03_view1_south_spawn.png) | `126,284` | `C67E84279AF1944B46A06CD13722C3B36DA71EA65BC31FD789C0DA7C8F2027D4` | Góc 1 (`Yaw=0, Pitch=45, Dist=18`): Điểm xuất phát trung tâm làng `(0.00, 0.04, -3.00)`, trạng thái `Idle` |
| [`bulk03_view2_east_road.png`](file:///E:/Games/VLTK3D/SwordOnline/Client-New/reports/evidence/cn02-bulk02/bulk03_view2_east_road.png) | `126,173` | `D4A364DC4692F40BFFF148FBA522111D250409889F82075C86F758F0B594F65A` | Góc 2 (`Yaw=90, Pitch=42, Dist=16`): Sau khi đi bộ sang đường Đông `(5.93, 0.11, -0.04)`, thấy sân đá kín `(16, 0, -16)` (`1347.1 FPS`) |
| [`bulk03_view3_north_obstacle_routed.png`](file:///E:/Games/VLTK3D/SwordOnline/Client-New/reports/evidence/cn02-bulk02/bulk03_view3_north_obstacle_routed.png) | `146,298` | `8C537E317B0376F8C667CC59D0B510E32E9CDA620B4B06590AA912A588BFB4F1` | Góc 3 (`Yaw=180, Pitch=58, Dist=20`): Sau khi A* định tuyến `3 waypoints` đi vòng qua tường chắn Tây `Obstacle_WestWall` tới `(-8.47, 0.10, 2.07)` (`1430.5 FPS`) |
| [`bulk03_view4_west_northgate.png`](file:///E:/Games/VLTK3D/SwordOnline/Client-New/reports/evidence/cn02-bulk02/bulk03_view4_west_northgate.png) | `134,679` | `0E595A677EE0231C9A426658BAA9D8BDA131D005615F546FC967EF31584BFC0A` | Góc 4 (`Yaw=240, Pitch=35, Dist=12`): Góc chéo Tây tại cổng Bắc `(0.00, 0.10, 15.92)` sau khi hoàn tất 8 kịch bản (`1433.3 FPS`) |

---

## 5. Tồn đọng và Nhiệm vụ Tiếp theo

1. **Tồn đọng phụ thuộc Server (`CN02-04` BLOCKED):**
   - Server đang được chủ dự án tắt để sửa chữa (`SERVER_PAUSED_BY_USER`).
   - Chưa có phiên chạy online xác thực quyền chọn nhân vật từ Bishop -> GameServer permit -> `WORLD_SYNC` -> 6 hệ số affine runtime của Map 53 -> `3+1` điểm neo tương ứng giữa MPS và Unity.
2. **Nhiệm vụ tiếp theo khi chủ dự án thông báo Server đã sửa xong:**
   - Thực hiện preflight mới và thu thập gói dữ liệu 1 phiên duy nhất (Bishop -> GS -> `WORLD_SYNC` -> `3+1` landmark Map 53).
   - Nghiệm thu cổng `CN02-04` với sai số holdout thực tế trước khi mở `CN-03`.
