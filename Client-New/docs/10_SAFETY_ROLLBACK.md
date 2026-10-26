# 10 — An toàn vận hành, dữ liệu và hoàn nguyên

## Hệ thống phải giữ nguyên

- Server 192.168.1.69: original Bishop 56722, gs001.exe 51189, đang chứa ba nhân vật thật. Tuyệt đối không reset/xóa/khởi động lại hoặc sửa DB theo CN-00→02.
- Gateway 14180 là bridge thử nghiệm client4016; client mới **chưa có endpoint v1 được phê duyệt**.
- Các client tham chiếu 4016, Clent-3D và Kiếm Võ Giang Hồ đều read-only. Không chép đè dữ liệu trong cây tham chiếu.
- Máy phát triển chỉ ghi dưới Client-New trừ khi có ủy quyền riêng.

## Nội dung bí mật

Không ghi vào Git/chat/report: password, OTP, session ticket GUID thật, MAC/định danh client thô, dữ liệu kinh tế nhân vật, DB dump, private API keys. Logs chỉ nên có session correlation ID không thể dùng login, mapId/entityId khi phù hợp, seq, timestamp, status.

## Thay đổi theo gate

**CN-00→02**: không cần chạm server. Nếu có thay đổi ngoài Client-New, dừng ngay, viết plan và yêu cầu quyền.

**CN-03/04**: Gateway compatibility change cần:
1. Bản sao nguồn/sha256 và command-line startup, status ports/PIDs, số kết nối thực.
2. Branch/test offline + interop mock; không dựng ngầm backend mới.
3. Backup và rollback đã kiểm thử; tách runtime port/feature-flag default OFF.
4. Chỉ triển khai khi không có session active; có bằng chứng người dùng cho phép áp dụng.
5. Không restart Bishop/gs001; nếu không thể triển khai an toàn -> BLOCKED.
6. Verify same GS, player identity and no data mutation.

## Client-New rollback

- Source bug: revert commit hẹp, tests, rebuild Library có thể làm lại. Không xóa Assets source.
- Asset import error: restore scene/meta/prefab từ Git trước import; không copy binary Unity2022 vào Unity6.
- Build issue: build artifact outside Git; không ghi đè existing Game.exe reference.
- Wrong config: edit environment-specific configuration, không chỉnh script chạy production.
- Dependency update: lock Packages version và commit; rollback lockfile/manifest.

## AI agent failure behavior

Nếu tool chặn thao tác ghi/terminal, không dùng Git Bash hoặc đường vòng để né chính sách. Báo rõ block, giữ artifact hiện có. Không hứa "chạy nền" nếu chưa có automation. Không viết PASS khi Editor/GS không được kiểm tra thật.

## Release gate bảo mật

Security lint, secret scan, license/provenance manifest, protocol fuzz/size tests, scene load+movement smoke, rollback procedure. Không push public remote chưa có phép.
