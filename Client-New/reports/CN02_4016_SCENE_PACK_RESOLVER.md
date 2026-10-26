# CN02-04 — Truy nguồn maplist và scene key trong client 4016

**Ngày:** 25-10-2026 (UTC+07); branch `cg/cn02-anchor-evidence`; baseline `32fb062`.

## Kết quả xác minh

Trên file IL2CPP gốc (chỉ đọc), đã xác nhận thêm bằng địa chỉ lệnh `call` và mapping MethodDef đúng assembly:

| Nhánh | Hàm gọi | RVA call | Hàm nhận |
|---|---|---|---|
| Map pack | `MapDataService.LoadForTemplate` | `0x8C9F59` | `ResolveMapFolderPath` |
| Maplist | `ResolveMapFolderPath` | `0x8CD367`, `0x8CD391` | `ParseMaplistFile` |
| Maplist | `ResolveMapFolderPath` | `0x8CD40B` | `TryMaplistPath` |
| Đọc / phân tích maplist | `ParseMaplistFile` | `0x8CC194`, `0x8CC19E` | `ReadStreamingBytes`, `ParseMaplist` |
| Scene async | `<LoadScene>d__96.MoveNext` | `0xA6F841`, `0xA6F9A5` | `GameManager.RequestLoadMap` |
| Scene async | `<LoadSceneAsync>d__24.MoveNext` | `0xC8DAD7` | `SceneLoadService.LoadSceneInternal` |
| Scene key | `<LoadSceneInternal>d__26.MoveNext` | `0xC8EA74`, `0xC8ECC7` | `SceneLoadService.ResolveVariantKeyAsync` |
| Addressables | `<LoadSceneInternal>d__26.MoveNext` | `0xC8F16A`, `0xC8F26E` | `SceneLoadService.LoadAddressableMapSceneAsync` |
| Scene container | `<LoadSceneInternal>d__26.MoveNext` | `0xC8F5ED` | `LoadSceneContainerAsMapSceneAsync` |
| Prefab | `<LoadSceneInternal>d__26.MoveNext` | `0xC8F809` | `LoadPrefabsAsync` |

Các hàm được trỏ về từ bản gốc `Assembly-CSharp.dll` thông qua CodeGenModule; các lời gọi trên là địa chỉ machine-code thực. Một số hàm runtime thunk có rất nhiều alias trên cùng native RVA. Công cụ mới **không** tùy tiện gán tên cho thunk có trên ba alias.

## Bổ sung đoạn code đã bỏ sót

`MapDataService.LoadForTemplate` tại RVA `0x8C9BF0` có 2 vùng `.pdata` liền nhau, từ `0x8C9BF0` đến `0x8C9F7C`, tổng cộng **908 byte và 175 lệnh giải mã**. Phần gọi `ResolveMapFolderPath` ở đoạn tiếp nối; chỉ phân tích entry đầu tiên có thể bỏ sót hoàn toàn cơ chế tìm maplist.

`GameManager.LoadScene` chủ yếu điều phối một async state machine; đường tải scene thực tế còn đi qua `<LoadScene>d__96.MoveNext` và `SceneLoadService` với các `MoveNext` khác. Việc có các lời gọi Addressables xác nhận cơ chế, **không** xác nhận AssetReference / scene key Map53 đã phân giải thành công.

## Những gì còn chưa chứng minh

- Chưa tìm được bản ghi `maplist` được client 4016 chọn cho Map 53, cũng như path của gói map theo `templateId=53`.
- Chưa xác minh scene key chính thức được `ResolveVariantKeyAsync` chọn cho Map 53, và chưa nghiệm thu tải scene/render trên runtime.
- Không có snapshot runtime của sáu hệ số affine `m00,m01,m10,m11,offsetX,offsetZ`; hệ số mặc định từ `.cctor` không thay thế được bằng chứng đang chạy.
- Chưa có 3 điểm mốc không thẳng hàng cùng cảnh legacy↔Unity và điểm holdout thứ tư. Các vị trí tile hoặc marker synthetic không tính là anchor.
- CodeRegistration tổng thể còn chưa xác minh độc lập, dù các direct-call và native pointers trong mục này đã có đối chiếu.

**Gate giữ nguyên:** CN02-04 BLOCKED / NOT VERIFIED, CN02 PARTIAL, CN03 CLOSED.

## Tái lập

- `tools/reference4016/il2cpp/scene_pack_resolver.py`
- `tools/reference4016/il2cpp/verify_scene_pack_resolver.py`
- `reports/evidence/cn02-4016-il2cpp/scene-pack-resolver.json`

Tệp tham chiếu không đổi:
- `GameAssembly.dll` SHA256 `011357720B210AA1F392A8B62FF8DE090D3A895230A6E0135BCE707196871069`.
- `global-metadata.dat` SHA256 `5586F416AA26EBE57EA7F6580D0A53E5E5F29C50E425E64A4D3F2A95D6069E08`.

Chạy từ `Client-New`:

```powershell
python tools/reference4016/il2cpp/scene_pack_resolver.py --binary "E:\Games\VLTK3D\SwordOnline\Client-3D-2\update\GameAssembly.dll" --metadata "E:\Games\VLTK3D\SwordOnline\Client-3D-2\update\Game_Data\il2cpp_data\Metadata\global-metadata.dat" --module reports/evidence/cn02-4016-il2cpp/pe-module-candidates.json --loadmap reports/evidence/cn02-4016-il2cpp/loadmap-selector.json --out reports/evidence/cn02-4016-il2cpp/scene-pack-resolver.json
python tools/reference4016/il2cpp/verify_scene_pack_resolver.py
```

New static regression **17/17 PASS**, và sáu bộ kiểm thử tĩnh từ checkpoint trước chạy lại **19/19 + 19/19 + 14/14 + 11/11 + 10/10 + 10/10 PASS**. Tổng **100/100 PASS**, exit code 0 cho tất cả các bộ. Không build lại Unity vì không thay đổi `UnityProject`; không chạy client tham chiếu, không chỉnh Bishop/GameServer/DB và không chép asset gốc.

**Bước tiếp theo:** tìm bản ghi thực trong `maplist`/map-pack config và nguồn variant scene key, rồi so khớp read-only với scene bundle Map 53; cuối cùng yêu cầu nghiệm thu runtime/3+1 trước tích hợp.
