# 08 — Build Windows, Web, Android, iOS và phát hành

## Nguyên tắc một source

UnityProject/Assets/_JXClient chứa gameplay C# và asset được quyền sử dụng; các platform-specific adapters chỉ khác input, transport, chất lượng render và packaging. Không tạo ba gameplay core riêng.

## Phiên bản & packages

CN-00 ưu tiên Unity Editor 6000.3.9f1 tương thích bản tham chiếu 4016; xác minh Editor thực sự có và Windows Build Support. Không khẳng định mọi package URP có version nhất định đến khi đọc manifest từ Project đã khởi tạo.

Chốt exact version Unity, packages, render pipeline và lock file sau build đầu tiên; cập nhật ở reports/CN-00_ACCEPTANCE.md.

## PC (gate CN-00 rồi CN-05)

- Win64 standalone, IL2CPP hoặc Mono choice có benchmark; trong CN-00 có thể Mono để debug, production test phải đúng lựa chọn release.
- Build output Builds/Windows/ bị .gitignore; ghi manifest tên/size/SHA256/version riêng trong reports.
- Scene Bootstrap -> MovementTest -> BaLangHuyen; build includes đúng scene.
- Cold start kiểm tra đường dẫn tương đối, không tham chiếu Client-3D-2 hay files ngoài Client-New (ngoại trừ config server test).
- Logging redacted, gameplay metrics FPS/GC/load time, crash handling.

## Web (sau CN-05)

- Unity Web platform có giới hạn .NET sockets; mặc định design ITransport với WebSocket/WSS.
- Browser HTTPS yêu cầu WSS/TLS; CORS/CSP khi gọi web API; no secret in build.
- File system & multithreading differ; không phụ thuộc Unity native DLL/Frida.
- Asset delivery CDN/cache/AB version, streaming loading; kiểm thử Safari/Chrome/Edge khi có build.

## Android

- ARM64, touch to move, mobile quality and occlusion, reduce draw calls, memory budget.
- App background/resume and network reconnect, permissions tối thiểu, GPU compatibility.
- APK/AAB signing keys ở vault riêng, KHÔNG commit.

## iOS

- Build yêu cầu macOS/Xcode/cert provisioning phù hợp. Nếu máy chỉ có Windows thì iOS là BLOCKED_BY_ENV, không report build PASS.
- Input & lifecycle tương thích, WSS và storage đúng Apple platform; đánh giá shader/touch.

## Quality budget (mục tiêu sơ bộ, phải hiệu chuẩn sau profile)

Các ngưỡng FPS, memory, load time, bundle size được quyết định bằng profiler trên thiết bị tham chiếu và ghi thành metric. Không giả lập con số "60FPS" khi chưa đo.

## Environment configuration

- Development (LAN) vs Stage vs Release server endpoints từ config, không hardcode IP trong gameplay.
- Không nhúng tài khoản hoặc Bishop one-time ticket vào client builds.
- TLS cert policy, rollback environment, version negotiation.
- Không kết nối live server nếu chưa có test plan được chấp thuận.

## Release checklist

Compile/test -> build -> local smoke -> security check -> asset provenance check -> artifact hash/sign -> rollback plan -> user test. Chỉ phát hành PC khi CN-04/05 PASS thực.
