# TOOLCHAIN-03 — PATH và build công cụ CN02-BULK

**Thời điểm nghiệm thu:** 2026-10-25 21:47 (Asia/Ho_Chi_Minh). **Máy:** DESKTOP-TPG2RNF. **Branch:** cg/cn02-anchor-evidence.
**Kết quả:** 4/4 bộ công cụ có executable/launcher hoạt động; Ghidra là bản phân phối chính thức, không phải tự build source. Chưa chạy toàn bộ GameAssembly 4016 bằng pipeline.

## 1. Cấu hình môi trường Windows

Đã đặt User JAVA_HOME = C:\Program Files\Java\jdk-25.0.5.0.3.
Đã thêm chính xác một lần vào User PATH, giữ nguyên các mục trước đó:
- C:\Program Files\Java\jdk-25.0.5.0.3\bin
- C:\Program Files\dotnet
- C:\Users\QT\.cargo\bin
Xác minh registry và chạy: javac 25.0.5.0.3, dotnet SDK 11.0.100-rc.2.26504.105, cargo 1.99.1, rustc 1.99.1.
Terminal/AI agent đang mở từ trước có thể cần khởi động lại hoặc nạp lại PATH.

## 2. File thực thi và kết quả

Gốc: E:\Games\VLTK3D\SwordOnline\Tool\_built

| Công cụ | Tệp dùng để chạy | Kiểm thử |
|---|---|---|
| Il2CppInspectorRedux | Il2CppInspectorRedux\Il2CppInspector.Redux.CLI.exe | publish Release win-x64 PASS, --help exit 0 |
| il2cpp-dumper-rs | il2cpp-dumper-rs\il2cpp_dumper.exe | cargo build --release --locked PASS, --version: 8.0.0 exit 0 |
| AssetRipper | AssetRipper\AssetRipper.GUI.Free.exe | publish Release win-x64 PASS, --help exit 0 |
| Ghidra 12.2 | Ghidra\ghidra_12.2_PUBLIC\support\analyzeHeadless.bat | Official SHA-verified ZIP, PE64 import and auto-analysis PASS exit 0 |

SHA256 chính:
- Inspector EXE: 0A191CBD9387E215F1A1126D41669348A0B56B18AEF85B4FE04E2A0C9A79E38D
- Dumper EXE: F054DC9A3C0B4D55220B50A10CACCE23B43D4A09493FD7D95378B2D2B0BDA602
- AssetRipper EXE: D844BEFA4E41D549B2E91A41F69054EDDA5F4368E9343312E4DE2435B328B560
- Ghidra analyzeHeadless.bat: DD7B9D17D32ED70A71DF82A43A21CDAED6C4CE67064E30F8642C149F81C2AE07

## 3. Khắc phục và giới hạn

- Il2CppInspectorRedux: ZIP source thiếu submodule Bin2Object. Clone đúng repo khai báo tại .gitmodules: https://github.com/LukeFZ/Bin2Object, commit dd090deedb57. Publish lại PASS. Không sửa thuật toán IL2CPP.
- il2cpp-dumper-rs: build Release sử dụng toolchain stable x86_64-pc-windows-msvc, output build cô lập dưới %TEMP%\jx1-rust-release-target; copy EXE ra _built.
- AssetRipper: publish -p:PublishAot=false, framework-dependent .NET 10. Các warning CS8032 source generator/TODO vẫn tồn tại nhưng publish và --help PASS. Phải giữ các DLL đi kèm EXE.
- Ghidra: Gradle wrapper 9.8/JDK25 PASS nhưng source build yêu cầu nhiều dependency bên ngoài (YAJSW). Đã dừng các Java/Gradle fetch process do phiên này tạo và dùng bản phát hành Ghidra 12.2 PUBLIC chính thức từ GitHub, không tuyên bố đã build source.
- ZIP chính thức ghidra_12.2_PUBLIC_20261016.zip tại Tool\_downloads, SHA256 từ GitHub Releases API xác minh PASS: 2C2CD776DB3C1F6A097BA418922378AC3649CADD5E844A6F5F088AE628A7BE67.

## 4. Ghidra Headless smoke test

Tạo một chương trình Rust nhỏ tại %TEMP%\jx1-ghidra-smoke-20261025\smoke.exe; dùng analyzeHeadless.bat ở trên với -import smoke.exe -analysisTimeoutPerFile 45 -deleteProject. Log: %TEMP%\jx1-ghidra-smoke-20261025\ghidra.log.
Kết quả EXIT 0; PE64/x86-64 Windows được nhận diện; auto-analysis ~15 giây và log chứa Analysis succeeded / Save succeeded / Import succeeded. Một số cảnh báo pcode/PDB xuất hiện, không khiến test FAIL.

## 5. Gate tiếp theo

4 công cụ đã sẵn sàng chạy thử CLI/headless nhưng chưa xác minh output metadata v39 của chính GameAssembly.dll trên toàn bộ type/method; chưa xác minh maplist, runtime scene, tọa độ Map53 và 3+1 anchors. CN02-04 vẫn BLOCKED; CN03 CLOSED.
Thử CN02-BULK phải thực hiện read-only trên DLL/metadata gốc; giữ output tại khu riêng, giới hạn CPU/RAM; so chéo Inspector và dumper, không đưa binary/asset proprietary vào repo công khai. Không sửa UnityProject, client 4016, Bishop/GameServer hay database.
Build binaries nằm ngoài Git trong Tool/_built. Commit chỉ chứa tài liệu/checkpoint.
**Giám sát thao tác song song:** Khi chốt, git status xuất hiện 8 thay đổi `UnityProject/Assets/_JXClient/**/Materials/*.mat` thuộc lane Unity đang hoạt động. TOOLCHAIN-03 không sửa/không stage/không revert các file này. Vì vậy working tree toàn repo có thể vẫn dirty sau khi commit tài liệu; đừng tự động reset chúng.
