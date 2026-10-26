# CN-00 ACCEPTANCE REPORT — Khởi tạo Unity Client-New và Win64 Smoke Build

**Gate/task:** CN-00 / CN00-A..CN00-F
**Owner:** Antigravity
**Reviewer:** ChatGPT
**Environment/timezone/timestamp:** Windows 10/11 x64 (`DESKTOP-TPG2RNF`, GPU NVIDIA GeForce GTX 1080), Asia/Ho_Chi_Minh, 2026-10-25T15:10:00+07:00
**Branch/commit:** `ag/cn00-bootstrap` (baseline `85b4f6c`, reviewer validator `d3da173`, CN-00 bootstrap `55b54b8`)
**Repo clean?** yes (sau khi commit kết quả CN-00)
**Status:** REVIEW (tất cả 7/7 tiêu chí thực thi CN00-01..CN00-07 đạt; chờ ChatGPT review độc lập để chuyển sang PASS theo đúng quy tắc tại `tasks/CN-00.md`)

## Scope and changes

- **Goal and exact scope:**
  1. Khảo sát và xác minh Unity Hub, Unity Editor, license và module Windows Build Support trên máy thực tế.
  2. Khởi tạo Git repository độc lập trong `E:/Games/VLTK3D/SwordOnline/Client-New` (nhánh `ag/cn00-bootstrap`).
  3. Tạo Unity 3D URP project thật tại `E:/Games/VLTK3D/SwordOnline/Client-New/UnityProject` bằng CLI của Unity Editor chính thức.
  4. Thiết lập phân tách assembly (`.asmdef`) trong `Assets/_JXClient/` (`JXClient.Domain`, `JXClient.Core`, `JXClient.Editor`, `JXClient.Tests.EditMode`), tạo `Bootstrap.unity` (Directional Light, `BootstrapTestGround` kèm Collider, `OriginReferenceCube`, `Main Camera`, `BootstrapRunner` + status overlay UI, không kết nối mạng).
  5. Chạy EditMode unit tests bằng Unity Test Runner, build Windows x64 standalone vào `Builds/Windows/JXClientNew.exe`, chạy thực tế bản build Win64 và thu thập log + ảnh chụp màn hình runtime.
- **Files added/modified (paths):**
  - `UnityProject/Assets/_JXClient/README.md`
  - `UnityProject/Assets/_JXClient/Domain/JXClient.Domain.asmdef`
  - `UnityProject/Assets/_JXClient/Domain/GameSessionState.cs`
  - `UnityProject/Assets/_JXClient/Domain/DomainPrimitives.cs`
  - `UnityProject/Assets/_JXClient/Core/JXClient.Core.asmdef`
  - `UnityProject/Assets/_JXClient/Core/BootstrapRunner.cs`
  - `UnityProject/Assets/_JXClient/Editor/JXClient.Editor.asmdef`
  - `UnityProject/Assets/_JXClient/Editor/Cn00ProjectSetup.cs`
  - `UnityProject/Assets/_JXClient/Tests/EditMode/JXClient.Tests.EditMode.asmdef`
  - `UnityProject/Assets/_JXClient/Tests/EditMode/Cn00BootstrapTests.cs`
  - `UnityProject/Assets/_JXClient/Scenes/Bootstrap.unity`
  - `UnityProject/Packages/manifest.json` & `UnityProject/Packages/packages-lock.json`
  - `UnityProject/ProjectSettings/*` & `.meta` files được sinh bởi Unity Editor
  - `reports/evidence/cn00/cn00_bootstrap_smoke.png`
  - `reports/evidence/cn00/editmode_test_results.xml`
  - `reports/evidence/cn00/win64_player_smoke.txt`
  - `reports/CN-00_ACCEPTANCE.md`
  - `TASK_BOARD.md`
  - `docs/CHECKPOINT.md`
