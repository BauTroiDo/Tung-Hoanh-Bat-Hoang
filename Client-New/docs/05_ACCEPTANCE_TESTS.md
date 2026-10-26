# 05 — Acceptance & Regression Test Matrix

**PASS = thực hiện được + bằng chứng lưu; chưa thực hiện = NOT TESTED; thiếu môi trường = BLOCKED.** Không ghi PASS từ unit test mô phỏng khi gate yêu cầu real Editor/network/game.

## Gate CN-00 — Environment & build

| ID | Kiểm thử | Bằng chứng |
|---|---|---|
| CN00-01 | Unity Hub/Editor đúng version & license | Hub/Editor path, -version hoặc screenshot |
| CN00-02 | Tạo project chuẩn với Packages, ProjectSettings, .meta | Tree và Editor mở project |
| CN00-03 | Không compile error trong Editor | Editor.log đoạn summary/timestamp |
| CN00-04 | EditMode test baseline PASS | TestResults hoặc log exit0 |
| CN00-05 | Windows64 build thành công | build exit0, EXE/size/hash |
| CN00-06 | EXE tự khởi chạy, chỉ hiện bootstrap scene | screenshot/run log |
| CN00-07 | Git sạch + checkpoint + provenance | commit SHA, git status và manifest |

## Gate CN-01 — Movement offline

| ID | Kiểm thử | Bằng chứng |
|---|---|---|
| CN01-01 | Scene MovementTest có GroundCollider | Inspector / scene + collider bounds |
| CN01-02 | Click không qua UI lên Ground tạo 1 MoveIntent | Timestamp input + raycast hit |
| CN01-03 | Click vào UI KHÔNG tạo move | Input/raycast regression |
| CN01-04 | Ground không có collider => từ chối đi an toàn | PlayMode negative |
| CN01-05 | Player không xuyên vật cản/terrain | Game video/test traces |
| CN01-06 | Zoom/rotate camera vẫn raycast đúng | 3 góc camera & tests |

## Gate CN-02 — Map53 editable

| ID | Kiểm thử | Bằng chứng |
|---|---|---|
| CN02-01 | Scene BaLangHuyen.unity mở được | Unity Editor Scene View |
| CN02-02 | Có visible ground mesh/terrain + material URP | Hierarchy/Inspector/Game View |
| CN02-03 | Collider and raycast hits ground 5 sample points | PlayMode logs/coordinates |
| CN02-04 | Tọa độ legacy ↔ Unity khớp 3 anchors + test4 | Error metrics & golden fixtures |
| CN02-05 | Không xanh trống, spawn/camera/lighting đúng | 4 ảnh thực từ Game View |
| CN02-06 | Asset provenance/license hợp lệ | Approved manifest |

## Gate CN-03 — Bishop/GS real integration

| ID | Kiểm thử | Bằng chứng |
|---|---|---|
| CN03-01 | Gateway v1 handshake phiên bản đúng | Gateway logs (sanitized) |
| CN03-02 | Login xác thực và role list từ Bishop | Không lộ account/ticket |
| CN03-03 | Chọn existing role -> vào real gs001 | Server-side matched session |
| CN03-04 | Client nhận mapId53 + entityId thật | Correlated world snapshot & client |
| CN03-05 | Disconnect/reconnect không double-spawn | Negative + timeout tests |
| CN03-06 | Ba nhân vật hiện có không bị đổi | Read-only existence check |

## Gate CN-04 — authoritative movement

| ID | Kiểm thử | Bằng chứng |
|---|---|---|
| CN04-01 | Client xác nhận scene+collider+snapshot WorldReady | Stopwatch/status log |
| CN04-02 | Người dùng click **sau** WorldReady | Input T1 + scene + rayhit |
| CN04-03 | Outbound MoveRequest(seq, mapEpoch, dst) | Client log numeric no secret |
| CN04-04 | Gateway gửi lệnh hợp lệ tới GS | Same session/seq correlated |
| CN04-05 | GS chấp nhận, trả serverTick/position | Server log and bounds |
| CN04-06 | Avatar Unity thực sự di chuyển/reconcile | Onscreen proof, final delta |
| CN04-07 | Fake teleport/duplicate seq/map mismatch rejected | Negative integration |
| CN04-08 | Không nhầm chuyển động tự phát sau login là PASS | Timestamp correlation check |

## CN-05 & CN-06

- CN05: Win64 build sạch từ CI/local, EXE không phụ thuộc Frida/4016, tests/resources, cold launch, perf baseline.
- CN06: Web build thực qua WSS, Android touch/UI & perf, bundle quality variants; iOS gated on macOS.

## Format hồ sơ test

For each ID: test ID, environment/version/commit, exact command & exit, captured artifact path, expected vs actual, PASS/FAIL/BLOCKED, timestamp, reviewer. Sinh reports/CN-NN_ACCEPTANCE.md từ templates/REPORT_TEMPLATE.md.

## Tiêu chí tuyệt đối

Không đánh dấu Map53 PASS nếu màn hình chỉ có HUD mà nền xanh; không đánh dấu movement PASS nếu GS chỉ thay đổi tọa độ lúc login; không nói asset "đã có" khi collider/raycast chưa kiểm tra. Bảo mật/rollback phải đạt trước live use.

## Regression tối thiểu mỗi commit

Editor compile, unit tests domain/codec, serializer roundtrip, coordinate tests, scene spawn smoke. Với thay đổi network, thêm integration có mock **để phát hiện lỗi** nhưng vẫn cần real GS acceptance gate. Với thay đổi scene, PlayMode collider/camera tests.
