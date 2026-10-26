# 23 — Bộ công cụ dịch ngược tham chiếu JX1 3D (CN02-BULK)

**Kiểm kê thực tế:** 25/10/2026, máy Remote Desktop Commander `DESKTOP-TPG2RNF`.
**Thư mục tuyệt đối (Windows):** `E:\Games\VLTK3D\SwordOnline\Tool`
**Project áp dụng:** `E:\Games\VLTK3D\SwordOnline\Client-New`

## I. Danh mục đã xác nhận tồn tại trên đĩa

| Công cụ | Thư mục nguồn trực tiếp dưới Tool/ | Vai trò | Trạng thái kiểm tra |
|---|---|---|---|
| Il2CppInspectorRedux | `Il2CppInspectorRedux-master` | Phân tích GameAssembly + IL2CPP metadata, tạo sơ đồ types/fields/methods và hỗ trợ Ghidra/IDA | CLI publish Release PASS, đã có `_built/Il2CppInspectorRedux/Il2CppInspector.Redux.CLI.exe` và `--help` exit 0; đã khôi phục submodule Bin2Object |
| il2cpp-dumper-rs | `il2cpp-dumper-rs-master` | Xuất dump class/field/method theo lô, đối chiếu độc lập với Inspector | Rust source + Cargo.toml; đã build Release PASS; `_built/il2cpp-dumper-rs/il2cpp_dumper.exe` trả version 8.0.0 exit 0; chưa nghiệm thu output trên client 4016 |
| Ghidra | `ghidra-master` | Static decompile, call graph, headless batch-script | Source Gradle vẫn có; **đã cài bản Ghidra 12.2 PUBLIC chính thức đã xác minh hash** tại `_built/Ghidra/ghidra_12.2_PUBLIC`; Headless import/auto-analysis PE64 PASS exit 0; KHÔNG phải tự build source |
| AssetRipper | `AssetRipper-master` | Kiểm kê Unity asset, prefab, scene và dependencies (trong phạm vi được phép) | Đã publish Release .NET 10 PASS, `_built/AssetRipper/AssetRipper.GUI.Free.exe --help` exit 0; Unity 6000.3.9f1 nằm trong dải phiên bản công bố nhưng import thực tế chưa kiểm chứng |

Mỗi thư mục trên có ZIP cùng tên, ví dụ `Tool/ghidra-master.zip`, `Tool/Il2CppInspectorRedux-master.zip`. ZIP là bản lưu source, không được coi là executable đã cài.

**Trạng thái môi trường cập nhật 25/10/2026:** Rust **đã cài** bằng WinGet gói chính thức `Rustlang.Rustup` 1.29.1, cài toolchain stable `x86_64-pc-windows-msvc`: `rustc 1.99.1`, `cargo 1.99.1`. Đường dẫn `C:\Users\QT\.cargo\bin` đã được thêm vào User PATH (terminal/agent cũ cần nạp lại PATH hoặc gọi EXE theo đường dẫn tuyệt đối). Biên dịch/chạy chương trình smoke bằng rustc PASS, `cargo metadata --no-deps` và `cargo check --locked` cho source `il2cpp-dumper-rs-master` PASS exit 0, output nằm trong `%TEMP%\jx1-rust-check-target`. **Cập nhật TOOLCHAIN-03:** đã tạo EXE Release dumper 8.0.0; .NET CLI đã tồn tại tại `C:\Program Files\dotnet\dotnet.exe` (SDK 11 RC, .NET 10 targeting packs). User `JAVA_HOME` đã đặt thành `C:\Program Files\Java\jdk-25.0.5.0.3`, User PATH đã bổ sung Java bin và dotnet (đều xác nhận đúng một lần). `javac 25.0.5.0.3`, `dotnet --version` và Cargo chạy thành công theo đường dẫn cài đặt. Bốn bộ tool runnable, xem `reports/TOOLCHAIN_2026-10-25_BUILD.md` và mục cập nhật trong `Tool/README_CLIENT_NEW.md`. Một số `.exe` trong `ghidra-master/GhidraDocs` chỉ là mẫu bài học, không phải binary Ghidra.

