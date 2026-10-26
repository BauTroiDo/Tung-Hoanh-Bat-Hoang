# Asset Provenance Manifest — CN-00 Bootstrap

**Updated:** 2026-10-25T15:11:30+07:00, Asia/Ho_Chi_Minh
**Scope:** Gate `CN-00` (`UnityProject` initial URP template & `Bootstrap.unity`).
**Policy:** No proprietary third-party or legacy-client extracted assets are included in `CN-00`.

| Field | Record 1: Unity 6 URP Template | Record 2: CN-00 Bootstrap Scene & Scripts |
|---|---|---|
| `assetId` | `cn00-unity6-urp-template` | `cn00-bootstrap-scene-and-scripts` |
| `sourceProduct` | Unity Editor `6000.6.7f1 (b22767f04f04)` official template | Authored from scratch in `Client-New` (`Antigravity`) |
| `sourceFile` | `C:/Program Files/Unity/Hub/Editor/6000.6.7f1/Editor/Data/Resources/PackageManager/ProjectTemplates/com.unity.template.urp-blank-17.2.6.tgz` | `UnityProject/Assets/_JXClient/Scenes/Bootstrap.unity` & `UnityProject/Assets/_JXClient/**/*.cs` |
| `sha256` | `FBFA9D0F4D7F5579459D6B1BAF1AD952B091ECBCDB17BA393E07D4994941D570` (28,718,322 bytes) | Scene SHA256: `0FB546975B1DAC584365DECBEE85F84610D7BB7A3E496511B2B6BCA4BD21EEA4` |
| `sourceUnityVersion` | `6000.6.7f1` | `6000.6.7f1` |
| `extractionToolVersion` | None (`Unity.exe -createProject -cloneFromTemplate`) | None (generated via `JXClient.Editor.Cn00ProjectSetup.GenerateBootstrapScene` using built-in Unity primitives `Plane`, `Cube`, `Light`, `Camera`) |
| `licensingBasis` | Official Unity Editor / URP template under active Unity Personal license (`UnityEntitlementLicense.xml`) | First-party project code & synthetic primitive scene authored in `Client-New` |
| `permittedUse` | Project development, testing, and build | Project development, testing, modification, and redistribution |
| `derivativePath` | `UnityProject/Assets/Settings/*`, `Assets/Scenes/*`, `Assets/TutorialInfo/*`, `Assets/Readme.asset`, `Assets/InputSystem_Actions.inputactions`, `UnityProject/Packages/*`, `UnityProject/ProjectSettings/*` | `UnityProject/Assets/_JXClient/**` |
| `conversions` | None | None |
| `dependencies` | `com.unity.render-pipelines.universal@17.6.0`, `com.unity.inputsystem@1.20.1`, `com.unity.ugui@2.6.0` | `JXClient.Domain`, `JXClient.Core` |
| `reviewStatus` | APPROVED | APPROVED |
| `reviewer` | Antigravity & Codex (ChatGPT), 2026-10-25 | Antigravity & Codex (ChatGPT), 2026-10-25 |
| `integrationTests` | `Cn00BootstrapTests.BootstrapScene_ExistsAndInitializesOfflineReadyState` | `Cn00BootstrapTests` (`3/3` EditMode PASS) + `Builds/Windows/JXClientNew.exe` smoke test |

## CN-01 synthetic offline test assets — 2026-10-25

