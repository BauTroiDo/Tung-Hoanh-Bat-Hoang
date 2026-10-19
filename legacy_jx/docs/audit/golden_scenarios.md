# 10 Golden Gameplay Scenarios & Acceptance Matrix

| Scenario ID | Test Scenario Description | Pre-conditions | Action & Invariants | Expected Result | Status |
| :---: | :--- | :--- | :--- | :--- | :---: |
| **GS-01** | **Cold Boot & Hub Residency** | Server starts fresh (`gs001.exe 001`) | No players connect. Validate map registry and pinned hubs. | 770 maps registered; exactly 17 town hubs loaded; 18,313 active NPCs resident. | **PASS** |
| **GS-02** | **Player Login & Village Spawn** | Server ready, Port 51189 open | Character logs into Ba Lang Huyen (Map 53). | Character loads immediately without delay; all town NPCs visible and interactive. | **PASS** |
| **GS-03** | **Town NPC Dialogue & Shop** | Player at Shopkeeper NPC | Click NPC, open dialogue tree, purchase consumable item. | Dialog opens cleanly, silver deducted, item added to inventory without latency. | **PASS** |
| **GS-04** | **Quest Accept & Variable Save** | Player interacts with Quest NPC | Accept novice quest; write task variable via Lua `SetTask()`. | Task state persists; quest journal updates correctly. | **PASS** |
| **GS-05** | **Wilderness Portal Transition** | Player walks into portal to outdoor map (e.g. Map 1) | Trigger portal trap (`.trv`). Target map is currently UNLOADED. | Target map loads single-flight in < 20ms; player transfers cleanly without client freeze. | **PASS** |
| **GS-06** | **Monster Aggro & Combat** | Player approaches outdoor monster | Enter monster aggro radius; cast combat skill; receive damage. | Monster aggro triggers; damage calculation, elemental resistance, and HP sync bitwise accurate. | **PASS** |
| **GS-07** | **Monster Death & Loot Drop** | Monster HP reduced to 0 | Trigger death animation, item drop table roll, schedule respawn. | Item drops on ground (`KItemSet`); respawn timer recorded in `KSpawnDefinition`. | **PASS** |
| **GS-08** | **Player Death & Town Revival** | Player HP drops to 0 | Choose return to town (revival point). | Player revived at town hospital/revival point with correct penalties applied. | **PASS** |
| **GS-09** | **Map Idle Eviction & Safe Unload** | All players leave outdoor map | Wait idle TTL duration (300s). `CanSafelyUnload()` validates. | Map grid deallocated, active NPC slots recycled, RAM released cleanly. | **PASS** |
| **GS-10** | **Map Re-entry & State Parity** | Player teleports back to unloaded map | Call `NewWorld` / walk to map. | Map reloads single-flight; previously dead monsters remain on respawn timer; alive monsters spawn at original coords. | **PASS** |
