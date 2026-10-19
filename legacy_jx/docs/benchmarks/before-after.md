# Benchmark & Memory Footprint: Before vs After

## 1. Measurement Methodology
- **Platform:** Windows Server / Windows x64.
- **Process:** `gs001.exe 001` (CoreServer.dll Server Release x64).
- **Instrumentation:** PowerShell `Get-Process` (`WorkingSet64`, `PrivateMemorySize64`), Windows Performance Counters (`QueryPerformanceCounter`), and GameServer internal frame clock (`m_dwCurrentTime` at 18 FPS).
- **Measurement Point:** Steady-state idle immediately following full initialization and IOCP port `51189` listening.

---

## 2. Comparative Benchmark Results

| Metric | Original Monolithic (Baseline) | Post-PR5 (Streaming & Optimized) | Delta | Reduction |
| :--- | :---: | :---: | :---: | :---: |
| **Private Commit Memory** | **11.82 GB** (12,103.6 MB) | **4.56 GB** (4,670.9 MB) | **-7.26 GB** | **-61.4%** |
| **Working Set (Physical RAM)** | **10.91 GB** (11,171.8 MB) | **3.85 GB** (3,943.9 MB) | **-7.06 GB** | **-64.7%** |
| **Startup Loaded Maps** | 770 / 770 (100%) | 17 / 770 (2.2%) | -753 maps | -97.8% |
| **Startup Active NPCs** | 74,290 | 18,313 | -55,977 NPCs | -75.3% |
| **Map Grid Allocation (Per Map)** | Eager at boot | On-Demand (< 20ms) | Dynamic | - |
| **Idle Memory Reclamation** | None (Permanent leak) | Safe Unload after 300s TTL | Reclaims 100% | - |
| **GameServer Tick Rate** | 18 / 18 FPS | 18 / 18 FPS | 0 (Unchanged) | Stable |

---

## 3. Allocation Breakdown of Saved Memory
1. **Elimination of KRegion Heap Churn & Trap Grids:** ~1.96 GB saved.
2. **Two-Tier Map Loading (753 Maps on-demand):** ~3.27 GB saved.
3. **Static Pool Tuning (`MaxNpcCount=90000`, `MaxItemCount=100000`):** ~605 MB saved.
4. **Other Struct Alignment & Hotspot Removals:** ~1.43 GB saved.
5. **Total Memory Reclaimed:** **~7.26 GB**.