## II. Đầu vào tham chiếu 4016 (bắt buộc giữ nguyên)

- `E:\Games\VLTK3D\SwordOnline\Client-3D-2\update\GameAssembly.dll`
- `E:\Games\VLTK3D\SwordOnline\Client-3D-2\update\Game_Data\il2cpp_data\Metadata\global-metadata.dat`
- IL2CPP metadata format v39; chỉ dùng phân tích **read-only**.
- SHA256 baseline:
  - GameAssembly.dll: `011357720B210AA1F392A8B62FF8DE090D3A895230A6E0135BCE707196871069`
  - global-metadata.dat: `5586F416AA26EBE57EA7F6580D0A53E5E5F29C50E425E64A4D3F2A95D6069E08`
- Kiểm lại hash trước mọi phân tích. Không chạy hoặc patch `Game.exe` gốc, không sửa server/DB.

## III. Hướng dùng cho phiên mới

1. Đọc `AGENTS.md`, `docs/NEXT_SESSION_HANDOFF.md`, `docs/CHECKPOINT.md`, `TASK_BOARD.md`, sau đó tài liệu này. Chạy `git status`, xác nhận claim/branch/Unity Editor.
2. Kiểm tra tại chỗ bốn thư mục dưới `Tool`; xác minh compiler/SDK và kiểm tra output binaries trước khi ghi `READY`. **Tồn tại source không đồng nghĩa chạy thành công**.
3. Thiết lập phân tích theo lô trong vùng output riêng thuộc `Client-New/reports/evidence/` hoặc vùng `.tmp/` không commit binary propriet­ary: Inspector/RS để dựng index, Ghidra để phân tích chức năng trọng tâm; chỉ kiểm kê asset khi được phép.
4. Đối chiếu định danh lớp, metadata registry và RVA bằng ít nhất hai công cụ; tách `observed`, `inferred`, `untested` và `BLOCKED`. Với v39, MethodDef token có thể trùng giữa assembly: phải lọc đúng image trước khi gắn tên native.
5. Ưu tiên gameplay vertical slice (đăng nhập → vào map → nhân vật/NPC → di chuyển → chiến đấu), không ép dịch ngược tất cả thư viện Unity/framework hay chờ decompile 100% mới phát triển source mới.
6. Khi sử dụng output làm chứng cứ: ghi tool version/commit nguồn, lệnh thực thi, hash input, số lượng thành công/thất bại, thời gian kiểm thử, đường dẫn output và checkpoint Git.
7. **Giữ CN02-04 BLOCKED / CN03 CLOSED** cho đến khi có hệ số affine Map53 đang hoạt động và 3 landmark non-collinear ghép cặp thật cùng điểm holdout thứ tư; công cụ decompile không thay thế nghiệm thu runtime.

## IV. Kiểm tra tồn tại (chỉ đọc)

```powershell
$ToolRoot = 'E:\Games\VLTK3D\SwordOnline\Tool'
'Il2CppInspectorRedux-master','il2cpp-dumper-rs-master','ghidra-master','AssetRipper-master' |
  ForEach-Object { [pscustomobject]@{ Name=$_; Exists=Test-Path (Join-Path $ToolRoot $_) } }
' dotnet','cargo','rustc','java' | ForEach-Object {
  $n=$_.Trim(); [pscustomobject]@{ Command=$n; Path=(Get-Command $n -ErrorAction SilentlyContinue).Source }
}
```

**Nguồn điều hướng:** `Tool/README_CLIENT_NEW.md` (trong thư mục chung), `Client-New/README.md`, `Client-New/AGENTS.md`, `Client-New/docs/NEXT_SESSION_HANDOFF.md`.

**Phạm vi quyền:** reverse engineering làm tài liệu tham chiếu không tự động tạo quyền sao chép/phát hành asset, source hay dữ liệu độc quyền; không upload proprietary payload vào GitHub. Mọi thay đổi mã, UnityProject hoặc server phải tuân theo claim và nghiệm thu riêng.
