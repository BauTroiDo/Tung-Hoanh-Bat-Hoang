# GameServer Bootstrap Call Graph & Lifecycle Sequence

## 1. Startup Call Graph Sequence

```mermaid
sequenceDiagram
    autonumber
    participant Main as GameServer.cpp (main)
    participant Server as KSOServer (InitServer)
    participant Core as KCore (g_InitCore)
    participant WorldSet as KSubWorldSet (LoadSerMaps)
    participant SubWorld as KSubWorld (LoadMapS)
    participant Region as KRegion (Load/LoadObjectS)
    participant Lua as Lua Engine (InitTaskSetting)
    participant Loop as KSOServer (MainLoop)

    Main->>Server: InitServer(parmName)
    Server->>Server: Load gamesetting_gs001.ini
    Server->>Server: Initialize IOCP Network (Port 51189)
    Server->>Core: CoreGetServerShell() -> g_InitCore()
    Note over Core: Preallocates Static Global Pools<br/>(SubWorld, Npc, Item, Player, Missle)
    Core->>WorldSet: LoadSerMaps("WorldSet_001.ini")
    
    loop For each of 770 Maps
        WorldSet->>SubWorld: LoadMapS(nWorldID)
        SubWorld->>SubWorld: Tier 1: LoadMapMetadata() (.wor header)
        alt Eager (Baseline) vs Lazy (Optimized)
            SubWorld->>SubWorld: Tier 2: LoadMapGrid() (if Pinned Hub)
            SubWorld->>Region: m_Region[i].Load() (Obstacles)
            SubWorld->>Region: m_Region[i].LoadObjectS() (Spawns)
        end
    end

    Core->>Lua: InitTaskSetting() & RegisterFunctions()
    Note over Lua: Compiles ~3,500 scripts into Lua VM
    Server->>Loop: MainLoop() [18 Frames/Second]
    Note over Loop: Server ready and listening!
```

---

## 2. File:Line Implementation References
1. **Host Entrypoint:** `gameserver\Sources\Server\GameServer.cpp:141` (`main`)
2. **Server Shell Initialization:** `gameserver\Sources\Server\KSOServer.cpp:704` (`KSwordOnLineSever::InitServer`)
3. **Core Engine Initialization:** `gameserver\Sources\Core\Src\KCore.cpp:254` (`g_InitCore`)
4. **Static Array Allocation:** `gameserver\Sources\Core\Src\KCore.cpp:395-440`
5. **World Set Map Loading:** `gameserver\Sources\Core\Src\KSubWorldSet.cpp:441` (`KSubWorldSet::LoadSerMaps`)
6. **SubWorld Map Loading:** `gameserver\Sources\Core\Src\KSubWorld.cpp:828` (`KSubWorld::LoadMapS`)
7. **Region Grid Allocation:** `gameserver\Sources\Core\Src\KSubWorld.cpp:1044` (`m_Region = new KRegion[m_nTotalRegion]`)
8. **Obstacle & Spawn Loading:** `gameserver\Sources\Core\Src\KRegion.cpp:184-210` (`LoadServerObstacle`, `LoadServerNpc`)
9. **Lua Functions Registration:** `gameserver\Sources\Core\Src\KCore.cpp:685-715` (`InitTaskSetting`, `g_WorldScript.RegisterFunctions`)
10. **World Simulation Tick:** `gameserver\Sources\Core\Src\KSubWorldSet.cpp:522` (`KSubWorldSet::MainLoop`)
