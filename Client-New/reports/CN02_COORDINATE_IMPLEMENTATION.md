# Coordinate Math Verification / Integration Handoff

**Owner:** Codex/ChatGPT. **Branch:** cg/coordinate-math, isolated checkout .tmp/coordinate-worktree.
**Baseline:** 0ac5669 (CN01 implementation 755747f, user acceptance, ownership reservation).
**Gate:** CN01 independent review PASS observed in root reports/CN-01_REVIEW.md at 2026-10-25 15:57 UTC+07 before production module implementation. Root CN02 scene work remains Antigravity-owned.
**Component status:** IMPLEMENTED / TESTED. Real Map53 calibration: UNVERIFIED.

## Source/API

New pure Domain source: Assets/_JXClient/Domain/Coordinates/AffineMapTransform.cs. Existing JXClient.Domain asmdef still has noEngineReferences=true; no new package or assembly dependency.

```csharp
var legacyBounds = new PlanarBounds(minLegacyX, minLegacyY, maxLegacyX, maxLegacyY);
var sceneBounds = new PlanarBounds(minUnityX, minUnityZ, maxUnityX, maxUnityZ);
var mapping = AffineMapTransform.Fit(first, second, third, legacyBounds, sceneBounds);
PlanarPoint sceneXZ = mapping.ToScene(legacyXY);
PlanarPoint legacyXYAgain = mapping.ToLegacy(sceneXZ);
double residual = mapping.HoldoutError(fourthIndependentPair);
```

Namespace: JXClient.Domain.Coordinates. CoordinateAnchor pairs two PlanarPoint values (Legacy, Scene). PlanarPoint.Y means legacy horizontal Y or scene horizontal Z; it NEVER supplies Unity vertical height. Scene integration must raycast terrain separately. World can consume this via its existing Domain reference; no second coordinate converter required.

Fit stores origin-relative, immutable coefficients. Three fit anchors must be finite, within bounds and non-collinear; mapped axes must be invertible. Source geometry and transform degeneracy checks use normalized relative determinant floor 1e-12, a numerical conditioning guard, NOT a gameplay/holdout acceptance tolerance. Very extreme finite arithmetic that cannot be represented safely is rejected, not clamped. Bounds are strict inclusive comparisons; no hidden margin or gameplay tolerance is invented.

ToScene and ToLegacy validate both input and output domains. Invalid points/bounds/fits raise ArgumentException; out-of-bounds points raise ArgumentOutOfRangeException. Default invalid bounds rejected. HoldoutError returns Euclidean residual in scene units without setting any verified/authoritative state. No mapId, server, network or ground-height authority is inferred by this numerical API.

## Evidence

- Test-first standalone API shell: 11 synthetic fixtures, 10 failures and one identity pass; red exit 1. Initial identity behavior could not enforce constraints or fit real transforms.
- Implemented source: standalone 11/11 passed, exit 0, using installed Unity .NET SDK 8.0.318 and existing NUnit DLL. This was explicitly not Unity Test Runner.
- Actual Unity 6000.6.7f1 EditMode suite on isolated project: 14/14 passed (11 coordinate + 3 existing), zero failed/skipped, process exit 0.
- Actual PlayMode regression suite on same isolated project: 9/9 passed, zero failed/skipped, process exit 0.
- Unity generated all new .meta files. No root Editor was launched or borrowed. Antigravity root World/scene/asmdef changes were not modified.
- Tests cover scale/translation, rotation, reflection, inverse, negative coordinates, inclusive bounds and output bounds, large source origin at 1e12, duplicate/collinear/near-collinear input, singular/near-singular output, NaN/Infinity, arithmetic overflow, invalid/default bounds, out-of-bounds anchors and independent 3-4-5 residual.

Unity commands used installed Editor with -batchmode -nographics -projectPath pointing to the isolated checkout's UnityProject, -runTests -testPlatform EditMode/PlayMode, and result/log paths under its .tmp folder. Exact command inputs, paths, exits and hashes are recorded in verification.json. No changes to production server/reference client; no build/runtime proof of real Map53 is claimed.

Standalone SDK first launch emitted a development-certificate installation notice and workload-verification warning; no certificate trust command was run. These notices did not prevent mathematical tests. No global SDK/package install was performed.

## Integration

Cherry-pick the component commit from cg/coordinate-math when the root project is not in an active test/build/import operation. The commit only adds owned coordinate source/tests/metas, this report/evidence/plan, and updates the owned coordinate contract document. Preserve Antigravity's pending World/scene changes. Run the combined root suites after integration; root may have extra CN02 tests not present in this isolated baseline.

Do not mark CN02-04 PASS from these synthetic fixtures. There are still no verified real 3+1 Map53 anchors or measured gameplay tolerance. Antigravity supplies anchor/scene evidence; Codex reviews fitting/error/bounds. No duplication of mapping inside loader or movement.

Rollback: revert only the coordinate component commit after checking consumers; never reset/clean the shared root. Keep this isolated branch/checkpoint while integration is pending.