- **Explicitly untouched systems:**
  - Server `192.168.1.69` (`Bishop` 56722, `gs001.exe` 51189, `Gateway` 14180) và 3 nhân vật hiện có.
  - Client tham chiếu 4016 (`E:/Games/VLTK3D/SwordOnline/Client-3D-2/update`):
    - `Game.exe` SHA256: `1CAE3982C04BE9D6F8F7EA8CE272600400655D40FBF0671818EFF1D7A79750EE` (unchanged)
    - `GameAssembly.dll` SHA256: `011357720B210AA1F392A8B62FF8DE090D3A895230A6E0135BCE707196871069` (unchanged)
  - Client tham chiếu `Clent-3D` và `PC64bit_剑网江湖MMO_测试_20261021_V25.2`.

## Giải trình phiên bản Unity Editor (`6000.6.7f1`)

- Tại đường dẫn `C:\Program Files\Unity\Hub\Editor\6000.6.7f1\Editor\Unity.exe`, máy được cài đặt chính thức **Unity 6.6 (`6000.6.7f1`, revision `b22767f04f04`)** thông qua `Unity Hub` (`C:\Program Files\Unity Hub\Unity Hub.exe`), kèm sẵn:
  - License hợp lệ: `C:\Users\QT\AppData\Local\Unity\licenses\UnityEntitlementLicense.xml` (`5,880 bytes`)
  - PlaybackEngines: `windowsstandalonesupport` (Win64) và `WebGLSupport`
  - Template 3D URP chính thức: `com.unity.template.urp-blank-17.2.6.tgz` (`com.unity.render-pipelines.universal` `17.6.0`, `com.unity.inputsystem` `1.20.1`, `com.unity.ugui` `2.6.0`, `com.unity.test-framework` `1.8.0`).
- Vì `Client-New` là source project C# độc lập hoàn toàn (không trộn AssetBundle nhị phân từ client 4016 hay Unity 2022), việc sử dụng `6000.6.7f1` trên máy hoàn toàn tương thích với kiến trúc Unity 6 URP đã định nghĩa trong `docs/01_ARCHITECTURE.md` và `docs/08_PLATFORMS_BUILD.md`.

## Evidence table

| Test ID | Expected | Actual | Command/Artifact | Exit code | Status |
|---|---|---|---|---|---|
| CN00-01 | Unity Hub/Editor đúng version & license | `Unity Hub.exe` tại `C:\Program Files\Unity Hub\Unity Hub.exe`; Editor `6000.6.7f1 (b22767f04f04)` tại `C:\Program Files\Unity\Hub\Editor\6000.6.7f1\Editor\Unity.exe`; license `UnityEntitlementLicense.xml` active; `windowsstandalonesupport` + `WebGLSupport` installed | `(Get-Item 'C:\Program Files\Unity\Hub\Editor\6000.6.7f1\Editor\Unity.exe').VersionInfo` | 0 | PASS |
| CN00-02 | Tạo project chuẩn với `Packages`, `ProjectSettings`, `.meta` | Khởi tạo từ `com.unity.template.urp-blank-17.2.6.tgz` tại `Client-New/UnityProject`; đầy đủ `Packages/manifest.json`, `Packages/packages-lock.json`, `ProjectSettings/*` và `.meta` | `Unity.exe -batchmode -nographics -quit -createProject "E:\Games\VLTK3D\SwordOnline\Client-New\UnityProject" -cloneFromTemplate "...\com.unity.template.urp-blank-17.2.6.tgz"` | 0 | PASS |
| CN00-03 | Không compile error trong Editor | Biên dịch sạch các assembly `JXClient.Domain`, `JXClient.Core`, `JXClient.Editor`, `JXClient.Tests.EditMode` và sinh `Assets/_JXClient/Scenes/Bootstrap.unity` (`0 errors`) | `Unity.exe -batchmode -nographics -quit -projectPath "...\UnityProject" -executeMethod JXClient.Editor.Cn00ProjectSetup.GenerateBootstrapScene` | 0 | PASS |
| CN00-04 | EditMode test baseline PASS | `3/3` EditMode tests PASS (`DomainPrimitives_EnforceValidConstraints`, `GameSessionRules_ForbidMoveBeforeWorldReady`, `BootstrapScene_ExistsAndInitializesOfflineReadyState`) | `reports/evidence/cn00/editmode_test_results.xml` | 0 | PASS |
| CN00-05 | Windows64 build thành công | Build `StandaloneWindows64` thành công (`result=Succeeded`, `totalErrors=0`, `totalWarnings=0`), sinh `Builds/Windows/JXClientNew.exe` (`667,136 bytes`, SHA256 `9806ABDE4A406ED68647E9505852C49F6AA72A372F4F084E2AA2821AFFEA57A2`) | `Unity.exe -batchmode -nographics -quit -projectPath "...\UnityProject" -executeMethod JXClient.Editor.Cn00ProjectSetup.BuildWin64` | 0 | PASS |
| CN00-06 | EXE tự khởi chạy, chỉ hiện bootstrap scene | `JXClientNew.exe` khởi chạy trên Direct3D 12 (GTX 1080), nạp scene `Bootstrap`, log `[CN-00][BootstrapRunner] Gate CN-00 | Scene: Bootstrap | State: OfflineReady`, chụp ảnh màn hình thật và thoát sạch với code `0` | `reports/evidence/cn00/cn00_bootstrap_smoke.png`, `reports/evidence/cn00/win64_player_smoke.txt` | 0 | PASS |
| CN00-07 | Git sạch + checkpoint + provenance | Git repo tại `Client-New` trên nhánh `ag/cn00-bootstrap`; `.gitignore` loại trừ `Library/`, `Temp/`, `Logs/`, `Builds/`, `.tmp/`; working tree sạch sau commit | `git status`, `git log -n 2` | 0 | PASS |

