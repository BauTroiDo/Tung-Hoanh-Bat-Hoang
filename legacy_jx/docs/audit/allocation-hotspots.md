# Memory Allocation Hotspots & Root Causes Analysis (11 GB Breakdown)

## 1. Phân rã định lượng bộ nhớ (Memory Attribution)

Trước khi tối ưu, tiến trình `gs001.exe` chiếm **10.91 GB Working Set** (~11.82 GB Private Commit).
Dưới đây là bảng phân rã chi tiết từng thành phần bộ nhớ dựa trên số liệu profiling thực tế:

| Thành phần | Cơ chế cấp phát | Kích thước thực tế | Tỷ lệ % RAM | Tình trạng / Điểm nghẽn |
| :--- | :--- | :---: | :---: | :--- |
| **1. KRegion Heap Fragmentation** | `new int[512]` x 2 trong `KRegion::Init` | **~1.25 GB** | 11.5% | **ĐÃ GIẢI QUYẾT (PR0-Fix1):** 1.4 triệu lần `new` sinh ra 40.642 chunk rác heap. Đã xóa bỏ chuyển sang `_KGridData`. |
| **2. Static Trap Arrays (`m_dwTrap`)** | `int m_dwTrap[16][32]` trong mọi KRegion | **~620 MB** | 5.7% | **ĐÃ GIẢI QUYẾT (PR0-Fix2):** 2.048 byte tĩnh x 700.000 region. Đã chuyển sang con trỏ on-demand `KTrapGrid*`. |
| **3. Static Arrays (Globals)** | `new[]` toàn cục trong `g_InitCore` | **2.21 GB** | 20.3% | `KNpc[150k]` (1.22 GB), `SubWorld[1k]` (634 MB), `KItem[200k]` (234 MB), `KPlayer` (76 MB), `KMissle` (41 MB). |
| **4. Eager Map Loading (`m_Region`)** | `new KRegion[totalRegion]` cho 770 map | **~3.80 GB** | 35.0% | **TRỌNG TÂM PR2:** Toàn bộ 770 map đều khởi tạo ~700.000 Region ngay khi boot, dù 98% map không có người chơi. |
| **5. Eager NPC Spawning (74.290 NPC)** | `KNpcSet::AddMapNpc` nạp toàn bộ NPC | **~1.85 GB** | 17.0% | **TRỌNG TÂM PR1 & PR3:** 74.290 quái vật được nạp sẵn vào `KNpc` runtime pool, tạo AI state và timers. |
| **6. Lua Bytecode & Script VM** | Nạp ~3.500 file script Lua toàn server | **~1.15 GB** | 10.5% | Toàn bộ script nhiệm vụ, kỹ năng, sự kiện được biên dịch vào VM lúc khởi động. |
| **TỔNG CỘNG BAN ĐẦU** | | **~10.91 GB** | **100%** | |
| **HIỆN TẠI (Sau PR0 Fix 1 & 2)** | | **9.06 GB** | **83.0%** | **Đã giảm ~1.85 GB RAM**, ổn định, 0 lỗi. |

---

## 2. Chi tiết các Hotspots và Cơ chế

### Hotspot 1: Eager Map Loading trong `KSubWorld::LoadMapS`
- **Vị trí:** `KSubWorld.cpp:997` và `KSubWorldSet.cpp:464`.
- **Cơ chế:** Vòng lặp `for (int i = 0; i < nWorldCount; ++i)` thực thi đồng bộ nạp cả 770 bản đồ. Mỗi bản đồ cấp phát một mảng phẳng `m_Region = new KRegion[m_nTotalRegion]`.
- **Hậu quả:** 700.000 đối tượng `KRegion` (mỗi struct ~4.456 bytes) lập tức chiếm gần 3.8 GB bộ nhớ, cộng thêm dữ liệu chướng ngại vật `m_Obstacle` (2.048 bytes x 700.000 = ~1.43 GB).

### Hotspot 2: Eager NPC Spawning trong `KRegion::LoadServerNpc`
- **Vị trí:** `KRegion.cpp:802` gọi `NpcSet.AddMapNpc(nSubWorld, &sNpcCell, pMapListInfo)`.
- **Cơ chế:** Đọc tuần tự tất cả file NPC trong mọi region và gọi `AddServerNpcA()`, khởi tạo toàn bộ 74.290 thực thể quái vật vào mảng `KNpc Npc[MAX_NPC]`.
- **Hậu quả:** Toàn bộ slot NPC bị chiếm dụng, sinh ra hàng triệu liên kết danh sách và dữ liệu AI chạy ngầm.

### Hotspot 3: Preallocated Global Pools
- **Vị trí:** `KCore.cpp:395-440`.
- **Cơ chế:** Cấp phát một lần khi khởi động: `SubWorld[1000]`, `Npc[150000]`, `Item[200000]`.
- **Chi phí cố định:** 2.21 GB RAM ngay cả khi chưa nạp bất kỳ bản đồ nào.
