# ADR-003: Selection of Client 3D Engine for JX1 Remastered

## 1. Status
**ACCEPTED** (Target Prototype: **Unity 3D 2022 LTS / 6000 LTS with Universal Render Pipeline - URP**)

---

## 2. Context & Requirements
- **Target Style:** 3D Isometric / $45^\circ$ Oblique Overhead view, preserving classic Võ Lâm Truyền Kỳ 1 gameplay feel (click-to-move, directional skills, fast martial arts combat pacing).
- **Platform Scope:** PC Windows first (Keyboard/Mouse), with clear architectural capability for future Mobile (Android/iOS) touch adaptation.
- **Integration Requirements:**
  - Must seamlessly consume the **Canonical World Bundle** (`world_bundles/`).
  - Must connect to the existing 64-bit GameServer via the lightweight network protocol adapter (`KProtocolAdapter`).
  - Must support rapid asset prototyping, skeletal animation retargeting, isometric camera occlusion culling, and custom sprite/texture shaders.

---

## 3. Evaluated Engine Alternatives

| Evaluation Criteria | **Unity (URP)** | **Unreal Engine 5** | **Godot 4** | **Custom C++ (DirectX/Vulkan)** |
| :--- | :--- | :--- | :--- | :--- |
| **Isometric Rendering & Camera** | **Xuất sắc:** Hỗ trợ orthographic/perspective camera, custom shaders, post-processing nhẹ nhàng. | Tốt, nhưng pipeline Nanite/Lumen quá nặng và thừa thãi cho góc nhìn isometric cố định. | Khá, nhưng 3D pipeline và visual tooling cho MMORPG quy mô lớn chưa trưởng thành. | Phải tự code từ đầu frustum, sorting, shadow maps, UI rendering (rủi ro tiến độ cực lớn). |
| **Tài nguyên & Asset Ecosystem** | **Dồi dào nhất:** Thư viện nhân vật cổ trang, kiếm hiệp, vũ khí, hiệu ứng chưởng pháp phong phú. | Phong phú cho phong cách tả thực (realistic), ít asset võ hiệp cách điệu nhẹ. | Hạn chế, thiếu các gói asset kiếm hiệp sẵn sàng cho production. | Không có ecosystem; phải tự viết importer/parser cho mọi định dạng. |
| **Animation & Blend Trees** | **Mecanim:** Hỗ trợ tuyệt vời 8 hướng di chuyển, blend tree phân tầng (chạy + vung kiếm), mount points ngựa. | Control Rig rất mạnh nhưng phức tạp quá mức cần thiết cho phong cách JX1. | AnimationPlayer đủ dùng nhưng thiếu công cụ retargeting nâng cao. | Phải tích hợp thư viện ngoài (Ozz-animation) tốn nhiều tháng phát triển. |
| **Footprint & Hiệu năng máy yếu** | **Nhẹ:** Khởi động tức thì, build PC chỉ ~150 MB, chạy mượt mà 60–120 FPS trên card đồ họa tích hợp. | Nặng: Cần GPU rời mạnh, dung lượng build > 2 GB, thời gian mở project lâu. | Rất nhẹ, nhưng hiệu năng hạt VFX 3D dày đặc chưa tối ưu bằng URP. | Tùy biến tối đa, nhưng công sức bảo trì và sửa bug driver quá cao. |
| **Chi phí Giấy phép (Licensing)** | Miễn phí bản Personal / Standard Royalty minh bạch; không xung đột giấy phép. | 5% royalty sau ngưỡng doanh thu $1M. | MIT hoàn toàn miễn phí. | Tự do hoàn toàn. |

---

## 4. Decision & Rationale
**Lựa chọn Unity 3D URP** làm engine chính thức để triển khai Vertical Slice Client 3D vì:
1. **Thời gian tạo mẫu nhanh nhất (Fastest Time-to-Playable):** Khả năng nạp trực tiếp dữ liệu từ `manifest.json`, `map.meta.json`, và `spawns.json` vào scene thông qua C# script trong vòng vài ngày.
2. **Kiến trúc Camera hoàn hảo cho Isometric:** Thiết lập dễ dàng camera $45^\circ$ góc nghiêng, khoảng cách 25m, FOV $35^\circ$ kèm bộ lọc làm trong suốt mái nhà/vật cản khi nhân vật đi khuất (`Roof Transparency Occlusion Shader`).
3. **Mạng & Giao thức:** C# `System.Net.Sockets` kết nối trực tiếp socket TCP tới port `51189` của GameServer với zero-copy byte-packing.

---

## 5. Architectural Invariants
1. **Server Authority:** Client Unity 3D chỉ đóng vai trò View & Input Layer; không bao giờ tự ý quyết định lượng sát thương, vị trí hợp lệ hoặc kết quả nhặt đồ.
2. **Dual-Client Compatibility:** Giao thức mạng duy trì kênh tương thích để Client 2D gốc và Client 3D mới có thể cùng tham gia một thế giới game trong quá trình thử nghiệm DEV.
