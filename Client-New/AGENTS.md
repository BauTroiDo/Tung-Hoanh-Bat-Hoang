# AGENTS.md — Quy tắc chung cho Antigravity & ChatGPT

## 0. Mệnh lệnh ưu tiên

**Game chơi được trước.** Chỉ một lộ trình CN-00 → CN-06. Không tự mở nhánh gameplay/phó bản/PvP, không viết hàng loạt stub thay vì thực hiện gate hiện tại. Hãy đọc README.md, docs/CHECKPOINT.md và TASK_BOARD.md trong mỗi phiên mới.

## 1. Vùng quyền hạn

**Workspace ghi:** E:/Games/VLTK3D/SwordOnline/Client-New.
**Các client tham chiếu:** chỉ đọc, không chạy hoặc sửa nếu không được chỉ thị rõ:
- E:/Games/VLTK3D/SwordOnline/Client-3D-2/update (Unity 6 / build 4016)
- E:/Games/VLTK3D/SwordOnline/Clent-3D (Tân Võ Lâm PC, Unity 2022)
- E:/Games/PC64bit_剑网江湖MMO_测试_20261021_V25.2 (Kiếm Võ Giang Hồ)
**Server:** 192.168.1.69: Bishop 56722, gs001.exe 51189, Gateway 14180, tham chiếu hiện trạng **read-only mặc định**. Cấm restart/chỉnh Bishop/gs001/DB hay reset nhân vật. Không tạo server/DB nhân vật thứ hai.

## 2. Hợp tác và ghi file

- **Antigravity**: owner mặc định UnityProject/**, Tools/** và build client; có thể sửa docs khi phản ánh sự thật mới.
- **ChatGPT**: owner mặc định docs/**, reports/**, templates/** và công việc review/các adapter kiểm thử riêng.
- Cùng sửa một file: phải có claim rõ trong TASK_BOARD.md; một owner tại một thời điểm, không ghi đè file mà agent khác đang làm.
- Không sửa đè branch/master hoặc mất những thay đổi chưa commit. Trước sửa: kiểm tra git status; commit nhỏ theo milestone.
- Báo cáo file đã đổi, build/test thực tế, commit, rollback. Không tuyên bố đã push GitHub nếu chưa kiểm chứng remote và push.
- Khi cần thay đổi contract chung, sửa docs/02_PROTOCOL.md trước, cập nhật mã và regression sau, review chéo.

## 3. Nghiệm thu dựa bằng chứng

Không đánh dấu PASS từ số lượng test nếu tiêu chí yêu cầu Unity Editor/game thật. Mỗi gate phải có:
- Lệnh build/test đã chạy, exit code, file log/báo cáo.
- Hình ảnh hoặc log Unity runtime đúng đầu vào; không dựng ảnh bằng dữ liệu mock để làm bằng chứng live.
- Với nghiệm thu mạng: cùng phiên/session, cùng entity, map ID, ACK, yêu cầu di chuyển do người dùng click **sau khi vào map** và vị trí GS xác nhận.
- PASS/FAIL/BLOCKED phải phân biệt. Không tự suy luận từ màn hình HUD hoặc auto-movement.

## 4. Code quality

- C# có mã nguồn; module rõ; asmdef tránh phụ thuộc vòng; compile sạch và EditMode tests cơ bản.
- Tách World/Movement khỏi Transport. Không nhúng mật khẩu, IP production, account/token trong asset hoặc source.
- Không viết mã sử dụng tài khoản giả để đánh lừa kết quả nghiệp vụ.
- Không hardcode tọa độ spawn làm chân lý; đặt transform và xác minh từ server bằng test.
- Chuyển động chỉ do request hợp lệ; server authoritative. Chặn replay, oversized frames, malformed data và teleport client-side.
- Mọi prefabs/material/scene .meta của Unity phải được Git quản lý; loại Library/Temp/Logs/Build khỏi Git.

## 5. Tài sản, pháp lý, bản build

- Reverse analysis client tham chiếu là một hướng nghiên cứu, **không đồng nghĩa có quyền tái phân phối asset hoặc mã**. Chỉ nhập asset khi xác minh quyền sử dụng. Manifest nguồn/license bắt buộc.
- Không trộn AssetBundle Unity2022 với Unity6 hoặc patch DLL có sẵn để sửa build mới.
- Không dùng Frida hay binary patch ở sản phẩm phát hành; native client tham chiếu chỉ dùng để đối chiếu.
- Không upload code/asset proprietary vào repo công khai khi chưa được cho phép.
- Web: .NET raw TCP không chạy trong Unity Web; transport qua WebSocket/WSS hoặc phương án Web phù hợp sau khi đã chứng minh PC.

## 6. Khởi động phiên mới — quy trình 8 bước

1. Đọc README, AGENTS, docs/CHECKPOINT, TASK_BOARD.
2. Kiểm tra git status/branch và file đang có, không ghi đè.
3. Xác nhận gate hiện tại; lấy claim cho task.
4. Thiết lập test fail nếu là fix; chỉ sửa phần tối thiểu.
5. Compile và test đúng phạm vi; không báo PASS nếu tool chưa chạy.
6. Cập nhật docs/protocol nếu contract thay đổi.
7. Viết báo cáo & checkpoint có commit, log, rủi ro.
8. Chuyển bàn giao cho agent khác; chỉ mở gate mới khi gate hiện tại đạt.

## 7. Khi môi trường chưa đủ

Không tự khẳng định đã cài Unity. Kiểm tra editor, Hub, Windows Build Support và license trước. Nếu chưa có, lập bằng chứng BLOCKED, vẫn có thể chuẩn bị source/docs. Không tạo kết quả build giả và không chạy bộ cài không tin cậy.

## 8. Quy tắc dừng khẩn cấp

Dừng ngay khi: client tham chiếu bị đổi hash ngoài ý muốn, có tác vụ xóa nhân vật, lộ credential, build can thiệp source server live không phép, dữ liệu asset không có quyền sử dụng, hoặc không rõ ai đang sở hữu file. Ghi lại hiện trạng; không tìm đường lách chặn truy cập.

## 9. Bộ công cụ dịch ngược dùng chung — BẮT BUỘC kiểm tra đầu phiên

**Tool root trên máy Windows của dự án:** `E:\Games\VLTK3D\SwordOnline\Tool`. Thư mục chung đã kiểm tra thực tế 25/10/2026 và có source + ZIP của `Il2CppInspectorRedux-master`, `il2cpp-dumper-rs-master`, `ghidra-master`, `AssetRipper-master`. Đọc `docs/23_REFERENCE_TOOLCHAIN.md` và `Tool/README_CLIENT_NEW.md` trước khi chạy pipeline CN02-BULK. Đây **chưa phải** các ứng dụng đã build/cài; `dotnet`, `cargo`, `rustc`, `java` không có trên PATH khi kiểm kê; luôn kiểm tra lại. Chỉ phân tích client 4016 read-only; bảo vệ asset, UnityProject và server theo quy định trên.
