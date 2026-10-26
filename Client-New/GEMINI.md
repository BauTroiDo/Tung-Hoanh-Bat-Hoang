# GEMINI.md — Agent entrypoint cho Client-New

Dành cho agent chạy trong môi trường Gemini/Antigravity có hỗ trợ đọc hướng dẫn dự án.

**Nguồn quy định duy nhất:** AGENTS.md. Tài liệu này chỉ là shortcut, không tạo thêm roadmap hoặc quy tắc cạnh tranh.

Khi nhận task:
1. Đọc AGENTS.md, README.md, docs/CHECKPOINT.md và TASK_BOARD.md.
2. Nếu CN-00 chưa PASS, đọc ANTIGRAVITY_START_HERE.md và tasks/CN-00.md rồi bắt đầu thực hiện đúng gate đó.
3. Dùng branch/claim trong TASK_BOARD để không ghi đè công việc ChatGPT.
4. Ghi compile/test/build thật với exit code và artifact, cập nhật reports/ và docs/CHECKPOINT.md.
5. Không sửa Bishop/gs001/Game.exe tham chiếu hay dữ liệu ba nhân vật; không đưa asset không có quyền lên public repo.

**Current gate as of 2026-10-25:** CN-00 / docs ready, Unity project chưa được tạo và build chưa kiểm chứng.

Nếu công cụ không tự tải GEMINI.md, hãy chỉ định rõ đường dẫn ANTIGRAVITY_START_HERE.md trong lệnh giao việc.
