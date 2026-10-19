# Lua Engine & Script Dependency Map

## 1. Lua Virtual Machine Architecture
- **Engine Version:** Custom embedded Lua 5.0/5.1 engine with C API bindings.
- **VM Lifetime:** Global shared VM instantiated on GameServer boot via `g_WorldScript.Init()`.
- **C-to-Lua Function Table:** `WorldScriptFuns[]` registered in `ScriptFuns.cpp` (exposing > 450 server functions).
- **Execution Thread Discipline:** Strictly single-threaded execution on the **Main World Thread**. No multithreaded Lua VM access occurs.

---

## 2. Key Lua Script Categories & Bindings
| Category | File Location | Triggers / Callbacks | C++ Interaction |
| :--- | :--- | :--- | :--- |
| **NPC Dialogues** | `script\missions\`, `script\maps\` | `main()`, `Select()`, `OnDeath()` | Accesses NPC index via global context or parameter; calls `Say()`, `Talk()`. |
| **Quest Logic** | `script\task\` | Quest item check, progress flags | Reads/writes player task variables (`SetTask`, `GetTask`). |
| **Skill Scripts** | `script\skill\` | Cast validation, buff applications | Interacts with `KSkill` and `KNpc` attributes. |
| **Map Trap & Portals** | `script\maps\` | `OnEnter()`, `OnLeave()`, trap trigger | Invokes `NewWorld(mapId, x, y)` to initiate teleport transfer. |
| **Server Timers** | `script\global\` | Hourly/daily scheduled events | Global mission tick timers. |

---

## 3. Streaming Safety Guarantees & Lifecycle Compatibility
1. **Raw Pointer Protection:**
   - Lua scripts in JX1 pass integer identifiers (`nNpcIndex`, `nPlayerIndex`, `nSubWorldID`), not raw C++ pointers.
   - All C++ API entry points (`ScriptFuns.cpp`) validate indices against array boundaries (`nNpcIdx > 0 && nNpcIdx < MAX_NPC`).
2. **Dynamic NPC Retention:**
   - NPCs created via `AddNpc()` in Lua are tagged with `m_bDynamicAdd = TRUE` and are permanently exempt from dormant eviction.
3. **Map State Invariants:**
   - Teleport functions (`NewWorld`) call `EnsureMapLoadedById()` ensuring target map grids and obstacles exist before player placement.
