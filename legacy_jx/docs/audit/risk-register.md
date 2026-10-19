# Risk Register & Invariant Matrix (PR0 -> PR5)

## 1. Bảng Đăng ký Rủi ro (Risk Register)

| ID | Rủi ro | Mức độ | Nguyên nhân tiềm ẩn | Giải pháp kiểm soát & Bất biến |
| :--- | :--- | :---: | :--- | :--- |
| **R-01** | Teleport lag / Server freeze | **Cao** | Đọc đĩa đồng bộ khi người chơi chuyển map mới chưa nạp | Áp dụng Single-Flight Async Loading + Prewarm portal/teleport + Fallback an toàn |
| **R-02** | Mất hoặc lặp quái (Double Spawn) | **Cao** | Map/Region load lại sinh thêm instance NPC trùng ID | Sinh `spawnId` ổn định 64-bit; bảng hash kiểm tra tồn tại trước khi tạo runtime |
| **R-03** | Lua script dangling pointer | **Nghiêm trọng** | Lua giữ con trỏ thô tới NPC khi map/region bị unload | Dùng handle an toàn `{mapId, spawnId, generation}`; script hook kiểm tra tính hợp lệ |
| **R-04** | Boss / Event toàn server bị ngắt | **Cao** | Bản đồ sự kiện hoặc Boss bị unload khi không có player | Cơ chế Pinned Maps & Event Leases: Boss, Tống Kim, Vượt Ải luôn được ghim trong RAM |
| **R-05** | Respawn timer bị reset | **Trung bình** | Quái chết, map unload, khi load lại quái hồi sinh đầy máu | Quản lý trạng thái respawn tách biệt ở `SpawnDefinition` dựa trên clock timestamp |
| **R-06** | Hysteresis / Thrashing tại biên | **Trung bình** | Người chơi chạy qua lại biên giới giữa 2 region gây load/unload liên tục | Đệm 1 vòng prefetch ring + thời gian chờ Idle TTL (5 phút) trước khi cho phép unload |

---

## 2. Các Bất biến Hệ thống Bắt buộc Tuân thủ (System Invariants)

1. **Bất biến 1 (Gameplay & Protocol):** Giao thức mạng client-server, mã gói tin, định dạng dữ liệu nhân vật, thuộc tính trang bị, công thức sát thương không được phép thay đổi.
2. **Bất biến 2 (Lua Compatibility):** Mọi hàm API Lua hiện có (`GetNpc`, `AddNpc`, `SetPos`, `Msg2Player`...) phải hoạt động chính xác mà không cần sửa file script nhiệm vụ.
3. **Bất biến 3 (Owner-Thread Mutation):** Mọi thao tác thêm/xóa thực thể trên mảng toàn cục (`Npc[]`, `Player[]`, `SubWorld[]`) và máy ảo Lua chỉ được thực thi trên **Main World Thread**, I/O worker chỉ xử lý parse file tĩnh.
4. **Bất biến 4 (Single-Flight Load):** Mỗi `mapId` chỉ có duy nhất 1 tác vụ nạp in-flight tại một thời điểm; cấm nạp trùng lặp.
5. **Bất biến 5 (No Unload Under Conditions):** Tuyệt đối không unload map/region nếu:
   - Có ít nhất 1 người chơi (`m_nNearbyPlayerCount > 0`).
   - Đang có combat active hoặc đạn bay (`MissleList` không rỗng).
   - Có Boss hoàng kim hoặc sự kiện đang được giữ Lease (`isPinned == true`).
