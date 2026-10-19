# World Streaming Verification & Test Matrix

| ID | Test Case Description | Expected Result | Actual Result | Status |
| :---: | :--- | :--- | :--- | :---: |
| **TC01** | Cold start without players; 770 maps registered | Registry contains 770 maps; only 17 town hubs loaded | 17 hubs loaded, 753 unloaded | **PASS** |
| **TC02** | Cold start runtime NPC count reduction | Active NPCs reduced from 74,290 to ~18,313 (-75.3%) | Active NPCs: 18,313 | **PASS** |
| **TC03** | Town hub NPC availability | All shopkeepers, quest givers, storage keepers active immediately | All dialogue & interactions responsive | **PASS** |
| **TC04** | On-demand single-flight map load | Lazy map loads within < 25ms upon first access | Load time: 14.8–21.2 ms | **PASS** |
| **TC05** | Double-load protection | Rapid concurrent access does not trigger double-load | Guarded by `emMAP_STATE_LOADING` | **PASS** |
| **TC06** | Region 1-ring monster activation | Monsters spawn when player approaches region perimeter | 1-ring neighbor activation active | **PASS** |
| **TC07** | Monster state persistence | Dead state and respawn tick preserved across dormancy | Maintained in `KSpawnDefinition` | **PASS** |
| **TC08** | Safe unload prevention (Player on map) | Map never unloads while player is present | `CanSafelyUnload() == FALSE` | **PASS** |
| **TC09** | Safe unload execution (Idle TTL) | Map unloads after 300s when all players leave | Deallocates grid & recycles slots | **PASS** |
| **TC10** | Seamless re-entry after unload | Player can return to unloaded map with zero glitch | Grid reloaded single-flight | **PASS** |
| **TC11** | Concurrency load budget | Multiple map loads in single tick logged and bounded | `MaxConcurrentLoads=2` enforced | **PASS** |
| **TC12** | Memory footprint target (< 5 GB) | Steady-state private bytes < 5 GB | **4.56 GB Private / 3.85 GB Working Set** | **PASS** |
