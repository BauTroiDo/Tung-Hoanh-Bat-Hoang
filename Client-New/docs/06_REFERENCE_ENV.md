# 06 — Baseline thực tế và các nguồn chỉ đọc

Ngày khảo sát: 2026-10-25 (giờ Việt Nam). Dữ kiện kiểm tra file đã thấy thực tế, **không** có nghĩa hiện trạng tiến trình/port mãi giữ nguyên.

## Workspace mới

Path: E:/Games/VLTK3D/SwordOnline/Client-New
Tại lúc bắt đầu bộ tài liệu: thư mục tồn tại và trống, không có Git repo hoặc Unity Project. Git executable có ở C:/Program Files/Git/cmd/git.exe và Bash ở Program Files/Git/bin. Unity Hub/Editor **không tìm thấy trong các đường dẫn cài đặt phổ biến đã kiểm tra** (Program Files/Unity Hub, Program Files/Unity/Hub/Editor, AppData/Local/Programs/Unity Hub, vài vị trí khác). Điều này là UNKNOWN, không khẳng định chắc chắn chưa cài ở nơi khác.

**CN-00 phải xác minh Unity Editor và Windows Build Support, nếu thiếu cài qua nguồn chính thức trước build.** Target ban đầu: Unity 6000.3.9f1 / nhánh Unity6.3, test compatibility URP.

## Client tham chiếu chính: 4016

Path: E:/Games/VLTK3D/SwordOnline/Client-3D-2/update
UnityPlayer: 6000.3.9f1. Gameplay IL2CPP.
- Game.exe sha256: 1CAE3982C04BE9D6F8F7EA8CE272600400655D40FBF0671818EFF1D7A79750EE
- GameAssembly.dll sha256: 011357720B210AA1F392A8B62FF8DE090D3A895230A6E0135BCE707196871069
- Metadata global-metadata.dat sha256 original: 5586F416AA26EBE57EA7F6580D0A53E5E5F29C50E425E64A4D3F2A95D6069E08
- Catalog.bin sha256: 61997894DDD843F1FB521F2BEED3FB7E2006874B085E51EA8214C39A6378BB2D (bản build4016 đã so với bộ cài chính thức).

Nguồn Map53 đã được quan sát: original GS world map ID53; Unity active scene BLH_Bake 3; avatar thực và HUD hiển thị, nhưng nền xanh trống, click move không xác nhận. Global EnableCollider=true không đảm bảo địa hình có ground collider. Các đối tượng SceneLoadService._hasMapScene/_loadedWithPcVariant chưa chứng minh value.

## Client tham khảo khác: KHÔNG được trộn trực tiếp

E:/Games/VLTK3D/SwordOnline/Clent-3D
- Tên app Tan Vo Lam PC, Unity2022.3.62f3c1, 73 files .bdd, 57 .pak; GameAssembly 50,354,688 B; IL2CPP metadata 10,257,992 B.

E:/Games/PC64bit_剑网江湖MMO_测试_20261021_V25.2
- Tên app Kiếm Võ Giang Hồ, Unity2022.3.62f3c1, 68 files .bdd, không có .pak trong thư mục kiểm tra; GameAssembly 41,521,664 B; metadata 8,219,372 B.
- Hai bản Unity2022 có chung engine UnityPlayer.dll byte-for-byte, 67 bundle names trùng, nhưng 16 hash mẫu không trùng. Chỉ là nguồn nghiên cứu, **không phải** cùng binary.

## Server tham chiếu (không sửa mặc định)

Máy server được kết nối trước đó: WIN-33R6OP5GO1M / 192.168.1.69.
- Bishop 56722, gs001.exe 51189; Gateway thử nghiệm 14180.
- Original source and staging tại C:/Server-Game/SwordOnline/ trên server.
- Repo người dùng đã dùng cho dự án: https://github.com/BauTroiDo/Tung-Hoanh-Bat-Hoang; không tự chọn repo remote hoặc push chỉ vì URL tồn tại.
- Server roadmap M0 → M1/GAME-02 → M2; không ghi lại live GS khi làm CN-00.

## Cách kiểm tra CN-00 an toàn (PowerShell ví dụ)

~~~powershell
$root = 'E:\Games\VLTK3D\SwordOnline\Client-New'
Get-ChildItem -LiteralPath $root -Force
git --version
Get-Command 'Unity.exe' -ErrorAction SilentlyContinue
Get-ChildItem 'C:\Program Files\Unity\Hub\Editor' -Directory -ErrorAction SilentlyContinue
Get-Process Unity -ErrorAction SilentlyContinue
~~~

Đây là câu lệnh tham khảo, **không phải log đã chạy thành công**. Ghi kết quả vào reports/CN-00_ACCEPTANCE.md.