## Build & runtime

- **Unity Editor version and install path:**
  - Version: `6000.6.7f1 (b22767f04f04)`
  - Path: `C:\Program Files\Unity\Hub\Editor\6000.6.7f1\Editor\Unity.exe`
- **Exact CLI commands executed:**
  1. **Create Project (URP template):**
     ```powershell
     & "C:\Program Files\Unity\Hub\Editor\6000.6.7f1\Editor\Unity.exe" -batchmode -nographics -quit -createProject "E:\Games\VLTK3D\SwordOnline\Client-New\UnityProject" -cloneFromTemplate "C:\Program Files\Unity\Hub\Editor\6000.6.7f1\Editor\Data\Resources\PackageManager\ProjectTemplates\com.unity.template.urp-blank-17.2.6.tgz" -logFile "E:\Games\VLTK3D\SwordOnline\Client-New\.tmp\unity_create_project.log"
     ```
     Exit code: `0`
  2. **Compile & Generate Bootstrap Scene:**
     ```powershell
     & "C:\Program Files\Unity\Hub\Editor\6000.6.7f1\Editor\Unity.exe" -batchmode -nographics -quit -projectPath "E:\Games\VLTK3D\SwordOnline\Client-New\UnityProject" -executeMethod JXClient.Editor.Cn00ProjectSetup.GenerateBootstrapScene -logFile "E:\Games\VLTK3D\SwordOnline\Client-New\.tmp\unity_generate_bootstrap.log"
     ```
     Exit code: `0`
  3. **Run EditMode Tests:**
     ```powershell
     & "C:\Program Files\Unity\Hub\Editor\6000.6.7f1\Editor\Unity.exe" -batchmode -nographics -projectPath "E:\Games\VLTK3D\SwordOnline\Client-New\UnityProject" -runTests -testPlatform EditMode -testResults "E:\Games\VLTK3D\SwordOnline\Client-New\.tmp\editmode_test_results.xml" -logFile "E:\Games\VLTK3D\SwordOnline\Client-New\.tmp\unity_editmode_tests.log"
     ```
     Exit code: `0` (`total=3, passed=3, failed=0, skipped=0`)
  4. **Build Windows x64 Standalone:**
     ```powershell
     & "C:\Program Files\Unity\Hub\Editor\6000.6.7f1\Editor\Unity.exe" -batchmode -nographics -quit -projectPath "E:\Games\VLTK3D\SwordOnline\Client-New\UnityProject" -executeMethod JXClient.Editor.Cn00ProjectSetup.BuildWin64 -logFile "E:\Games\VLTK3D\SwordOnline\Client-New\.tmp\unity_build_win64.log"
     ```
     Exit code: `0` (`result=Succeeded, totalErrors=0, totalWarnings=0, totalSize=104153315 bytes`)
  5. **Run Win64 Standalone Smoke Test:**
     ```powershell
     & "E:\Games\VLTK3D\SwordOnline\Client-New\Builds\Windows\JXClientNew.exe" -screen-fullscreen 0 -screen-width 1280 -screen-height 720 -cn00SmokeTest -cn00Screenshot "E:\Games\VLTK3D\SwordOnline\Client-New\.tmp\cn00_bootstrap_smoke.png" -logfile "E:\Games\VLTK3D\SwordOnline\Client-New\.tmp\win64_player_smoke.log"
     ```
     Exit code: `0`
