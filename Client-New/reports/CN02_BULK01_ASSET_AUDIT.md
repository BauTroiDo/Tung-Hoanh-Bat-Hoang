# Báo cáo Nghiệm thu BULK-01 — Kiểm tra và Sửa hàng loạt Scene, Material, Prefab

- **Thời gian (UTC):** `2026-10-25T23:46:15.8759961Z`
- **Unity Version:** `6000.6.7f1`
- **Tổng số file trong `Assets/_JXClient/**` đã quét:** `51`
- **Missing `.meta`:** `0`
- **Duplicate GUID:** `0`
- **Tổng số lỗi còn tồn đọng:** `0`
- **Kết luận BULK-01:** `PASS`

## 1. Kiểm toán và Hoàn thiện 8 Material URP (Antigravity Lane)

| Material | Shader | `_BaseColor` | `_Color` (Synced) | Smoothness | RenderQueue | Missing Tex | Biện pháp xử lý / Trạng thái | SHA256 |
|---|---|---|---|---|---|---|---|---|
| `BLH_PlazaTile` | `Universal Render Pipeline/Lit` | `(0.54, 0.50, 0.43, 1.00)` | `(0.54, 0.50, 0.43, 1.00)` | `0.30` | `2000` | `0` | Verified clean (URP/Lit _BaseColor and _Color synced) | `88858F188DA6...` |
| `BLH_GrassTerrain` | `Universal Render Pipeline/Lit` | `(0.22, 0.38, 0.24, 1.00)` | `(0.22, 0.38, 0.24, 1.00)` | `0.15` | `2000` | `0` | Verified clean (URP/Lit _BaseColor and _Color synced) | `C31D56F27A80...` |
| `Wall` | `Universal Render Pipeline/Lit` | `(0.34, 0.40, 0.48, 1.00)` | `(0.34, 0.40, 0.48, 1.00)` | `0.50` | `2000` | `0` | Verified clean (URP/Lit _BaseColor and _Color synced) | `25168DFC1FBE...` |
| `Ground` | `Universal Render Pipeline/Lit` | `(0.16, 0.34, 0.27, 1.00)` | `(0.16, 0.34, 0.27, 1.00)` | `0.50` | `2000` | `0` | Verified clean (URP/Lit _BaseColor and _Color synced) | `1D4864D4F78D...` |
| `BLH_StoneRoad` | `Universal Render Pipeline/Lit` | `(0.46, 0.44, 0.39, 1.00)` | `(0.46, 0.44, 0.39, 1.00)` | `0.25` | `2000` | `0` | Verified clean (URP/Lit _BaseColor and _Color synced) | `46A3BC77A08A...` |
| `BLH_HouseWall` | `Universal Render Pipeline/Lit` | `(0.68, 0.63, 0.54, 1.00)` | `(0.68, 0.63, 0.54, 1.00)` | `0.20` | `2000` | `0` | Verified clean (URP/Lit _BaseColor and _Color synced) | `F890C19641CB...` |
| `BLH_WoodTimber` | `Universal Render Pipeline/Lit` | `(0.42, 0.27, 0.16, 1.00)` | `(0.42, 0.27, 0.16, 1.00)` | `0.20` | `2000` | `0` | Verified clean (URP/Lit _BaseColor and _Color synced) | `AAC3BDAE7617...` |
| `BLH_BoundaryRock` | `Universal Render Pipeline/Lit` | `(0.36, 0.38, 0.40, 1.00)` | `(0.36, 0.38, 0.40, 1.00)` | `0.15` | `2000` | `0` | Verified clean (URP/Lit _BaseColor and _Color synced) | `AAE70478475B...` |
| `BLH_TileRoof` | `Universal Render Pipeline/Lit` | `(0.28, 0.32, 0.38, 1.00)` | `(0.28, 0.32, 0.38, 1.00)` | `0.35` | `2000` | `0` | Verified clean (URP/Lit _BaseColor and _Color synced) | `4BCE61055280...` |
| `Avatar` | `Universal Render Pipeline/Lit` | `(0.94, 0.66, 0.19, 1.00)` | `(0.94, 0.66, 0.19, 1.00)` | `0.50` | `2000` | `0` | Verified clean (URP/Lit _BaseColor and _Color synced) | `840D19A61266...` |

## 2. Kiểm toán Prefab (`Assets/_JXClient/**/Prefabs`)

| Prefab | Đường dẫn | GameObjects | MeshRenderers | Colliders | Missing Scripts | Null Mesh/Mat | Transform/Bounds | Kết quả |
|---|---|---|---|---|---|---|---|---|
| `BLH_SyntheticVillageHouse` | `Assets/_JXClient/Environment/Prefabs/BLH_SyntheticVillageHouse.prefab` | `3` | `2` | `2` | `0` | `0` | `Valid` | `PASS` |
| `OfflinePlayer` | `Assets/_JXClient/Prefabs/OfflinePlayer.prefab` | `2` | `1` | `1` | `0` | `0` | `Valid` | `PASS` |
| `BLH_SyntheticGateArchway` | `Assets/_JXClient/Environment/Prefabs/BLH_SyntheticGateArchway.prefab` | `4` | `3` | `3` | `0` | `0` | `Valid` | `PASS` |

## 3. Kiểm toán Scene Hierarchy, Collider, Layer, Camera & Lighting

| Scene | In Build | Roots | Total GOs | Required 7 Roots | Renderers | Ground Colliders (L8) | Obstacle Colliders (L0) | Connected Prefabs | Broken Prefabs | Camera Clipping | Lighting | Kết quả |
|---|---|---|---|---|---|---|---|---|---|---|---|---|
| `BaLangHuyen` | `True` | `11` | `55` | `True` | `25` | `4` | `21` | `7` | `0` | `0.30..1000` | `Dir (1.45)` | `PASS` |
| `MovementTest` | `True` | `7` | `10` | `True` | `3` | `1` | `2` | `0` | `0` | `0.30..1000` | `Dir (1.50)` | `PASS` |
| `Bootstrap` | `True` | `5` | `5` | `True` | `2` | `0` | `2` | `0` | `0` | `0.30..1000` | `Dir (1.20)` | `PASS` |
