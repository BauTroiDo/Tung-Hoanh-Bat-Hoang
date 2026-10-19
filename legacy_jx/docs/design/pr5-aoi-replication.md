# PR5 Design & Verification: Area of Interest (AOI) & Object Replication

## 1. Overview & Objectives
- **Target:** Formalize and verify the spatial Area of Interest (AOI) interest sets, visibility boundaries, and entity replication lifecycle (Create, Update, Remove, Resync) between server and client.
- **Two-Tier Interest Model:**
  1. **Simulation Interest:** Handled server-side by `KSubWorld` and `KRegion` active flags. Simulates combat, AI, timers, and missile movement across active regions and pinned hubs.
  2. **Replication Interest:** Handled by `KRegion::BroadCast` and the 9-region spatial cluster (`CURREGION` + 8 `CONREGION` neighbors). Determines which observers receive network packets.
- **Client Protocol Parity:** Preserves 100% legacy 2D client packet formats while providing the foundation for the 3D isometric client adapter.

---

## 2. Spatial Index & 9-Region AOI Cluster

### 2.1 Spatial Topology:
```
+---------------+---------------+---------------+
| CONREGION(3)  | CONREGION(4)  | CONREGION(5)  |
| (-1, -1)      | ( 0, -1)      | ( 1, -1)      |
+---------------+---------------+---------------+
| CONREGION(2)  | CURREGION     | CONREGION(6)  |
| (-1,  0)      | ( 0,  0)      | ( 1,  0)      |
+---------------+---------------+---------------+
| CONREGION(1)  | CONREGION(0)  | CONREGION(7)  |
| (-1,  1)      | ( 0,  1)      | ( 1,  1)      |
+---------------+---------------+---------------+
```
- Each Region covers $16 \times 32$ logic cells ($512 \times 1024$ logic pixels).
- The 9-region AOI cluster covers $1536 \times 3072$ logic pixels, matching the field of view of both the legacy 2D client resolution and the target 3D isometric camera ($45^\circ$ oblique overhead view).

---

## 3. Entity Replication Lifecycle

| Phase | Server Trigger | Network Packet (Protocol) | Client Action |
| :--- | :--- | :--- | :--- |
| **CREATE** | Entity enters observer's 9-region cluster | `s2c_syncrole`, `ROLE_LIST_SYNC` | Instantiates entity model/sprite, sets initial position, level, weapon, horse. |
| **UPDATE** | Entity moves, changes state, casts skill | `s2c_npcwalk`, `s2c_npcrun`, `s2c_playerskillcast` | Interpolates movement destination, plays animation/VFX, syncs current HP/Mana. |
| **REMOVE** | Entity exits 9-region cluster or despawns | `s2c_delrole`, `s2c_npcdie` | Removes entity model from viewport, recycles client GameObject pool. |
| **RESYNC** | Map teleport (`NewWorld`) or reconnect | `s2c_gamestatistic`, full role list | Flushes previous map entity list, resets epoch, requests full destination snapshot. |

---

## 4. Test Verification Matrix (Acceptance Criteria)

| Test ID | Test Scenario | Expected Result | Status |
| :---: | :--- | :--- | :---: |
| **TC-01** | Cross-region movement broadcast | Movement packets broadcast to current + 8 neighbor regions | **PASS** |
| **TC-02** | Cross-map instance isolation | Entity in Map A never broadcasts packets to Map B | **PASS** |
| **TC-03** | Boundary crossing hysteresis | Moving back and forth across boundary does not flicker entity | **PASS** |
| **TC-04** | Teleport snapshot resync | Destination map entities snapshot sent cleanly upon arrival | **PASS** |
| **TC-05** | Backward compatibility | Legacy 2D client receives identical byte-level packet stream | **PASS** |
