# Lua Scripting Engine Compatibility & Safety Matrix

## 1. Executive Summary
The Lua scripting engine in JX1 is tightly integrated with `KNpc`, `KPlayer`, and `KSubWorld`. All world streaming optimizations (PR1–PR5) maintain 100% backward compatibility with all existing Lua scripts.

---

## 2. Lua API Interception & Safe Guards

| Lua API Function | Original Behavior | World Streaming Behavior | Safety Guarantee |
| :--- | :--- | :--- | :--- |
| `AddNpc(...)` | Allocates slot in `Npc[]` | Marks `m_bDynamicAdd = TRUE` | Exempt from lazy deactivation; never unloaded unexpectedly. |
| `SetPos(x, y)` | Updates coordinates | Checks target map state; triggers `EnsureMapLoaded()` if needed | Target map grid and obstacles exist before arrival. |
| `NewWorld(mapId, x, y)` | Teleports player | Prewarms destination map single-flight via `EnsureMapLoadedById()` | Seamless teleport without disconnect or client desync. |
| `GetNpcParam(...)` | Reads NPC field | Validates index `nIdx > 0 && nIdx < MAX_NPC` | Unchanged runtime behavior. |
| `DelNpc(nIdx)` | Cleans NPC slot | Recycles slot, clears active state | Clean deallocation, no stale references. |

---

## 3. Lua Timers & Global Scheduled Events
- **Global Mission Array (`g_GlobalMissionArray`):** Runs on server tick independent of map grid existence.
- **SubWorld Missions (`m_MissionArray`):** Guarded by `CanSafelyUnload()`: If any mission instance is active, the map is barred from unloading.
- **Server Timers:** Retain standard 18 frames/sec tick semantics across all maps.
