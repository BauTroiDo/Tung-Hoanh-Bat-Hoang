# 09 — Quyết định & risk register

## Đã chốt

| ID | Quyết định | Lý do |
|---|---|---|
| D-001 | Dựng Unity Project mới có source C#, không tiếp tục phụ thuộc Frida/binary patch | Có thể sửa lâu dài trong Editor |
| D-002 | Một lộ trình CN-00→CN-06 phục vụ server M0/M1/M2 | Tránh nhiều roadmap cạnh tranh |
| D-003 | Vertical slice Map53 + một nhân vật trước | Game chơi được trước |
| D-004 | Giữ original Bishop/gs001/3 nhân vật, không tạo backend khác | Bảo toàn dữ liệu và đúng hệ thống |
| D-005 | Client 4016 là reference, không lấy làm project source | IL2CPP build không hoàn nguyên trọn source |
| D-006 | Unity 6000.3.9f1 là target editor mở đầu, phải xác minh môi trường | Match runtime tham khảo và hạn chế asset migration |
| D-007 | Client mới theo ITransport/adapter, web qua WebSocket phù hợp | Đa nền tảng; không raw TCP web |
| D-008 | Asset source/license manifest trước khi import/commit | Quyền tài sản và an toàn phân phối |
| D-009 | Owner/claim và review chéo hai agent | Chống xung đột file |

## Chưa chốt (OPEN)

- O-001: Format v1 wire envelope: protobuf, MessagePack, JSON binary hay custom; yêu cầu benchmark/security review.
- O-002: Việc dùng lại đúng asset Ba Lăng huyện 4016 có được cấp phép hay không. Nếu chưa, phải xây scene hợp lệ mới.
- O-003: Tọa độ 2D legacy→Unity anchor/rotation/height được đo lại thế nào.
- O-004: Unity Editor thực sự đã cài? Chưa tìm thấy đường dẫn chuẩn; cần CN-00 khảo sát sâu/cài nếu cho phép.
- O-005: Web transport/gateway endpoint v1 cần server review và triển khai riêng, không coi có sẵn.
- O-006: Cơ chế navmesh/client interpolation vs authoritative server movement.

## Risk register

| Rủi ro | Mức | Phát hiện/giảm thiểu |
|---|---|---|
| Missing Editor/Windows Build Support | Cao | Kiểm tra path/version/license trước tạo project |
| Asset/IP/provenance không rõ | Rất cao | Manifest, không import nếu chưa có quyền |
| Asset Unity2022 không tương thích Unity6 | Cao | Export/import sandbox, never raw DLL swap |
| Map53 scene có HUD nhưng không có ground collider | Cao | Scene root, bounds, renderer, raycast gates |
| Avatar position không khớp scene | Cao | 3+1 anchor & editor gizmo/PlayMode |
| M1 movement log chỉ auto login chứ không phải click | Cao | Timestamp độc lập user click + GS |
| Gateway v1 khác legacy native experimental | Rất cao | Contract negotiation/interop tests, live gate riêng |
| Hai AI sửa chung một file | Trung bình | Owner/claim, branches và integration review |
| Web raw TCP & Unity native plugin | Cao | ITransport/WebSocket, Web build gate |
| Đẩy bí mật/asset proprietary lên GitHub | Rất cao | .gitignore, secret scan, asset rights review |
| Scope creep sang combat/map khác | Cao | Board + reviewer chặn ngoài gate |

## Quy trình đổi quyết định

Mọi đề nghị thay đổi D-xxx: PR/issue với lý do, evidence, phạm vi file, ảnh hưởng roadmap/gate, security/legal, rollback. Chỉ thay sau review và cập nhật AGENTS/roadmap nếu cần.
