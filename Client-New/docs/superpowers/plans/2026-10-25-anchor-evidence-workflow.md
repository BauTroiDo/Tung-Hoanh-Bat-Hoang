# Anchor evidence workflow implementation plan

Spec: docs/CN02_COORDINATE_EVIDENCE.md; baseline 8bfe8bc. Native execution requested by user.

- [x] Read-only audit legacy conversion origin; keep Unity2022 spawn alignment distinct from 4016/new scene calibration.
- [x] Test-first Domain assessment: exactly 3 fit + 1 independent holdout, explicit tolerance, evidence metadata, synthetic/draft rejection, no automatic Verified state.
- [x] Editor UI Toolkit form: capture named scene markers, attach evidence file hashes, import/export versioned draft JSON with scene fingerprint, assess without changing map catalog.
- [x] Real Unity tests for metadata/numerics and editor capture/export/round-trip/stale scene guard; full EditMode and PlayMode suites.
- [x] Export actual unpaired draft, record evidence/checkpoint and commit. Do not open CN03 or claim real calibration.