| assetId | Source / author | Output | Processing / rights basis | Evidence |
|---|---|---|---|---|
| cn01-movement-code | First-party authored in Client-New by Codex | Assets/_JXClient/Movement/*.cs; Editor/Cn01ProjectSetup.cs; Tests/PlayMode/* | New source; no binary extraction or legacy client code | reports/evidence/cn01/verification.json source hashes; PlayMode 9/9 |
| cn01-scene-prefab | Unity built-in Plane/Capsule/Cube, generated through Editor APIs | Scenes/MovementTest.unity; Prefabs/OfflinePlayer.prefab | Synthetic test scene, not original Ba Lang Huyen; Unity primitive meshes | scene/prefab hashes in verification.json; actual runtime.png |
| cn01-materials | New color values on official installed URP/Lit shader | Movement/Materials/{Ground,Wall,Avatar}.mat | No imported third-party textures/models; same installed Unity/URP dependency as CN00 | verification.json; shader warnings documented in acceptance report |

Reviewer: Antigravity (independently verified and approved 2026-10-25 in `reports/CN-01_REVIEW.md`). No new legacy asset permissions inferred.

## CN-02 Ba Lang Huyen synthetic scene, environment & read-only reference inventory — 2026-10-25

### A. First-party synthetic scene & environment assets committed in `UnityProject/Assets/_JXClient/`

| assetId | Source / author | Output | Processing / rights basis | Evidence |
|---|---|---|---|---|
| `cn02-world-code` | First-party authored in `Client-New` by Antigravity | `Assets/_JXClient/World/{JXClient.World.asmdef,MapSceneCatalog.cs,MapSceneLoader.cs,BaLangHuyenSceneController.cs}`; `Editor/Cn02ProjectSetup.cs`; `Tests/PlayMode/Cn02WorldSceneTests.cs` | Original C# source; no legacy binary extraction | `JXClient.World.dll` SHA256 `8C0CFCD15691A925BC07D8848BEE89CC85C7B2295C4787BD7CFCBDA14266B5B1`; PlayMode `13/13 PASS` |
| `cn02-balanghuyen-synthetic-scene` | Unity 6 (`6000.6.7f1`) built-in primitives & Editor APIs (`Cn02ProjectSetup.Generate`) | `Assets/_JXClient/Scenes/BaLangHuyen.unity` (SHA256 `1DF531129F8C502B54093DB5AD6C9939A97C187F22ACF4D939DDE34A04FE5A7A`, `106,053` bytes) | **SYNTHETIC FALLBACK SCENE** (`cn02-synthetic-v1`). Explicitly labeled as synthetic technology test scene, **NOT** an exact restoration of original Ba Lang Huyen | `reports/evidence/cn02/view1..4*.png`; `level0` SHA256 `209F368FF034E7E4152396FD702590D1E6F312FBD8C2D2DDADE54D4486CBC053` |
| `cn02-environment-prefabs-materials` | Unity built-in primitive meshes + `Universal Render Pipeline/Lit` materials generated via Editor APIs | `Assets/_JXClient/Environment/Prefabs/{BLH_SyntheticGateArchway,BLH_SyntheticVillageHouse}.prefab`; `Assets/_JXClient/Environment/Materials/BLH_*.mat` (7 materials) | First-party synthetic assets under active Unity Personal license; APPROVED for development/redistribution | 0 missing `.meta`; verified in PlayMode and Win64 standalone build |

### B. Read-only inventory of reference 4016 local files (NOT imported into `Client-New`)

| Reference file path | Size (bytes) | SHA256 | Source Unity Version | Rights / License Status | Action taken |
|---|---|---|---|---|---|
| `E:/Games/VLTK3D/SwordOnline/Client-3D-2/update/Game_Data/StreamingAssets/aa/catalog.bin` | `627,474` | `61997894DDD843F1FB521F2BEED3FB7E2006874B085E51EA8214C39A6378BB2D` | `6000.3.9f1` | `UNKNOWN` (third-party reference build) | Read-only hash/size check only; **not** copied or extracted |
| `E:/Games/VLTK3D/SwordOnline/Client-3D-2/update/Game_Data/StreamingAssets/aa/catalog.hash` | `32` | `F44CBEA09AE4D4AD80E9A39C847CDCDCB4A90D2D0914F572C7DAA9A9DF512A03` | `6000.3.9f1` | `UNKNOWN` (third-party reference build) | Read-only hash/size check only; **not** copied or extracted |
| `E:/Games/VLTK3D/SwordOnline/Client-3D-2/update/Game_Data/StreamingAssets/Settings/maps/country/balanghuyen.mpk` | `59,510` | `182111A74384D512AEBA89942078FA08674FA89F73ACC49941D89CBD1E3C9297` | Legacy/4016 map setting | `UNKNOWN` (third-party reference build) | Read-only hash/size check only; **not** copied or extracted |
| `pc_balanghuyen_assets_all_53fcdd524b8c67eafad5c013f036ed4b.bundle` & `pc_balanghuyen_scenes_all_4e0a8e141624756b20ca4ccfa32087fb.bundle` | Not present on local disk | N/A | `6000.3.9f1` remote/update catalog reference | `UNKNOWN` / `BLOCKED` until licensed | Not downloaded, not extracted, not imported; synthetic fallback used per `tasks/CN-01_TO_CN-06.md` |

## Read-only original-named BLH bundle inventory correction — 2026-10-25

Both named bundles are present in Client-3D-2/LAN-LOCAL/Map53Official4016, contrary to the previous checkpoint. Sizes/SHA256/source paths: reports/evidence/cn02-fixes/reference-inventory.json. Rights status is UNKNOWN. They were not imported, extracted, modified, executed, copied into Assets or approved for distribution. Existing synthetic scene provenance is unchanged.
