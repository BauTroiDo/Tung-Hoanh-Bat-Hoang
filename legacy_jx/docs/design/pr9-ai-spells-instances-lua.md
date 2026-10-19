# PR9 Design & Verification: Creature AI, 5-Element Spells, Instance Isolation & Lua Safety

## 1. Overview & Objectives
- **Target:** Verify and preserve core gameplay mechanics: Creature AI state machines, 5-element martial arts calculations, multi-instance battleground isolation, and Lua scripting handle safety.
- **Key Deliverables:**
  1. `tools/test_ai_spell_instance.py`: Automated parity test fixture for AI leash, 5-element damage math, instance state isolation, and Lua boundary checking.
  2. Zero regression guarantee on existing JX1 damage calculations, elemental affinities, and quest script execution.

---

## 2. Subsystems Architecture

### 2.1 Creature AI State Machine (`KNpcAi`)
```
[IDLE] (At Spawn Position)
   |
   | Player enters aggro range (<= 10.0m)
   v
[CHASE] (Moves towards player)
   |
   +---> If distance <= 3.0m: Transition to [ATTACK]
   |
   +---> If distance from spawn > Leash (20.0m): Transition to [RETURN_HOME]
   |
[RETURN_HOME] (Returns to original spawn point, disengages combat) -> Back to [IDLE]
```

### 2.2 5-Element Spell / Aura Pipeline (Ngũ Hành Tương Khắc)
- **Dominant (+50% damage):** Kim khắc Mộc, Mộc khắc Thổ, Thổ khắc Thủy, Thủy khắc Hỏa, Hỏa khắc Kim.
- **Recessive (-50% damage):** Mộc bị Kim khắc, Thổ bị Mộc khắc, Thủy bị Thổ khắc, Hỏa bị Thủy khắc, Kim bị Hỏa khắc.
- **Neutral (100% damage):** Cùng hệ hoặc không tương khắc.

### 2.3 Instance Map Isolation (`KMissionArray`)
- Multi-instance maps (e.g., Tống Kim, Vượt Ải, Lôi Đài) maintain distinct `instanceId` tokens.
- Kill scores, NPC spawns, and timers remain strictly encapsulated within their respective instances without cross-talk.

### 2.4 Lua Handle Safety
- All Lua C-API functions (`ScriptFuns.cpp`) validate entity handles (`0 < nIndex < MAX_NPC`).
- Invalid, unloaded, or negative handles return safely without causing access violations or server crashes.

---

## 3. Test Verification Matrix (Acceptance Criteria)

| Test ID | Test Scenario | Expected Result | Actual Result | Status |
| :---: | :--- | :--- | :--- | :---: |
| **TC-01** | Kim vs Mộc Combat Multiplier | $+50\%$ damage bonus ($1.50\times$) | $1.50\times$ | **PASS** |
| **TC-02** | Kim vs Hỏa Combat Multiplier | $-50\%$ damage penalty ($0.50\times$) | $0.50\times$ | **PASS** |
| **TC-03** | Kim vs Thổ Combat Multiplier | Neutral damage ($1.00\times$) | $1.00\times$ | **PASS** |
| **TC-04** | AI Leash & Return-to-Home | Mob returns home when pulled $>20.0$m | State: `RETURN_HOME` | **PASS** |
| **TC-05** | Instance State Isolation | Instance 1 and Instance 2 do not share scores/entities | 100% isolated | **PASS** |
| **TC-06** | Lua Boundary & Invalidation | Out-of-bounds indices rejected safely | Boundary guards pass | **PASS** |
