# CN02 lifecycle and scene-ground ownership fixes

Spec: reports/CN02_INTEGRATION_NOTES.md. Baseline 609f98f. Native execution explicitly requested by user.

- [x] Reproduce cancellation while async operation is pending; callback must occur once, cleanup complete before new load admission.
- [x] Reproduce foreign Ground satisfying missing-target probes/spawn, and foreign foreground Ground hiding real target ground.
- [x] Fix each root cause with minimal source changes. Preserve old tests and offline/network separation.
- [x] Run complete PlayMode/EditMode, separate Win64 review build and real runtime smoke.
- [x] Record review/checkpoint, investigate existing local anchor evidence read-only; do not claim real calibration without 3+1 verified pairs.
