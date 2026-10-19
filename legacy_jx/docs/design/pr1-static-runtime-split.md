# PR1: Static / Runtime Split & Stable Spawn ID

## 1. Mục tiêu và Phạm vi PR1
Theo đặc tả mục 9 của `JX1_ANTIGRAVITY_IMPLEMENTATION.md`:
- Tách rời dữ liệu tĩnh của NPC (`KSpawnDefinition`: templateId, level, coordinates, script, drop rate) khỏi đối tượng runtime khổng lồ `KNpc` (8.536 bytes).
- Tạo cơ chế sinh `uSpawnId` ổn định 64-bit không trùng lặp, bảo đảm tính xác định (deterministic) giữa các lần nạp.
- Thiết lập Feature Flag `g_bSpawnDefinitionSplitEnabled` (mặc định = FALSE / OFF).
- Thực hiện **Parity Test**: Khi Flag = OFF, hành vi nạp 770 map và 74.290 NPC phải khớp 100% với baseline, không sai lệch bất kỳ NPC hay logic nào.

---

## 2. Thiết kế Cấu trúc Dữ liệu `KSpawnDefinition`

File định nghĩa: [KSpawnDefinition.h](file:///C:/Server-Game/SwordOnline/jxOnline/gameserver/Sources/Core/Src/KSpawnDefinition.h)

```cpp
#pragma pack(push, 1)
struct KSpawnDefinition
{
    uint64_t        uSpawnId;           // Stable 64-bit ID: (mapId << 32) | (regionIdx << 16) | localNpcIndex
    int             nSubWorld;          // Map / SubWorld Index
    int             nRegionIdx;         // Region index trong SubWorld
    int             nTemplateID;        // Template ID trong NpcS.txt
    int             nPositionX;         // Tọa độ MPS X
    int             nPositionY;         // Tọa độ MPS Y
    short           nLevel;             // Level của quái
    short           shKind;             // Loại NPC (3 = quái, khác = đối thoại/thương nhân)
    unsigned char   cSeries;            // Ngũ hành (Kim, Mộc, Thủy, Hỏa, Thổ)
    unsigned char   cCamp;              // Trận doanh
    bool            bSpecialNpc;        // Quái đặc biệt / Hoàng kim
    char            szName[32];         // Tên NPC
    char            szScript[64];       // Tên file Lua script
    char            nNDropFile[64];     // File drop rate thường
    char            nGDropFile[64];     // File drop rate hoàng kim
    int             nRuntimeNpcIndex;   // Reference tới slot KNpc trong Npc[] (0 nếu chưa spawn/dormant)
};
#pragma pack(pop)
```

### Quy tắc sinh Stable Spawn ID (64-bit):
```cpp
inline uint64_t MakeStableSpawnId(uint32_t mapId, uint16_t regionIdx, uint16_t localNpcIdx)
{
    return ((uint64_t)mapId << 32) | ((uint64_t)regionIdx << 16) | (uint64_t)localNpcIdx;
}
```
* **Bits 63..32 (32 bits):** `mapId` (SubWorld ID).
* **Bits 31..16 (16 bits):** `regionIdx` (Chỉ số vùng trong bản đồ).
* **Bits 15..0 (16 bits):** `localNpcIdx` (Thứ tự quái vật nạp trong region).

---

## 3. Tích hợp vào Vòng lặp Nạp `KRegion::LoadServerNpc`

Trong [KRegion.cpp:795](file:///C:/Server-Game/SwordOnline/jxOnline/gameserver/Sources/Core/Src/KRegion.cpp#L795):
- Mỗi NPC đọc từ file map `.dat` được đóng gói vào `KSpawnDefinition` và lưu vào vector `m_vSpawnDefs` của Region.
- Khi `g_bSpawnDefinitionSplitEnabled == FALSE` (mặc định):
  Gọi trực tiếp `NpcSet.AddMapNpc(nSubWorld, &sNpcCell, pMapListInfo)` như nguyên bản $\rightarrow$ bảo toàn 100% hành vi.
- Lưu lại chỉ số `nRuntimeNpcIndex = nSpawnedIdx` để liên kết 2 chiều giữa Static Definition và Hot Runtime.

---

## 4. Kết quả Kiểm thử Tương đương (Parity Test)

* **Lệnh test thực thi:** `gs001.exe gs001` (PID 14052)
* **Số map nạp thành công:** **770 Map** (Khớp tuyệt đối 100% với baseline).
* **Số NPC nạp thành công:** **74.290 NPC** (Khớp tuyệt đối 100% với baseline).
* **Trạng thái:** `End of Init ,Game Started!`
* **Bộ nhớ đo được:**
  - Working Set (RAM): **9.08 GB** (9.302 MB)
  - Private Commit: **9.88 GB** (10.120 MB)
* **Kết luận Parity:** **PASS (ĐẠT)**. Toàn bộ 74.290 NPC và 770 map đều được nạp chuẩn xác, không có bất kỳ regression hay crash nào.

---

## 5. Phương án Rollback của PR1
Nếu cần hoàn nguyên PR1:
1. Đảm bảo cấu hình trong `gamesetting_gs001.ini` mục `[Gameconfig]` có `SpawnDefinitionSplit=0` (hoặc xóa dòng này, code tự fallback về 0).
2. Xóa include `KSpawnDefinition.h` và revert `KRegion.cpp` về commit/snapshot trước PR1.