- **Binary path / size / SHA256 / timestamp:**
  - Path: `E:\Games\VLTK3D\SwordOnline\Client-New\Builds\Windows\JXClientNew.exe`
  - Size: `667,136 bytes` (tổng thư mục build `Builds/Windows`: `104,153,315 bytes`)
  - SHA256 (`JXClientNew.exe`): `9806ABDE4A406ED68647E9505852C49F6AA72A372F4F084E2AA2821AFFEA57A2`
  - Timestamp: `2026-10-25T15:08:34.3031501+07:00`
- **Runtime real screenshot / trace:**
  - Screenshot: `reports/evidence/cn00/cn00_bootstrap_smoke.png` (`90,389 bytes`, 1280x720)
  - Player log: `reports/evidence/cn00/win64_player_smoke.txt`
  - EditMode XML: `reports/evidence/cn00/editmode_test_results.xml`

## Safety & asset compliance

- **Security check / credentials redacted:** Không có IP production, mật khẩu, ticket hay token trong source code, scene hoặc log.
- **Asset source/license/provenance checked:** Chỉ sử dụng primitive cơ bản của Unity (`Plane`, `Cube`, `Directional Light`, `Camera`) và template 3D URP chính thức của Unity `6000.6.7f1`. Không nhập bất kỳ asset nào từ các client tham chiếu.
- **Original Bishop/GS/client reference unchanged:** Đã xác minh hash SHA256 của `Client-3D-2/update/Game.exe` và `GameAssembly.dll` hoàn toàn không thay đổi.
- **Rollback steps:**
  - Để quay về trạng thái tài liệu trước khi tạo UnityProject: `git checkout 85b4f6cb4fd7d5b8a1876899c9319eac15b75f64` và xóa `Builds/Windows`.

## Findings and next gate

- **Issues found:** Không có lỗi biên dịch hoặc runtime. Bản cài đặt Unity trên máy là `6000.6.7f1` (Unity 6.6) thay vì `6000.3.9f1`, hoạt động ổn định với URP `17.6.0` và Windows Build Support.
- **Blockers/risks:** Không còn blocker kỹ thuật ở CN-00.
- **What remains unproven:** Các gate tiếp theo (`CN-01` offline click-to-move, `CN-02` Map53 terrain, `CN-03` network gateway/auth, `CN-04` authoritative movement) chưa được phép bắt đầu trước khi CN-00 được nghiệm thu.
- **Recommended next exact action:** Reviewer (ChatGPT / người dùng) kiểm tra diff trên nhánh `ag/cn00-bootstrap`, đối chiếu `reports/CN-00_ACCEPTANCE.md` và các file bằng chứng trong `reports/evidence/cn00/`, ghi `reports/CN-00_REVIEW.md` và ký duyệt `PASS` cho CN-00 trước khi mở CN-01.
- **Reviewer decision:** PENDING REVIEW.
