# Source & Asset Inventory: JX1 64-Bit Server & Data Formats

## 1. Codebase Architecture & Build Toolchain
- **OS Platform:** Windows Server / Windows 10/11 x64.
- **Compiler/Toolchain:** Microsoft Visual Studio 2022 BuildTools (v143, x64).
- **Target Platform Configuration:** `Server Release|x64`.
- **Primary Binary Artifacts:**
  - `CoreServer.dll` (~12.15 MB): Core game engine, world simulation, combat, physics, entities, Lua VM.
  - `gs001.exe` (~840 KB): GameServer host process, IOCP network loop, console dispatch.
  - `Sword3PaySys.dll` (~2.1 MB): Account authentication, billing, role tokens.

---

## 2. Source Tree Modules (`gameserver\Sources\`)
| Module Name | Path | Description & Responsibilities |
| :--- | :--- | :--- |
| **Core** | `Core\Src\` | `KCore.cpp`, `KSubWorld.cpp`, `KRegion.cpp`, `KNpc.cpp`, `KPlayer.cpp`, `KItem.cpp`, `KMissle.cpp`, `KObj.cpp`, `ScriptFuns.cpp`. Implements world grid, entities, combat mechanics, and script binding. |
| **Engine** | `Engine\Src\` | Memory allocators, tab file parser (`KTabFile`), INI parser (`KIniFile`), logging, timer routines. |
| **MultiServer** | `MultiServer\` | Database broker (`Rainbow`), Inter-server communication (`Bishop`), Relay, Gateway. |
| **RoleServer** | `RoleServer\` | Database persistence for character records, equipment, inventory, skills. |
| **Lua Core** | `Core\Src\Lua\` | Custom embedded Lua VM (Lua 5.0/5.1 hybrid) with C API bindings. |

---

## 3. Server Data Files (`settings\`, `maps\`, `script\`)
| Data Directory | File Types | Count / Scale | Purpose |
| :--- | :--- | :---: | :--- |
| `maps\` | `.wor`, `.obs`, `.trv`, `.txt` | **770 maps** | World dimensions, region layout, terrain obstacle collision bitmaps, trap triggers, monster spawn coordinate tables. |
| `settings\` | `.ini`, `.txt` | **~1,200 files** | Skill tables (`skill.txt`), NPC definitions (`npc.txt`), item attributes (`magicattrib.txt`), quest definitions. |
| `script\` | `.lua` | **~3,500 files** | Quests, NPC dialogues, map entry/exit events, tournament logic, GM tools. |

---

## 4. Client Legacy Asset Formats (2D -> 3D Transition Input)
| Format | Extension | Structure & Purpose | 3D Canonical Transformation Pipeline |
| :--- | :---: | :--- | :--- |
| **World Header** | `.wor` | Binary map header: `m_nRegionBeginX`, `m_nRegionBeginY`, `m_nRegionWidth`, `m_nRegionHeight`. | Maps canonical bounding box $(X, Y)$ and origin $T_{map}$. |
| **Obstacle Data** | `.obs` | 2D bitmap per region ($16 \times 32$ cells, 2 bytes/cell). 4 collision flags: walkable, obstacle, water, transparent. | Extruded into static collision proxy and NavMesh input geometry. |
| **Trap Triggers** | `.trv` | Cell triggers for portal transitions, map teleports, and Lua trap callbacks. | Converted to 3D volume trigger anchors and navigation exit links. |
| **Sprite Package** | `.spr` | Palette-based 2D directional sprites (8/16 angles) with frame offsets. | Replaced in 3D by skeletal meshes, rigs, and blend-tree animations. |
