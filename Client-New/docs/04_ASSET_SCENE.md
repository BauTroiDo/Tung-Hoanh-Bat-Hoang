# 04 — Tài nguyên, scene và phục dựng hợp lệ

## Nguyên tắc

Client-New phải có scene/prefab/material chỉnh sửa được trong Unity Editor. Không phát hành AssetRipper dump với asset proprietary khi chưa có quyền sử dụng. Reverse analysis có thể hỗ trợ học cấu trúc; nó KHÔNG cấp license phân phối.

## Reference 4016 (read-only)

E:/Games/VLTK3D/SwordOnline/Client-3D-2/update
- Unity 6000.3.9f1, IL2CPP, source binary (Game.exe + GameAssembly.dll + metadata).
- PAK + Addressables, catalog bản 4016.
- Active scene quan sát qua Unity SceneManager: BLH_Bake 3 khi map ID53.
- Legacy reference có thể tạo nhân vật thật nhưng nền trống và không đi được; **không** coi dữ liệu legacy là ground scene đã hoàn chỉnh.

Hai bundle PC Ba Lăng huyện đã được tìm thấy từ địa chỉ cập nhật chính thức theo device=windows, bản4016:
- pc_balanghuyen_assets_all_53fcdd524b8c67eafad5c013f036ed4b.bundle
- pc_balanghuyen_scenes_all_4e0a8e141624756b20ca4ccfa32087fb.bundle

Dữ liệu trên là nguồn **tham khảo kiểm kê**; không copy vào repo nếu chưa chứng minh quyền sử dụng. Phải xác minh dependency/hash/crypto/scene shader riêng trước khi import.

Client tham khảo 2022 (Clent-3D và Kiếm Võ Giang Hồ V25.2) dùng .bdd / UnityFS, engine Unity2022.3.62f3c1; KHÔNG trộn DLL/AssetBundle nhị phân vào project Unity6. Assets chỉ có thể chuyển sau bước export/reimport hợp lệ + kiểm tra license và bản quyền.

## Quy trình asset theo 5 lớp

**A. Inventory chỉ đọc:** file path, size, format, SHA256, version Unity, source, dependency, license status (unknown/owned/permitted/blocked). Không chép asset riêng tư sang GitHub.

**B. Extract sandbox:** vùng ngoài Assets/ Git, kiểm tra tool trust, size limits, export only whitelisted mesh/material/texture/scene. Không giải mã/bypass DRM chưa được phép; nếu không được phép, dùng asset mẫu do nhóm tạo.

**C. Convert:** glTF/FBX cho mesh, PNG/TGA cho texture theo quyền, chuẩn hóa materials cho URP; với skinned mesh giữ skeleton/rig mapping, animation retarget tests.

**D. Reconstruct scene:** root Ground, Environment, StaticProps, SpawnPoints, Colliders, Lighting, Nav; prefab có tên có thể tra cứu; scene hierarchy có license provenance. Không dùng nguyên khối mesh không thể chỉnh nếu cần hiệu chỉnh từng thành phần.

**E. Validate:** scene loaded, render pipelines, materials, bounds, collider hits, 4 camera angles, light baking, performance và build Win64 thật.

## Metadata provenance tối thiểu

Mỗi asset giữ record:
- assetId, sourcePath, SHA256, engineSourceVersion, extractionToolVersion,
- owner/license/permission evidence,
- convertedOutputPath, dependency list,
- integrityTest, approvedForPrivateUse, approvedForDistribution.

Status unknown => không được check-in output third-party. Khi chưa được phép, fallback là scene & model **được tạo mới từ đầu**; vẫn có thể nghiệm thu CN-01, nhưng CN-02 phải mô tả là scene thay thế chứ không mạo nhận nguyên bản.

## Map53 completion gate

- Inspector hiển thị mesh/collider, Hierarchy và Scene View có mặt đất.
- Game View có địa hình đúng từ điểm spawn và camera, không nền xanh.
- Physics.Raycast from pointer hits ground ở các điểm mẫu, không trúng avatar/UI.
- Nhân vật di chuyển offline quanh địa hình, không xuyên vật cản và không rơi xuyên nền.
- Build Win64 nạp scene từ project; không cần client4016.

## Kế hoạch rollback

Mỗi lần import asset/material lớn thực hiện ở branch riêng; log biến đổi và export manifest. Không xóa cache hoặc file nguồn tham chiếu. Nếu Unity upgrade/import phá scene, revert commit import và rebuild Library, không thay client4016.
