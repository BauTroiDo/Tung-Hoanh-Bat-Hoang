# Assets/_JXClient — Cấu trúc Module Client-New

Thư mục này chứa toàn bộ mã nguồn C#, scene và unit tests của `Client-New` theo kiến trúc được quy định tại `docs/01_ARCHITECTURE.md`.

## Phân tách Assembly (`.asmdef`)

1. **`JXClient.Domain`** (`Assets/_JXClient/Domain/`)
   - Thuần C# (`noEngineReferences: true`), không phụ thuộc vào `UnityEngine`.
   - Chứa các kiểu dữ liệu miền cốt lõi và máy trạng thái phiên: `GameSessionState`, `PlayerId`, `MapId`, `WorldPosition`, `MoveIntent`, `BootstrapStateInfo`.
2. **`JXClient.Core`** (`Assets/_JXClient/Core/`)
   - Phụ thuộc `JXClient.Domain` và `UnityEngine`.
   - Chứa `BootstrapRunner` (khởi chạy scene `Bootstrap.unity`, kiểm tra môi trường runtime, mặt nền kiểm thử, camera và hiển thị UI trạng thái).
   - Trong phạm vi gate `CN-00`, `BootstrapRunner` dừng ở trạng thái `OfflineReady` và **không** kết nối GameServer thật hay giả lập đăng nhập.
3. **`JXClient.Tests.EditMode`** (`Assets/_JXClient/Tests/EditMode/`)
   - Phụ thuộc `JXClient.Domain`, `JXClient.Core`, `UnityEngine.TestRunner`, `UnityEditor.TestRunner`.
   - Kiểm thử các ràng buộc giá trị miền (`Domain`), trạng thái khởi tạo `Bootstrap` và tính toàn vẹn của scene `Bootstrap.unity`.
4. **`JXClient.Editor`** (`Assets/_JXClient/Editor/`)
   - Chứa các công cụ hỗ trợ Editor và CLI build (`Cn00ProjectSetup`) phục vụ việc tạo `Bootstrap.unity` chuẩn xác và xuất bản Windows x64 (`Builds/Windows/JXClientNew.exe`).
