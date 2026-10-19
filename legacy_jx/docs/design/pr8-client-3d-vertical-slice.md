# PR8 Design & Verification: Client 3D Isometric Vertical Slice (Ba Lăng Huyện)

## 1. Overview & Objectives
- **Target:** Establish the complete Client 3D architecture, Engine ADR decision, isometric camera rig, and interactive vertical slice test harness for Map 53 (**Ba Lăng Huyện**).
- **Core Principles:**
  1. **Engine Selection:** Unity 3D with Universal Render Pipeline (URP) per [`docs/adr/ADR-003-client-engine.md`](file:///c:/Server-Game/SwordOnline/jxOnline-r78/docs/adr/ADR-003-client-engine.md).
  2. **Camera Contract:** $45^\circ$ oblique isometric overhead view, field-of-view $35^\circ$, camera distance 25m, keeping classic Võ Lâm Truyền Kỳ visual angle.
  3. **Entity View Binding:** Discrete server `entityId` bound 1-to-1 to 3D `GameObject3D` views with Mecanim animation state machine (`IDLE`, `RUN`, `ATTACK`, `HIT`, `DIE`).
  4. **Server Authority:** Player sends movement destination intent and skill targets; server validates obstacles, computes damage, and replicates authoritative states.

---

## 2. Client 3D Architecture Pipeline

```
[Canonical World Bundle] (world_bundles/maps/53/)
         |
         v
[Client Scene Loader: map.meta.json + spawns.json]
         |
         +---> Instantiates 3D Terrain & Village Prop Proxies
         +---> Binds 3D NPC Entities: Chưởng Điếm (101), Tiệm Rèn (102), Dã Ngoại (205)
         |
[Isometric Camera System] (Pitch: 45°, FOV: 35°, Follow Target: Player)
         |
[Input Dispatcher]
         |-- Click-to-Move Intent: Translates screen raycast -> 3D world coord -> Server validation
         |-- NPC Click: Pathfinds within 5.0m range -> Triggers Lua Dialogue Tree
         |-- Skill Cast: Faces target (Yaw rotation) -> Plays Attack Animation -> Applies Authoritative Damage
```

---

## 3. Test Verification Matrix (Acceptance Criteria)

| Test ID | Test Scenario | Expected Result | Actual Result | Status |
| :---: | :--- | :--- | :--- | :---: |
| **TC-01** | World Bundle Scene Ingestion | Loads Map 53 metadata and spawns correctly | Map 53 loaded; 3 entities bound cleanly | **PASS** |
| **TC-02** | Camera Rig Setup | Pitch $45^\circ$, FOV $35^\circ$, distance 25m | Configured per canonical specification | **PASS** |
| **TC-03** | Click-to-Move Interpolation | Interpolates from $(351.0, 392.0)$ to $(353.125, 390.625)$ with yaw heading | Smooth 5-frame interpolation (Yaw: $122.91^\circ$) | **PASS** |
| **TC-04** | NPC Dialogue Interaction | Interacting within range opens dialogue tree | Dialogue tree opened with Tiệm Rèn at $0.0$m | **PASS** |
| **TC-05** | Server-Authoritative Combat | Player casts skill, monster takes damage, transitions to `DIE` | 2 hits ($450 + 600$), HP: $0/1000$, state: `DIE` | **PASS** |
| **TC-06** | Dual-Client Compatibility | Client 2D network packets and gameplay rules unaffected | Server protocol framing preserved 100% | **PASS** |
