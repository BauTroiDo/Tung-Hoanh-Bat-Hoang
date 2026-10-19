# PR3 Design: NPC Dynamic Activation & Region Streaming

## 1. Overview & Objectives
- **Target:** Transition monster allocation from eager full-map instantiation to on-demand region streaming.
- **Baseline Behavior:** When a map is loaded, every single NPC (up to 1,000+ per map) is allocated in the global `KNpc` pool (8,536 bytes each) and ticked every frame in `KSubWorld::Activate()`.
- **PR3 Optimized Behavior:**
  - **Essential NPCs Pinned:** Shopkeepers, quest givers, town NPCs (`shKind != 3`), bosses/world bosses (`bSpecialNpc == true`), and Lua dynamically spawned NPCs (`m_bDynamicAdd == true`) are **always active** upon map load.
  - **Regular Monsters Streamed:** Non-essential monsters are stored as lightweight `KSpawnDefinition` records (~160 bytes vs 8,536 bytes) in `KRegion::m_vSpawnDefs`.
  - **Activation on Proximity:** When a player approaches a region (`m_nNearbyPlayerCount > 0`), the region activates and instantiates its defined monsters into runtime `KNpc`.
  - **Dormant Persistence:** When players leave the region and its 1-ring neighbor boundary, monster states (HP, death status, respawn tick) are preserved in `KSpawnDefinition` and the runtime slots can be recycled safely.
- **Feature Flag Gated:** Controlled via `NpcActivation` (0=Off baseline eager spawn, 1=On dynamic streaming). Default: 0.

---

## 2. State Machine & Exception Policy

### 2.1 NPC Streaming State Machine
```
[KSpawnDefinition] (Lightweight definition, 160 bytes)
       |
       | Player enters Region (UpdateActiveType: NearbyPlayerCount > 0)
       v
[SPAWNING] --> Verify respawn cooldown (dwNextRespawnTime <= dwCurrentTime)
       |
       v
[ACTIVE] (Instantiated KNpc in Npc[], 8,536 bytes, ticking AI & Combat)
       |
       | All players leave Region & 1-ring buffer (NearbyPlayerCount == 0)
       v
[DORMANT] --> Persist current HP & respawn state into KSpawnDefinition
       |
       v
[DESPAWNED] (KNpc slot recycled back to KNpcSet pool)
```

### 2.2 Exception Classification Matrix
| NPC Category | Identifier | Policy | Rationale |
| :--- | :--- | :--- | :--- |
| **City / Town NPCs** | MapType = City/Country/Capital | **Prewarm / Pinned** | Instant player interaction in all hubs |
| **Shopkeepers & Quest Givers** | `shKind != 3` or has `szScript` | **Always Pinned** | Dialogues, trade, quest turn-ins must never fail |
| **Bosses & Golden Mobs** | `bSpecialNpc == true` | **Always Pinned** | Boss timers, world events, special drop schedules |
| **Lua Dynamic NPCs** | `m_bDynamicAdd == true` | **Always Pinned** | Scripted mission spawns must maintain pointer/index integrity |
| **Outdoor Monsters / Mobs** | `shKind == 3 && !bSpecialNpc` | **Region Streamed** | Activated only when players are in AOI + 1-ring buffer |

---

## 3. Concurrency & Lua Safety Invariants
1. **Single-Thread Execution:** Region streaming and NPC activation execute strictly on the main GameServer world thread. Zero multi-threading races on `Npc[]` or `SubWorld[]`.
2. **Stable 64-bit Spawn ID:** Every `KNpc` spawned from `KSpawnDefinition` retains its immutable `uSpawnId` (`(mapId << 32) | (regionIdx << 16) | localIdx`).
3. **Double-Spawn Prevention:** A spawn definition with `nRuntimeNpcIndex > 0` is never re-spawned.
4. **Respawn Clock Consistency:** If a monster dies while active, `dwNextRespawnTime` is persisted. When the region re-activates, the monster will not spawn prematurely until its respawn duration expires.
