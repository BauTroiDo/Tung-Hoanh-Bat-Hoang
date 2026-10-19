# PR6 Design & Verification: Canonical World Data Converter (2D -> 3D)

## 1. Overview & Objectives
- **Target:** Establish the canonical conversion pipeline from legacy 2D discrete grid coordinates and maps into the unified **Canonical World Bundle** specification for 3D isometric representation.
- **Vertical Slice Map Selection:** Map ID 53 (**Ba Lăng Huyện** - Starter Village), featuring village hub, shopkeeper/quest NPCs, portals to major cities, and outdoor monster fields.
- **Key Deliverables:**
  1. `tools/canonical_world_converter.py`: Automated, deterministic conversion and validation engine.
  2. Coordinate contract with verified 100% round-trip precision (0 pixel tolerance).
  3. Canonical World Bundle generation for Map 53 with cryptographically verified SHA-256 manifests.

---

## 2. Coordinate Transformation Contract

```
Legacy 2D Discrete Grid (Pixels) <----------------------------------> Canonical 3D World (Meters)
[X_legacy, Y_legacy]               SCALE = 0.03125 (32 px = 1.0 m)     [X_3d, Y_3d, Z_3d]
```

### Formulas:
- **Forward Transform (2D -> 3D):**
  $$X_{3d} = X_{legacy} \times 0.03125$$
  $$Y_{3d} = \text{Elevation} = 0.0$$
  $$Z_{3d} = Y_{legacy} \times 0.03125$$
- **Reverse Transform (3D -> 2D):**
  $$X_{legacy} = \text{round}(X_{3d} / 0.03125)$$
  $$Y_{legacy} = \text{round}(Z_{3d} / 0.03125)$$
- **Precision Verification:**
  - Tested on 10,000 distributed sample points across the logic coordinate space ($0 \le X, Y \le 100,000$).
  - **Result:** $10,000 / 10,000$ points recovered bit-exact with **0 pixel error**!

---

## 3. Canonical World Bundle Structure (`world_bundles/`)
```
world_bundles/
├── manifest.json                   # Bundle metadata, schema version, SHA-256 checksums
└── maps/
    └── 53/                         # Vertical slice: Ba Lăng Huyện
        ├── map.meta.json           # Map dimensions, coordinate contract, portal anchors
        ├── regions.index           # 10x10 region grid boundaries and 3D centers
        ├── spawns.json             # Canonical spawn definitions with stable 64-bit IDs
        ├── collision.json          # Low-poly collision proxy layers & village walls
        └── navmesh.json            # Navigation waypoints and entry/exit links
```

---

## 4. Test Verification Matrix (Acceptance Criteria)

| Test ID | Test Scenario | Expected Result | Status |
| :---: | :--- | :--- | :---: |
| **TC-01** | Coordinate Round-Trip Precision | $\text{to\_legacy}(\text{legacy\_to\_3d}(x, y)) == (x, y)$ across 10,000 points | **PASS (0 err)** |
| **TC-02** | Deterministic Converter Output | Consecutive runs produce identical byte-for-byte SHA-256 hashes | **PASS (100% match)** |
| **TC-03** | Map Metadata Extraction | Map 53 correctly parsed with dimensions, bounds, and portals | **PASS** |
| **TC-04** | Stable 64-Bit Spawn IDs | Every NPC in canonical bundle has unique, bit-shift deterministic ID | **PASS** |
| **TC-05** | Collision & NavMesh Blockout | Low-poly collision proxies and path waypoints generated properly | **PASS** |
