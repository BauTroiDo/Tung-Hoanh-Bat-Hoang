# Operations & Rollback Runbook

## 1. Feature Flag Master Switches

All streaming systems are strictly gated behind configuration flags in `settings\gamesetting_gs001.ini`. Any component can be disabled without requiring code changes or server recompilation.

```ini
[ServerConfig]
; PR2: Two-Tier Lazy Map Loading (0 = Eager load all 770 maps, 1 = Lazy on-demand)
LazyMapLoading=1

; PR3: NPC Region Streaming (0 = Spawn all NPCs eagerly, 1 = 1-ring dynamic activation)
NpcActivation=1

; PR4: Safe Map Unload (0 = Keep loaded maps in memory forever, 1 = Safe unload after TTL)
SafeUnload=1

; PR4: Idle TTL duration in seconds before safe unload (default: 300)
MapIdleSeconds=300

; PR5: Max concurrent map loads per world tick (default: 2)
MaxConcurrentLoads=2

; PR5: Max active non-pinned maps before cache pressure eviction (default: 64)
MaxLoadedMaps=64

; PR5: Right-sized static pools
MaxNpcCount=90000
MaxItemCount=100000
```

---

## 2. Granular Rollback Procedures

### Scenario A: Revert Map Unloading Only (Keep Lazy Loading)
- Change `SafeUnload=0` in `gamesetting_gs001.ini`.
- Restart `gs001.exe`.
- Result: Maps load dynamically when entered, but never unload.

### Scenario B: Revert NPC Streaming Only
- Change `NpcActivation=0` in `gamesetting_gs001.ini`.
- Restart `gs001.exe`.
- Result: When a map loads, all its monsters spawn immediately across all regions.

### Scenario C: Complete System Reversion to Baseline
- Set:
  ```ini
  LazyMapLoading=0
  NpcActivation=0
  SafeUnload=0
  MaxNpcCount=150000
  MaxItemCount=200000
  ```
- Restart `gs001.exe`.
- Result: Restores 100% original monolithic startup behavior (~11.82 GB baseline).

---

## 3. Binary Backup Locations
- Binary Release: `c:\Server-Game\SwordOnline\ServerNew\_bin_v2_\gs\CoreServer.dll`
- Source Tree 1: `c:\Server-Game\SwordOnline\jxOnline-r78\`
- Source Tree 2: `c:\Server-Game\SwordOnline\jxOnline\`
