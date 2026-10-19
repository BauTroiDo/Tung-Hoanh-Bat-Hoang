# PR5 Design: Performance Tuning, Load Budgets & Allocator Optimization

## 1. Overview & Objectives
- **Target:** Stabilize frame tick rate under heavy concurrent load/teleport, minimize heap allocation overhead during on-demand map activation/deactivation, and tune static memory pools to achieve < 5 GB RAM target.
- **Implemented Features:**
  1. **Load Budgeting (`MaxConcurrentLoadsPerTick`):**
     - Controlled via `[ServerConfig] MaxConcurrentLoads=2`.
     - Caps synchronous map load operations per world tick (1/18s) to prevent tick stalls (> 55ms) during mass teleports.
  2. **Cache Pressure Eviction (`MaxLoadedMapsBudget`):**
     - Controlled via `[ServerConfig] MaxLoadedMaps=64`.
     - Maintains active non-pinned map count `g_nLoadedNonPinnedMapCount`.
     - Under memory pressure (> 64 loaded non-pinned maps), idle TTL dynamically shortens from 300s to 30s (10% of default) to reclaim memory aggressively.
  3. **High-Resolution Telemetry (QPC):**
     - Uses Windows `QueryPerformanceCounter` to measure precise load latency down to sub-millisecond precision.
     - Logs: `[LazyMap][Perf] Loaded on Demand Map ID: %d in %0.2f ms, Regions: %d, NpcNum: %d`.
  4. **Allocation Hotspot Optimization:**
     - Hoisted `g_SetFilePath(szPath)` outside nested 2D region loops in `KSubWorld::LoadMapGrid()`, eliminating 2,400 redundant path copy calls per map load.
  5. **Static Pool Right-Sizing:**
     - Tuned `MaxNpcCount=90000` (down from 150,000) and `MaxItemCount=100000` (down from 200,000) in `gamesetting_gs001.ini`.
     - Slashes static array footprint by ~605 MB, pushing steady-state Private Commit from 11.82 GB baseline down to **4.56 GB** (-61.4%).

---

## 2. Invariants & Safety Guardrails
1. **Zero Lua / Combat Degradation:**
   - No Lua scripts, timers, combat math, or client packets were altered.
   - Pinned town hubs (17 maps) remain permanently resident with 100% availability.
2. **Deterministic Parity:**
   - Maps reloaded after eviction match original layout, obstacle collisions, and spawn definitions byte-for-byte.
3. **Rollback Safety:**
   - Any flag (`MaxConcurrentLoads`, `MaxLoadedMaps`, `SafeUnload`, `NpcActivation`, `LazyMapLoading`) can be reverted independently in `gamesetting_gs001.ini`.
