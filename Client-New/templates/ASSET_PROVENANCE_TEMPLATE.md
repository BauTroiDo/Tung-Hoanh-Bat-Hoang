# Asset provenance manifest — Template

**No asset can be committed or distributed based only on having been extracted.** Maintain one record for each imported asset.

| Field | Required value |
|---|---|
| assetId | Globally unique logical ID |
| sourceProduct | Client/author/source |
| sourceFile | Original path and file name |
| sha256 | Source bytes digest |
| sourceUnityVersion | Runtime build version, if applicable |
| extractionToolVersion | Tool and version or manual creation |
| licensingBasis | Owned / licensed / public-domain / permission evidence |
| permittedUse | private test / modification / redistribution |
| derivativePath | UnityProject output path |
| conversions | glTF/FBX, shader, texture/rig changes |
| dependencies | IDs for materials, mesh, textures, scene |
| reviewStatus | APPROVED / BLOCKED / UNKNOWN |
| reviewer | Name/agent with approval date |
| integrationTests | Scene render/collider test references |

Example record must use **synthetic self-made asset** for illustration; no real client third-party asset should be silently marked APPROVED.

If licenseStatus UNKNOWN, keep extracted output outside Git until cleared; if cannot clear rights, recreate scene from independently authored assets.
