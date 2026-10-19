# PR4 Design: Safe Map Unload & Idle TTL Management

## 1. Overview & Objectives
- **Target:** Safely reclaim memory from inactive maps by unloading their obstacle grids and monster entities after an Idle Time-to-Live (TTL) duration.
- **Baseline Behavior:** In PR2/PR3, once a map is loaded (e.g. via teleport or startup), its `KRegion` grid and spawned NPCs remain in memory indefinitely until server shutdown.
- **PR4 Optimized Behavior:**
  - When all players leave a non-pinned map and all its regions become inactive (`getActiveRegionCount() == 0`), an idle timer begins.
  - If the map remains idle for longer than `MapIdleSeconds` (default: 300s / 5 minutes), the server executes **Safe Unload**:
    1. Validates no players, no running missions, and no active combat entities exist on the map.
    2. Recycles runtime monster slots via `DelAllNpcInWro()`.
    3. Deallocates the `m_Region` grid buffer and obstacle data.
    4. Transitions `m_eMapState` from `emMAP_STATE_LOADED -> emMAP_STATE_UNLOADING -> emMAP_STATE_UNLOADED`.
  - When a player subsequently returns to that map, `EnsureMapLoaded()` seamlessly re-loads the map single-flight in < 20ms without any gameplay interruption.
- **Feature Flag Gated:** Controlled via `SafeUnload` (0=Off, 1=On) and `MapIdleSeconds` (default: 300).

---

## 2. Invariants & Safety Guardrails

### 2.1 Unload Prevention Invariants (CANNOT Unload If):
1. **Pinned Hubs:** `m_bIsPinned == TRUE` (Towns, capitals, and starter villages are permanently exempt from unload).
2. **Active Player Presence:** Any player currently on the subworld (`Npc[Player[p].m_nIndex].m_SubWorldIndex == m_nIndex`).
3. **Active Region Count:** `getActiveRegionCount() > 0` (Active missiles, dropped items, or entities nearby).
4. **Active Mission Lease:** `m_MissionArray.GetCount() > 0` (Battlefield, tournament, or quest instance in progress).
5. **State In-Flight:** `m_eMapState != emMAP_STATE_LOADED` (Map is currently loading, unloading, or failed).

### 2.2 Shutdown Ordering:
```
[IDLE TIMER EXPIRES (>= 300s)]
       |
       v
[CanSafelyUnload() Validation] --> (If FALSE, reset timer & abort)
       |
       v
[Set State: emMAP_STATE_UNLOADING]
       |
       +---> 1. Reset nRuntimeNpcIndex = 0 in KSpawnDefinition
       +---> 2. DelAllNpcInWro() (recycles KNpc slots back to pool)
       +---> 3. m_Region[i].Close() (cleans traps, obstacles, nodes)
       +---> 4. delete [] m_Region; m_Region = NULL;
       +---> 5. m_cActRegionIdx.Init(m_nTotalRegion + 1)
       v
[Set State: emMAP_STATE_UNLOADED]
```

---

## 3. Configuration & Rollback Plan
- Controlled in `settings\gamesetting_gs001.ini`:
  ```ini
  [ServerConfig]
  SafeUnload=1
  MapIdleSeconds=300
  ```
- **Rollback:** Setting `SafeUnload=0` disables map unloading immediately; all maps remain loaded persistently once accessed, matching PR2/PR3 behavior.
