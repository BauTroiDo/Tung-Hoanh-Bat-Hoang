# CN02-BULK-07 — Map 53 original PC UnityFS asset and localhost mirror audit

**Ngày:** 26/10/2026, DESKTOP-TPG2RNF, branch `cg/cn02-anchor-evidence`, prior commit `7b1f045`.
**Result:** OFFLINE ORIGINAL PC RESOURCE STRUCTURE PASS; **LIVE SCENE / PLAYER / GAME SERVER MOVEMENT NOT VERIFIED**. CN02-04 **BLOCKED**, CN03 **CLOSED**.

## 1. Nguồn gốc và tiêu chuẩn kiểm tra

Chỉ đọc các file sẵn có của **client 4016**:
- `E:\Games\VLTK3D\SwordOnline\Client-3D-2\update\Game_Data\StreamingAssets\aa\catalog.bin` (627,474 B) và `settings.json` — `m_buildTarget=StandaloneWindows64`, Addressables `2.8.1`.
- `E:\Games\VLTK3D\SwordOnline\Client-3D-2\LAN-LOCAL\Map53Official4016` — hai bundle đã được tải từ nhánh `/upload/Addressable/4016/windows` và chốt SHA256 trong tài liệu M1 của ngày 24/10.
- `LAN-LOCAL/m1_map53_asset_server.py` chỉ được **import, gọi `paths()` và đọc map trong bộ nhớ**, không mở HTTP port, không ghi log hoặc thay cache.
- Client binary gốc `GameAssembly.dll`, metadata và nội dung catalog không thay đổi.

Script mới `tools/reference4016/audit_map53_unityfs.py` xác minh độ dài/SHA256 gốc, tên bundle đúng trong catalog, cấu trúc UnityFS big-endian, giải mã phần block-info bằng LZ4HC và kiểm tra node offset/length. Sau đó **giải nén thử từng block dữ liệu**, đối chiếu kích thước unpacked với bảng block. Không xuất hoặc lưu byte tài nguyên độc quyền ra Git.

## 2. Kết quả hai bundle PC Ba Lăng Huyện

| Chỉ tiêu | `pc_balanghuyen_assets_all` | `pc_balanghuyen_scenes_all` |
|---|---:|---:|
| File trên đĩa | Có, tên đúng catalog | Có, tên đúng catalog |
| Độ dài gốc | 31.592.781 B | 9.738.990 B |
| Pinned SHA256 | **PASS** `73283BA18C14…D07A5DF4D1` | **PASS** `EEB567D492CE…B4DEC4A` |
| UnityFS format / revision | 8 / Unity 6000.3.9f1 | 8 / Unity 6000.3.9f1 |
| Block nén đã giải thử | **349/349 PASS** | **198/198 PASS** |
| Tổng byte đã giải nén thử | **45.637.664** | **25.943.784** |
| Các node trong directory | 2 | 4 |
| Node loại CAB/resS | CAB + resS | CAB + sharedAssets + resS |
| Metadata + node bounds | **PASS** | **PASS** |

**Tổng: 547/547 block giải nén thành công; 71.581.448 byte dữ liệu sau giải nén đã được kiểm tra kích thước.** File `.bundle.part` còn tồn tại cho một số map KHÁC (ví dụ Biện Kinh, Đào Tây Tuy, Giang Tân Thôn); chúng không phải hai bundle PC Ba Lăng Huyện đã được kiểm chứng ở đây.

Đây là bằng chứng mạnh hơn chỉ nhận diện header `UnityFS`, nhưng **không** kiểm tra được nội dung serialized object với Unity engine, liên kết asset/cảnh theo catalog, tình trạng toàn bộ dependency bundle khác, catalog runtime được chọn, scene key Map 53 hay kết quả scene/render thật.

## 3. Mirror preflight an toàn

