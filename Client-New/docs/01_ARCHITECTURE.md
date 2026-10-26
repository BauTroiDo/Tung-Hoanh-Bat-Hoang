# 01 — Kiến trúc Client-New / Unity C#

## Mục tiêu

Client **render + input + prediction hạn chế**; GameServer JX1 là nguồn chân lý của nhân vật, vị trí, HP, vật phẩm. Mã C# có source chỉnh sửa được, test module độc lập, không phụ thuộc Frida/client4016.

## Tầng hệ thống

~~~text
Unity PC / Web / Mobile
  Bootstrap/UI/Session
       |
  Input -> GroundRaycaster -> MoveIntent
       |                       |
  WorldScene + EntityView <- WorldPresenter <- WorldStore
       |                                      ^
       v                                      |
  AssetCatalog / SceneLoader          Domain event reducers
                                              ^
                                      ProtocolCodec v1
                                              ^
                                  ITransport / WebSocket
                                              |
                                 Versioned Gateway adapter
                                        |           |
                                    Bishop        JX1 gs001
                                   (Auth)         (World)
~~~

**Dấu lưu ý:** sơ đồ phía Gateway là đích tích hợp **chưa tồn tại như v1 mới**; hiện Gateway phục vụ client 4016 bằng nhánh native thực nghiệm. Không giả định message v1 đã được GS hỗ trợ.

## Các module đề xuất (UnityProject/Assets/_JXClient)

- **Core**: GameBootstrap, GameClock, lifetimes, DI nhẹ, diagnostics.
- **Domain**: GameSessionState, PlayerId, MapId, WorldSnapshot, Position, MoveIntent; không phụ thuộc UnityEngine.
- **Networking**: ITransport, WebSocketTransport, ProtocolCodec, NetworkStateMachine, reconnect, backpressure.
- **Authentication**: LoginViewModel, RoleList, role selection, session entitlement.
- **World**: WorldManager, MapSceneLoader, MapCoordinateConverter, WorldStore, EntitySpawner, Despawner.
- **Movement**: GroundRaycaster, MovementController, MovementPresenter, ServerReconciliation.
- **Camera**: follow/zoom/clamp, orientation independent world model.
- **UI**: Bootstrap, Login, Roster, HUD, network diagnostics.
- **Diagnostics**: masked logs, event correlation, FPS, scene manifest, test-only overlays.
- **Tests**: EditMode independent contract/coordinate tests; PlayMode renderer/movement; integration optional.

## State machine bắt buộc

AppBoot → OfflineReady/Login → Authenticating → Roles → JoiningWorld → LoadingScene → AwaitingSnapshot → PlayerSpawned → WorldReady → Reconnecting/Disconnected.

Cấm MoveRequest trước WorldReady. Chuyển WorldReady phải dựa trên **cả** scene hoàn thành, collider/raycast sẵn sàng, authoritative player ID/position và protocol entry complete, không dựa riêng HUD hay ACK.

## Ownership của hệ thống

- Server sở hữu identity, entity ID, scene/map ID và vị trí cuối; client không tự cộng vàng/HP, teleport tùy ý.
- Client gửi MoveIntent với sequence và tọa độ đích đã kiểm tra; server validate.
- WorldStore cập nhật các snapshot/delta theo session epoch, map epoch; bỏ event cũ sau reconnect.
- EntityView là biểu diễn hiển thị và không giữ trạng thái kinh tế/gameplay authoritative.
- SceneLoader phải hỗ trợ cancel, unload, timeout, một scene đang tải cùng lúc, và rollback nếu lỗi.

## Performance PC/Web/Mobile

- Tách dữ liệu thuần C# khỏi Unity MonoBehaviour.
- Pool entity/UI, AOI updates, loading scene theo khu vực khi server hỗ trợ; không preload cả thế giới.
- Phân cấp chất lượng: PC High/Medium; mobile Low/Medium; Web conservative.
- Không giả định đa luồng hoặc raw TCP .NET khả dụng trên Web.
- Memory budget/GC/asset lifetime đo bằng Profiler trước khi tối ưu.

## Bảo mật

- Không đưa raw Bishop/GS private ticket vào log, file cấu hình hoặc inspector.
- Bounds check mọi gói, số entity, danh sách, độ dài chuỗi; khoá theo phiên/người chơi/map.
- Không tự ghi đè original GameServer/DB. Backend adapter phải triển khai riêng theo approval và regression.
- Tài sản third-party chỉ nhập sau manifest provenance/legal.

## Interface review gates

Trước khi code network: duyệt docs/02_PROTOCOL.md.
Trước khi code world: duyệt docs/03_WORLD_COORDINATES.md.
Trước khi nhập prefab/material: duyệt docs/04_ASSET_SCENE.md.
Trước khi báo chạy được: docs/05_ACCEPTANCE_TESTS.md.
