# CN-01 → CN-06 — Gói nhiệm vụ sẵn sàng sau CN-00

**Không thực hiện các gate này cho đến khi gate trước PASS.** Tệp này đóng vai trò task spec kế tiếp, còn status chính ở TASK_BOARD.md.

## CN-01 — Offline player + input + collider

**Inputs:** Win64 Bootstrap CN-00 đã chạy thật, Unity Editor project đã commit.
**Implement:** MovementTest.unity, Ground layer, player prefab/collider, Orbit/Follow camera, mouse/touch raycast, navigation destination, speed/animation state, UI click shielding, debug metrics.
**Tests:** CN01-01..06 docs/05_ACCEPTANCE_TESTS.md; PlayMode thực ít nhất ground hit/no hit/UI block/obstacle/3 camera cases.
**Artifact:** Video/screens, code tests, reports/CN-01_ACCEPTANCE.md.
**PASS:** người dùng click mặt đất và avatar đi được **offline**; không dùng GS mock để đạt Gate này.
**Owner:** Antigravity.

## CN-02 — Ba Lăng huyện editable

**Inputs:** CN-01 PASS; asset manifest quyền sử dụng và tọa độ có dữ liệu.
**Implement:** BaLangHuyen.unity, ground meshes, material URP, colliders, lighting, spawn/camera bounds, map coordinates test, platform quality.
**Fallback:** Nếu không thể tái sử dụng tài nguyên third-party có phép, dùng terrain tự tạo và ghi rõ synthetic; synthetic có thể test công nghệ nhưng **không đủ để gọi exact original BLH restored**.
**Tests:** CN02-01..06, 5 ground raycast, 3+1 anchor, no blue background.
**Artifact:** Scene/Hierarchy screenshots và asset provenance report.
**PASS:** scene chỉnh sửa thật trong Editor với mặt đất/collider và logic tọa độ đã xác minh.
**Owner:** Antigravity Asset/Scene.

## CN-03 — Bishop / Gateway / real GS

**Inputs:** CN-02 PASS; review security/protocol v1, owner server approved.
**Implement:** ITransport, codec, handshake, auth + roles, map world snapshot, entity spawn, state machine, disconnect/reconnect.
**Constraint:** Original Bishop/gs001 remains authoritative. Gateway v1 chưa có; cần test+feature flag+rollout/rollback riêng, tuyệt đối không coi opcode v1 đã chạy.
**Tests:** CN03-01..06, wrong role, expired permit, limits, real login/Map53/entity.
**Artifact:** Sanitized client/gateway/GS same-session timestamps; no credentials or raw tickets.
**PASS:** one existing character actually appears at authoritative Map53 position.
**Owner:** Antigravity Network + ChatGPT contract/security reviewer.

## CN-04 — Authoritative movement end-to-end

**Inputs:** CN-03 PASS, renderer/collider tested.
**Implement:** click event, world-map coordinate conversion, MoveRequest seq, server authoritative reply, player view reconciliation, jitter/timeout/reconnect.
**Tests:** CN04-01..08; deliberately separated user click timestamp from auto-login movement. GS rejection/duplicate/replay cases.
**Artifact:** Screen recording, correlated sanitized click->gateway->gs001->Unity trace.
**PASS:** avatar actually moves after user action and matching GS updated position.
**Owner:** Antigravity gameplay + ChatGPT independent reviewer.

## CN-05 — Windows playable release

**Inputs:** CN-04 PASS.
**Implement:** Win64 stable output, splash, settings, logs, installer optional, smoke tests, resource lifetime, perf baselines, debug symbols separation.
**Tests:** clean build, cold launch, Map53 login+movement 3 cycles, reconnect, no runtime dependence on 4016 or Frida, hashes, source/license scan.
**Artifact:** Builds/Windows, reports/CN-05_ACCEPTANCE.md, tagged branch after signoff.
**PASS:** independent Windows x64 game playable on target PC.
**Owner:** Antigravity Release.

## CN-06 — Web + Android; iOS gated

**Inputs:** CN-05 PASS, approved WSS endpoint, Web/Android build modules.
**Implement:** WebSocketTransport, Web UI/input, assets and memory optimization; Android touch, background/resume, ARM64. iOS only with macOS/Xcode environment.
**Tests:** Web browser real build, Android device/emulator real build, network and move accuracy, read-only original GS side effects.
**Artifact:** separate platform builds plus FPS/memory/latency evidence.
**PASS:** Web and Android actually load/play vertical slice; iOS status independently gated.
**Owner:** Antigravity Cross-platform.

## Chung cho mọi gate

Before: claim in TASK_BOARD, record baseline and source rights. During: smallest diff, tests; after: commit/report/checkpoint/reviewer. Unknown evidence => BLOCKED/NOT TESTED, not PASS. Never expand to combat, multi-map/quests etc until CN-05.
