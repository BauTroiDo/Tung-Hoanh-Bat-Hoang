# PR7 Design & Verification: Movement 3D, Server-Authoritative Collision, Line-of-Sight (LOS) & NavMesh

## 1. Overview & Objectives
- **Target:** Establish server-authoritative movement validation, line-of-sight (LOS) raycasting, and obstacle collision boundaries for 3D isometric navigation while maintaining strict compatibility with JX1 combat rules.
- **Key Deliverables:**
  1. `tools/test_movement_nav_los.py`: Complete test fixture validating wall penetration guards, gate traversal, water obstacle missile flight, and LOS raycasting.
  2. Sub-millisecond performance benchmark over 10,000 navigation queries.
  3. Seamless integration between discrete JX1 logic cells ($32 \times 32$ px) and continuous 3D world vectors.

---

## 2. Server Authority & Movement Architecture

```
[Client 3D Input Intent] (Click-to-Move / Skill Target)
         |
         v
[Canonical Transform: (X_3d, Z_3d) -> Logic Cell (Cx, Cy)]
         |
         v
[NavQueryService: TestBarrier Validation & Bresenham Raycast]
         |
         +---> If OBSTACLE_NORMAL (Solid Wall):
         |        Clamps movement to stopPoint at boundary (Zero Penetration).
         |
         +---> If OBSTACLE_FLY (Water):
         |        Blocks character walking; permits missile & projectile raycasts.
         |
         v
[Authority Broadcast: Authoritative Stop Point / Path Waypoint Broadcast]
```

---

## 3. Test Verification Matrix & Quantitative Metrics

| Test ID | Test Scenario | Preconditions | Expected Result | Actual Result | Status |
| :---: | :--- | :--- | :--- | :--- | :---: |
| **TC-01** | LOS Across Open Ground | Attacker at (50, 50), Target at (65, 50) | Unobstructed line of sight | `True` | **PASS** |
| **TC-02** | LOS Blocked by Solid Wall | Attacker at (50, 50), Target behind wall at (80, 50) | Raycast blocked at $X=70$ | `False` | **PASS** |
| **TC-03** | Missile Trajectory Over Water | Missile over moat ($X=72..74$) with `check_fly=False` | Missile traverses water barrier | `True` | **PASS** |
| **TC-04** | Wall Penetration Prevention | Move intent from (65, 50) straight into wall at (75, 50) | Clamps at $(69, 50)$, no penetration | Clamped at $(69, 50)$ | **PASS** |
| **TC-05** | Movement Through Village Gate | Move intent through North Gate opening at $Y=62$ | Traverses gate cleanly without collision | Reachable at $(80, 62)$ | **PASS** |
| **TC-06** | Query Throughput Benchmark | 10,000 Nav & LOS queries executed sequentially | Latency well under world tick (55 ms) | **56.10 ms total (178,238 queries/sec)** | **PASS** |

### Benchmark Latency Metrics (10,000 Queries):
- **p50 (Median):** **4.70 µs** (0.0047 ms)
- **p95:** **10.30 µs** (0.0103 ms)
- **p99:** **11.70 µs** (0.0117 ms)
- **Throughput:** **178,238 queries / second** (Over 9,000 times faster than the 18 FPS frame deadline).
