# JX1 Server Architecture Audit & Startup Call Graph

## 1. Tổng quan môi trường & Nền tảng
- **Hệ điều hành:** Windows Server / Windows 10/11 x64.
- **Compiler/Toolchain:** Microsoft Visual Studio 2022 BuildTools (v143, x64).
- **Target Configuration:** `Server Release|x64`, DLL output: `CoreServer.dll` (12.15 MB), EXE: `gs001.exe`.
- **GameServer process model:** Đơn tiến trình (`gs001.exe`) nạp toàn bộ thế giới trò chơi (770 map).
- **Cơ chế nạp gốc:** Monolithic eager loading (nạp toàn bộ 770 map, obstacle, trap, và 74.290 NPC ngay khi boot).

---

## 2. Chi tiết Startup Call Graph (Đường dẫn file:dòng chính xác)

```mermaid
flowchart TD
    A["GameServer.cpp:141<br/>main(argc, argv)"] --> B["GameServer.cpp:189<br/>g_pSOServer.InitServer(moduleFileName)"]
    B --> C["KSOServer.cpp:704<br/>KSwordOnLineSever::InitServer(parmName)"]
    C --> D["KSOServer.cpp:717<br/>Load config: gamesetting_gs001.ini"]
    C --> E["KSOServer.cpp:830<br/>Initialize IOCP Server Network"]
    C --> F["KSOServer.cpp:965<br/>CoreGetServerShell(parmName)"]
    F --> G["CoreServerShell.cpp:227<br/>CoreGetServerShell(gamePass)"]
    G --> H["KCore.cpp:254<br/>g_InitCore(nParmName)"]
    H --> I["KCore.cpp:395<br/>Static Arrays Allocation:<br/>SubWorld[1000] (634 MB)<br/>KNpc[150000] (1221 MB)<br/>KItem[200000] (234 MB)<br/>KPlayer[500] (76 MB)<br/>KObj[20000] (8 MB)<br/>KMissle[100000] (41 MB)"]
    H --> J["KCore.cpp:670<br/>g_SubWorldSet.LoadSerMaps(nTempMapSetPath)"]
    J --> K["KSubWorldSet.cpp:441<br/>KSubWorldSet::LoadSerMaps(szFileName)"]
    K --> L["Loop 0..nWorldCount (770 maps)<br/>KSubWorldSet.cpp:464: SubWorld[i].LoadMapS(nWorldID)"]
    L --> M["KSubWorld.cpp:828<br/>KSubWorld::LoadMapS(nId)"]
    M --> N["KSubWorld.cpp:942<br/>Load map header (.wor): rect, m_nTotalRegion"]
    M --> O["KSubWorld.cpp:997<br/>m_Region = new KRegion[m_nTotalRegion]"]
    M --> P["Loop Region Y/X:<br/>KSubWorld.cpp:1031: m_Region[nIdx].Load()"]
    M --> Q["Loop Region Y/X:<br/>KSubWorld.cpp:1070: m_Region[nIdx].LoadObjectS()"]
    Q --> R["KRegion.cpp:184<br/>LoadServerObstacle()"]
    Q --> S["KRegion.cpp:188<br/>LoadServerTrap()"]
    Q --> T["KRegion.cpp:192<br/>LoadServerNpc()"]
    T --> U["KNpcSet.cpp:527<br/>KNpcSet::AddMapNpc()<br/>-> AddServerNpcA() -> Npc[i].Init()"]
    Q --> V["KRegion.cpp:196<br/>LoadServerObj()"]
    K --> W["KCore.cpp:685<br/>InitTaskSetting() -> Lua Scripts Init"]
    W --> X["KSOServer.cpp:1441<br/>MainLoop(): Server start succeeded, Game Started!"]
```

---

## 3. Phân biệt Dữ liệu Tĩnh (Cold Definitions) và Dữ liệu Động (Hot Runtime)

### 3.1 Dữ liệu Bản đồ
* **Bản đồ tĩnh (Cold Definitions):**
  - Tên map, ID, kích thước rect (BeginX, BeginY, EndX, EndY), số region (`m_nTotalRegion`), tỷ lệ rơi đồ, thiết lập PK punish state.
  - Obstacle map data: 2.048 bytes per region (`m_Obstacle[16][32]`).
  - Trap points: ID bẫy và vị trí cell chuyển cảnh.
* **Bản đồ runtime (Hot Runtime):**
  - `m_cActRegionIdx`: Danh sách các region đang active có người chơi.
  - Vòng lặp `KSubWorld::Activate()`: Xử lý quái, va chạm, missile theo từng frame.

### 3.2 Dữ liệu NPC
* **NPC tĩnh (Spawn Definitions):**
  - Template ID, Level, vị trí tọa độ gốc $(X, Y)$, Tên hiển thị, Ngũ hành, Loại quái (`shKind`), Đường dẫn script Lua gắn liền (`szScript`), Tỷ lệ rơi đồ (`nNDropFile`, `nGDropFile`).
* **NPC runtime (Hot Instance State):**
  - `KNpc[i]` instance: Máu hiện tại (`m_CurrentLife`), Nội lực, Mana, Trạng thái chiến đấu, Trạng thái đóng băng/trúng độc, Vị trí di chuyển hiện tại, Điểm đích (`m_DesX, m_DesY`), AI state machine, Danh sách hận thù (Aggro table).

---

## 4. Các yếu tố phụ thuộc kiến trúc 64-bit (x64)

1. **Kích thước con trỏ (Pointer Size):**
   - Trên x86 (32-bit Linux V9): `sizeof(void*) = 4` bytes.
   - Trên x64 (Windows CoreServer): `sizeof(void*) = 8` bytes.
   - Hệ quả: Mọi vtable, con trỏ mảng, linked list node (`KList`, `KIndexNode`, `KNode`), `KRegion* m_pConRegion[8]` đều tăng gấp đôi kích thước.
2. **Căn lề bộ nhớ (Struct Alignment & Padding):**
   - MSVC x64 mặc định align các thành viên 8-byte theo biên 8-byte.
   - Các struct chứa hỗn hợp `int` (4 bytes) và con trỏ (8 bytes) bị chèn padding 4 bytes vào giữa.
3. **Mảng toàn cục tĩnh (Static Preallocated Pools):**
   - `KNpc[150000]`: `8536 bytes * 150000 = 1221 MB` (trên x86 chỉ ~600 MB).
   - `SubWorld[1000]`: `sizeof(KSubWorld) ~ 634 KB * 1000 = 634 MB` (trên x86 chỉ ~280 MB).
   - `KItem[200000]`: `1232 bytes * 200000 = 234 MB` (trên x86 chỉ ~120 MB).
   - `KPlayer[500]`: `160824 bytes * 500 = 76 MB`.
   - `KMissle[100000]`: `432 bytes * 100000 = 41 MB`.
   - `KObj[20000]`: `420 bytes * 20000 = 8 MB`.
   - **Tổng mảng tĩnh toàn cục: ~2.214 MB (2.21 GB).**
