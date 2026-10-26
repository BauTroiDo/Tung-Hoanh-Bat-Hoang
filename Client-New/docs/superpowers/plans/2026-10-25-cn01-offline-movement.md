# CN-01 Offline Movement Implementation Plan

**Goal:** Playable offline click/touch movement with collision, camera and UI shielding.
**Architecture:** Separate Movement assembly consumes Input System and uGUI; CharacterController moves only after validated ground input. No transport, login or WorldReady/network policy changes. Editor generator creates a new synthetic MovementTest scene and player prefab; CN00 Bootstrap remains intact.
**Tech Stack:** Unity 6000.6.7f1, URP 17.6, Input System, uGUI, Unity Test Framework.
**Spec:** tasks/CN-01_TO_CN-06.md and docs/05_ACCEPTANCE_TESTS.md (CN01-01..06).
**Execution:** Native implementation explicitly requested by the user's continuation. No subagents or separate execution approval needed.

## Constraints / review focus

- Only Client-New; production server and reference clients unchanged.
- Ground layer required; UI consumes input before physics; obstacle foreground cannot click through.
- Reject invalid/offscreen coordinates; no movement if ground disabled/missing.
- CharacterController must stop at wall and ground edge; speed bounded, no teleport.
- Three camera orbit/zoom positions must still produce a valid ground destination.
- Tests and scripted runtime movement are automated evidence; user click acceptance is recorded separately.

## Tasks

- [x] Add real PlayMode tests for ground click/movement, UI shielding, no collider, obstacle collision, camera cases and invalid input. Observe red.
- [x] Implement OfflineMovement and OrbitFollowCamera; run PlayMode plus existing EditMode suite.
- [x] Generate scene/prefab through Editor code; build to Builds/CN01-Windows (preserve CN00 build).
- [x] Run real standalone smoke and inspect image/log; distinguish scripted test from physical pointer evidence.
- [x] Record provenance, exact commands/results and checkpoint; commit narrow scope. Gate remains REVIEW unless all evidence, including independent/user click, is established.