Import `LAN-LOCAL/m1_map53_asset_server.py` và chạy `paths()` read-only:
- **1.127** tên bundle được phục vụ từ catalog/cache/nhóm official hiện có.
- Trong đó **7** đường dẫn file chính thức trong `Map53Official4016` đã đủ điều kiện proof để đưa vào map của mirror.
- Hai bundle PC Ba Lăng Huyện: **2/2 present**.
- Các file cache khác: **1.120** nguồn `__data` (chỉ kiểm tra chữ ký UnityFS theo logic hiện tại của mirror). Không được khẳng định toàn bộ 1.120 asset đã qua integrity hashing/Unity load.

Live QA preflight **chỉ đọc** (26/10, giờ VN):
- Installed `update/Game_Data/il2cpp_data/Metadata/global-metadata.dat` SHA256 **5586F416…6069E08**, đúng bản gốc.
- Staged `LAN-LOCAL/global-metadata.map53local-4016.dat` SHA256 **C677837D…51BFE1987**, đúng mốc bản localhost.
- Backup metadata gốc SHA256 **5586F416…6069E08**, đúng.
- Trên máy client: `Game.exe` **không chạy**, cổng **14180** (LAN gateway) và **14186** (localhost mirror) **chưa listen**. Cổng 56722/51189 **không listen trên máy client**, điều đó **không chứng minh server ở máy khác đang tắt**.
- Script `Start-M1-Map53-LocalAssets.ps1` có tồn tại nhưng **chưa chạy trong giai đoạn này**. Không thay metadata, không chạy game, không mở port, không chạm Bishop/gs001.exe/DB.

## 4. Kiểm thử mới và mức nghiệm thu

- `tools/reference4016/verify_map53_unityfs.py`: **18/18 PASS** — hash, catalog, phiên bản, node/block counts, toàn bộ block giải nén, LZ4 synthetic (nhánh valid/corrupt), mirror map read-only và giữ báo cáo không chứa bundle payload.
- Kiểm thử CN02 cũ: **238/238** ở checkpoint `7b1f045`; sau khi chạy lại toàn bộ cùng phiên, mục tiêu tổng **256/256**.
- Bằng chứng tóm tắt: `reports/evidence/cn02-4016-il2cpp/map53-offline-assets-summary.json` (metadata/đếm/sizes/digests, không chứa mã/asset).

**Kết luận kỹ thuật:** Không còn phù hợp để gọi hai bundle gốc PC Map 53 là *thiếu hoặc hỏng file nén trên đĩa*. Khả năng còn lỗi là thứ tự/nơi resolver chọn catalog, scene key, nội dung serialized/asset dependency, hoặc luồng client runtime, chưa thể xác định chỉ từ offline audit.

## 5. Bước tiếp theo có điều kiện: một phiên QA Map53 thật

Khi có người điều khiển client:
1. Kiểm tra backend/gateway thực sự sẵn sàng theo môi trường QA được ủy quyền; không restart server thật chỉ vì localhost không listen.
2. Dùng launcher `LAN-LOCAL/Start-M1-Map53-LocalAssets.ps1` *nếu đã rà soát lại hành vi thay metadata và người vận hành đồng ý*, có rollback và hash kiểm tra. **Không khởi động launcher từ báo cáo này**.
3. Người dùng login/nhân vật QA, ghi cùng session/actor, catalog key/asset load callback; xác minh 2 bundles đúng SHA, Unity `SceneReady`, player xuất hiện.
4. Ghi 1 thao tác click di chuyển của người chơi và **ACK/cập nhật vị trí GameServer gốc** rồi client echo.
5. Thu sáu affine coefficients của runtime Map53, ba cặp MPS↔Unity không thẳng hàng **cùng bản đồ** và điểm holdout thứ tư; kiểm thử sai số được người đánh giá xác nhận.
6. Dùng `tools/reference4016/check_single_session.py` để kiểm tra cấu trúc bằng chứng (không đưa credential/token vào log). Kết quả script chỉ là sơ bộ; nghiệm thu đòi kiểm chứng nguồn và hình ảnh.

**Không tái tạo giả tài nguyên, không sửa client gốc, không ép GameServer ACK, không đánh dấu CN02-04 PASS nếu chỉ vì file UnityFS được giải nén thành công.**
