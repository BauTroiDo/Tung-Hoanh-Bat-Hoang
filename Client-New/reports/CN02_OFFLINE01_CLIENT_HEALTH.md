# CN02-OFFLINE-01 — Nghiệm thu client Unity offline khi server đang bảo trì

**Ngày:** 26/10/2026 (Asia/Ho_Chi_Minh). **Máy:** DESKTOP-TPG2RNF. **Baseline Git:** `0a774d3`.
**Trạng thái server:** **SERVER_PAUSED_BY_USER** — chủ dự án đang tắt/sửa server, tạm ngừng mọi tác vụ trên Bishop, GameServer, gateway, database, tài khoản và thử nghiệm kết nối. Không tự bật lại server hoặc resume CN02-BULK-09/10 nếu người dùng chưa báo server đã ổn.

**Kết quả:** Unity EditMode **29/29 PASS**, Unity PlayMode **18/18 PASS**, Win64 Player **build thành công**, kiểm toán asset GUID **15/15 PASS**, tổng bộ Python kiểm thử lưu trữ/static trước đó cùng mới **305/305 PASS**. Các kết quả Python và Unity là hai nhóm khác nhau, không cộng gộp thành nghiệm thu multiplayer.

## 1. Phạm vi và cách bảo vệ

- Workspace `E:\Games\VLTK3D\SwordOnline\Client-New`, branch `cg/cn02-anchor-evidence`.
- Unity Editor thật: `C:\Program Files\Unity\Hub\Editor\6000.6.7f1\Editor\Unity.exe`.
- Mọi nguồn UnityProject/** được **đọc và build**, không chỉnh sửa. Tám file material chưa commit thuộc Antigravity được giữ nguyên:
  `BLH_BoundaryRock.mat`, `BLH_GrassTerrain.mat`, `BLH_HouseWall.mat`, `BLH_PlazaTile.mat`, `BLH_StoneRoad.mat`, `BLH_TileRoof.mat`, `BLH_WoodTimber.mat`, `Avatar.mat`.
- Không kiểm tra TCP, không gọi/khởi động bất cứ process nào trên server, không thao tác `Game.exe` tham chiếu, không thay PAK/Unity metadata, không nhập dữ liệu sở hữu của client 4016.
- Toàn bộ log/test/build được xuất vào `Client-New\.tmp\cn02-offline01\` (ngoài danh mục version-control), không ghi đè những build trước.

## 2. Kết quả Unity Editor và build

| Nội dung | Kết quả | File log / bằng chứng |
|---|---|---|
| EditMode test trực tiếp trong Editor | **29/29 Passed; 0 Failed, 0 Skipped** | `.tmp/cn02-offline01/editmode.xml`, `editmode.log` |
| PlayMode test trực tiếp trong Editor | **18/18 Passed; 0 Failed, 0 Skipped** | `.tmp/cn02-offline01/playmode.xml`, `playmode.log` |
| Build Unity Win64 từ source | **Batch completed, exit 0** | `.tmp/cn02-offline01/build-win64.log` |
| Win64 Player file | `JXClientNew.exe`, **667.136 B** | `.tmp/cn02-offline01/Builds/JXClientNew.exe` |
| File phụ thuộc Unity Player | `UnityPlayer.dll` và `JXClientNew_Data\` **có tồn tại** | `.tmp/cn02-offline01/Builds` |
| SHA256 Win64 exe | `9806ABDE4A406ED68647E9505852C49F6AA72A372F4F084E2AA2821AFFEA57A2` | đã đo lại |

**Câu lệnh áp dụng:** Unity `-batchmode -projectPath "<UnityProject>" -runTests -testPlatform EditMode -testResults <.../editmode.xml> -logFile <.../editmode.log>`; PlayMode thêm `-nographics` và `-testPlatform PlayMode`; build thêm `-quit -nographics -buildWindows64Player <.../Builds/JXClientNew.exe>`. Chạy tuần tự, không đè Editor phiên khác. Sau mỗi lần test/build, Editor batch đã đóng.

Đây là **compile/build và bộ kiểm thử Unity thực**, nhưng **chưa** kiểm tra chạy `JXClientNew.exe` tương tác trong cửa sổ PC hay một ảnh chụp runtime mới của build này. Scene `BaLangHuyen.unity` đang được mô tả là **SYNTHETIC FALLBACK**, không thể gọi đó là địa hình BLH gốc hoàn thiện.

## 3. Kiểm toán integrity scene, prefab và material

Đã bổ sung công cụ **read-only** `tools/reference4016/audit_unity_offline.py`, kiểm tra YAML/GUID của 3 scene offline, prefab, material, asset trong `Assets/_JXClient` và `Library/PackageCache` với metadata của Unity.

| Kiểm tra | Số liệu |
|---|---:|
| Project `Assets/**/*.meta` | **90** |
| `Library/PackageCache/**/*.meta` | **9.691** |
| File Unity YAML có thể chứa tham chiếu đã kiểm tra | **16** |
| Tham chiếu tới project Assets | **120** |
| Tham chiếu tới Unity packages | **56** |
| Tham chiếu Unity built-in | **35** |
| Tham chiếu GUID chưa giải được trong phạm vi quét | **0** |
| Asset thiếu `.meta` | **0** |
| GUID của project bị trùng | **0** |

Tổng cộng **211 tham chiếu** đã phân loại. Công cụ cũng lưu danh sách file, hash, Unity version và status build/test; **không** ghi raw mesh/texture, body C# hay UnityFS. `tools/reference4016/verify_unity_offline.py`: **15/15 test PASS**. Kết quả: `reports/evidence/cn02-offline01/client-offline-health.json`.

Các file/script không thuộc phạm vi YAML scan, một số đối tượng Unity built-in đặc biệt và tương tác shader/runtime vẫn có thể lỗi dù GUID resolved; vì vậy "0 unresolved" không đồng nghĩa tất cả cảnh đã render đúng.

## 4. Hồi quy CN02 còn lại

Chạy tuần tự các bộ kiểm thử Python read-only trước đó, cộng 15 bài offline mới: **305/305 PASS**, tất cả exit 0. Trong số này có bài kiểm tra **bằng chứng lịch sử** của CN02-BULK-09, tuyệt đối không suy diễn đó là kết nối server thực hiện lại trong phiên offline này.

## 5. Quyết định và việc kế tiếp

- **CN02-OFFLINE-01:** PASS ở mức kiểm thử code/Unity Editor/Win64 build.
- **CN02-04:** BLOCKED như trước: chưa có 6 hệ số affine runtime và 3+1 mốc thật, scene original Map53 và ACK di chuyển từ GS thật.
- **CN03:** CLOSED như trước.
- **Server:** **SERVER_PAUSED_BY_USER**; không tiếp tục CN02-BULK-10, không tự test Bishop/Gateway/GS hoặc resume các PID cũ trong handoff. PID của CN02-BULK-09 chỉ là **bằng chứng lịch sử**, có thể đã bị người dùng tắt/sửa; không assume còn sống.
- **Luồng offline kế tiếp:** dùng chính Win64 build để kiểm tra startup/render/camera và collider một cách offline; sau đó rà soát UI click-to-move, NavMesh/pathfinding, và cải thiện scene synthetic **trong lane Antigravity** hoặc task được giao rõ. Nếu chưa được giao owner UnityProject thì chỉ tạo tài liệu, kiểm thử và báo cáo, không sửa các file thuộc Antigravity.
- **Điều kiện mở lại mạng:** chỉ sau khi người dùng thông báo server sửa xong; kiểm tra lại host/port, hash/runtime/DB readiness từ đầu, không dùng snapshot ngày 26/10 làm bằng chứng "đang chạy".

Rollback: xóa riêng `.tmp/cn02-offline01` nếu không cần kết quả build/test; chỉ hoàn tác commit các công cụ/ báo cáo `CN02-OFFLINE-01` nếu muốn trở lại. **Không** revert material hay xóa cache/source Unity của Antigravity.
