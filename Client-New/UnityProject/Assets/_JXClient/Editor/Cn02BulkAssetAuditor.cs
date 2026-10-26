using System;
using System.Collections.Generic;
using System.IO;
using System.Security.Cryptography;
using System.Text;
using JXClient.Movement;
using JXClient.World;
using UnityEditor;
using UnityEditor.SceneManagement;
using UnityEngine;
using UnityEngine.Rendering;
using UnityEngine.SceneManagement;

namespace JXClient.Editor
{
    [Serializable]
    public sealed class MaterialAuditRecord
    {
        public string assetPath;
        public string materialName;
        public string shaderNameBefore;
        public string shaderNameAfter;
        public bool shaderSupported;
        public Color baseColorBefore;
        public Color colorPropertyBefore;
        public Color baseColorAfter;
        public Color colorPropertyAfter;
        public float smoothnessBefore;
        public float smoothnessAfter;
        public int renderQueue;
        public int missingTextureCount;
        public bool wasFixed;
        public string fixApplied;
        public string sha256After;
    }

    [Serializable]
    public sealed class PrefabAuditRecord
    {
        public string assetPath;
        public string prefabName;
        public int totalGameObjects;
        public int meshRendererCount;
        public int colliderCount;
        public int missingScriptCount;
        public int nullMeshOrMaterialCount;
        public bool validTransformAndBounds;
        public bool passed;
    }

    [Serializable]
    public sealed class SceneAuditRecord
    {
        public string scenePath;
        public string sceneName;
        public bool enabledInBuildSettings;
        public int rootObjectCount;
        public int totalGameObjects;
        public int unexpectedlyDisabledCount;
        public bool hasAllRequiredRoots;
        public List<string> missingRoots = new List<string>();
        public int meshRendererCount;
        public int validMeshFilterCount;
        public int nullMaterialSlotCount;
        public int groundColliderCount;
        public int obstacleColliderCount;
        public int wrongLayerColliderCount;
        public int abnormalTransformOrBoundsCount;
        public int missingScriptCount;
        public int connectedPrefabInstanceCount;
        public int brokenPrefabInstanceCount;
        public bool hasMainCamera;
        public float cameraNearClip;
        public float cameraFarClip;
        public bool cameraCanSeeGroundAndSpawn;
        public bool hasDirectionalLight;
        public float directionalLightIntensity;
        public int fixesAppliedCount;
        public List<string> fixNotes = new List<string>();
        public bool passed;
    }

    [Serializable]
    public sealed class Bulk01AuditReport
    {
        public string schema = "cn02-bulk01-asset-audit-v1";
        public string timestampUtc;
        public string unityVersion;
        public int totalAssetsScanned;
        public int missingMetaCount;
        public int duplicateGuidCount;
        public List<string> missingMetaFiles = new List<string>();
        public List<string> duplicateGuids = new List<string>();
        public List<MaterialAuditRecord> materials = new List<MaterialAuditRecord>();
        public List<PrefabAuditRecord> prefabs = new List<PrefabAuditRecord>();
        public List<SceneAuditRecord> scenes = new List<SceneAuditRecord>();
        public int totalFixesApplied;
        public int totalUnresolvedErrors;
        public bool allChecksPassed;
    }

    public static class Cn02BulkAssetAuditor
    {
        private const string BaseAssetDir = "Assets/_JXClient";
        private const string ExpectedUrpShader = "Universal Render Pipeline/Lit";

        [MenuItem("JXClient/CN-02/BULK-01 Audit And Fix Assets")]
        public static void RunMenuAuditAndFix()
        {
            Bulk01AuditReport report = ExecuteAudit(applyDeterministicFixes: true, writeReportFiles: true);
            Debug.Log($"[CN02-BULK01] Completed. passed={report.allChecksPassed} fixes={report.totalFixesApplied} unresolved={report.totalUnresolvedErrors}");
        }

        public static void RunBatchAuditAndFix()
        {
            Bulk01AuditReport report = ExecuteAudit(applyDeterministicFixes: true, writeReportFiles: true);
            Debug.Log($"[CN02-BULK01] Batch Completed. passed={report.allChecksPassed} fixes={report.totalFixesApplied} unresolved={report.totalUnresolvedErrors}");
            if (!report.allChecksPassed)
            {
                EditorApplication.Exit(1);
                throw new InvalidOperationException($"BULK-01 Asset Audit failed with {report.totalUnresolvedErrors} unresolved errors.");
            }
        }

        public static Bulk01AuditReport ExecuteAudit(bool applyDeterministicFixes, bool writeReportFiles)
        {
            var report = new Bulk01AuditReport
            {
                timestampUtc = DateTime.UtcNow.ToString("o"),
                unityVersion = Application.unityVersion
            };

            string previousScenePath = SceneManager.GetActiveScene().path;

            AuditGuidsAndMetaFiles(report);
            AuditMaterials(report, applyDeterministicFixes);
            AuditPrefabs(report, applyDeterministicFixes);
            AuditScenes(report, applyDeterministicFixes);

            if (applyDeterministicFixes && report.totalFixesApplied > 0)
            {
                AssetDatabase.SaveAssets();
                AssetDatabase.Refresh();
            }

            if (!string.IsNullOrEmpty(previousScenePath) && File.Exists(previousScenePath))
            {
                EditorSceneManager.OpenScene(previousScenePath, OpenSceneMode.Single);
            }

            report.allChecksPassed =
                report.missingMetaCount == 0 &&
                report.duplicateGuidCount == 0 &&
                report.totalUnresolvedErrors == 0 &&
                report.materials.Count >= 8 &&
                report.prefabs.Count >= 3 &&
                report.scenes.Count >= 3;

            if (writeReportFiles)
            {
                WriteReportArtifacts(report);
            }

            return report;
        }

        private static void AuditGuidsAndMetaFiles(Bulk01AuditReport report)
        {
            string projectRoot = Path.GetFullPath(Path.Combine(Application.dataPath, ".."));
            string jxRoot = Path.Combine(Application.dataPath, "_JXClient");

            foreach (string file in Directory.GetFiles(jxRoot, "*", SearchOption.AllDirectories))
            {
                if (file.EndsWith(".meta", StringComparison.OrdinalIgnoreCase)) continue;
                report.totalAssetsScanned++;
                string metaPath = file + ".meta";
                if (!File.Exists(metaPath))
                {
                    string rel = Path.GetRelativePath(projectRoot, file).Replace('\\', '/');
                    report.missingMetaFiles.Add(rel);
                    report.missingMetaCount++;
                    report.totalUnresolvedErrors++;
                }
            }

            var guidToPaths = new Dictionary<string, List<string>>(StringComparer.OrdinalIgnoreCase);
            foreach (string meta in Directory.GetFiles(Application.dataPath, "*.meta", SearchOption.AllDirectories))
            {
                string guid = ExtractGuidFromMeta(meta);
                if (string.IsNullOrEmpty(guid)) continue;
                string rel = Path.GetRelativePath(projectRoot, meta).Replace('\\', '/');
                if (!guidToPaths.TryGetValue(guid, out List<string> list))
                {
                    list = new List<string>();
                    guidToPaths[guid] = list;
                }
                list.Add(rel);
            }

            foreach (KeyValuePair<string, List<string>> kvp in guidToPaths)
            {
                if (kvp.Value.Count > 1)
                {
                    report.duplicateGuids.Add($"{kvp.Key}: {string.Join(", ", kvp.Value)}");
                    report.duplicateGuidCount++;
                    report.totalUnresolvedErrors++;
                }
            }
        }

        private static void AuditMaterials(Bulk01AuditReport report, bool applyFixes)
        {
            string[] guids = AssetDatabase.FindAssets("t:Material", new[] { BaseAssetDir });
            Array.Sort(guids, StringComparer.Ordinal);

            Shader urpLit = Shader.Find(ExpectedUrpShader);

            foreach (string guid in guids)
            {
                string path = AssetDatabase.GUIDToAssetPath(guid);
                Material mat = AssetDatabase.LoadAssetAtPath<Material>(path);
                if (mat == null)
                {
                    report.totalUnresolvedErrors++;
                    continue;
                }

                var rec = new MaterialAuditRecord
                {
                    assetPath = path,
                    materialName = mat.name,
                    shaderNameBefore = mat.shader != null ? mat.shader.name : "<null>",
                    baseColorBefore = mat.HasProperty("_BaseColor") ? mat.GetColor("_BaseColor") : Color.clear,
                    colorPropertyBefore = mat.HasProperty("_Color") ? mat.GetColor("_Color") : Color.clear,
                    smoothnessBefore = mat.HasProperty("_Smoothness") ? mat.GetFloat("_Smoothness") : -1f
                };

                var fixes = new List<string>();

                if (mat.shader == null || !mat.shader.isSupported || mat.shader.name == "Hidden/InternalErrorShader" || mat.shader.name != ExpectedUrpShader)
                {
                    if (applyFixes && urpLit != null)
                    {
                        mat.shader = urpLit;
                        fixes.Add($"Assigned {ExpectedUrpShader}");
                    }
                    else
                    {
                        report.totalUnresolvedErrors++;
                    }
                }

                if (mat.HasProperty("_BaseColor") && mat.HasProperty("_Color"))
                {
                    Color baseCol = mat.GetColor("_BaseColor");
                    Color legacyCol = mat.GetColor("_Color");
                    if (Vector4.Distance(baseCol, legacyCol) > 0.002f || Mathf.Abs(baseCol.a - 1f) > 0.001f)
                    {
                        if (applyFixes)
                        {
                            baseCol.a = 1f;
                            mat.SetColor("_BaseColor", baseCol);
                            mat.SetColor("_Color", baseCol);
                            mat.color = baseCol;
                            fixes.Add("Synced _Color with _BaseColor (alpha=1)");
                        }
                        else
                        {
                            report.totalUnresolvedErrors++;
                        }
                    }
                }

                if (mat.HasProperty("_Smoothness"))
                {
                    float sm = mat.GetFloat("_Smoothness");
                    if (!float.IsFinite(sm) || sm < 0f || sm > 1f)
                    {
                        if (applyFixes)
                        {
                            mat.SetFloat("_Smoothness", Mathf.Clamp(sm, 0.1f, 0.5f));
                            fixes.Add("Clamped _Smoothness to valid range");
                        }
                        else
                        {
                            report.totalUnresolvedErrors++;
                        }
                    }
                }

                int missingTex = 0;
                if (mat.shader != null)
                {
                    int propCount = ShaderUtil.GetPropertyCount(mat.shader);
                    var so = new SerializedObject(mat);
                    SerializedProperty texEnvs = so.FindProperty("m_SavedProperties.m_TexEnvs");
                    if (texEnvs != null && texEnvs.isArray)
                    {
                        for (int i = 0; i < texEnvs.arraySize; i++)
                        {
                            SerializedProperty elem = texEnvs.GetArrayElementAtIndex(i);
                            SerializedProperty texProp = elem.FindPropertyRelative("second.m_Texture");
                            if (texProp != null && texProp.objectReferenceValue == null && texProp.objectReferenceEntityIdValue != EntityId.None)
                            {
                                missingTex++;
                            }
                        }
                    }
                    _ = propCount;
                }
                rec.missingTextureCount = missingTex;
                if (missingTex > 0)
                {
                    report.totalUnresolvedErrors += missingTex;
                }

                if (fixes.Count > 0)
                {
                    EditorUtility.SetDirty(mat);
                    AssetDatabase.SaveAssetIfDirty(mat);
                    rec.wasFixed = true;
                    rec.fixApplied = string.Join("; ", fixes);
                    report.totalFixesApplied += fixes.Count;
                }
                else
                {
                    rec.fixApplied = "Verified clean (URP/Lit _BaseColor and _Color synced)";
                }

                rec.shaderNameAfter = mat.shader != null ? mat.shader.name : "<null>";
                rec.shaderSupported = mat.shader != null && mat.shader.isSupported && mat.shader.name == ExpectedUrpShader;
                rec.baseColorAfter = mat.HasProperty("_BaseColor") ? mat.GetColor("_BaseColor") : Color.clear;
                rec.colorPropertyAfter = mat.HasProperty("_Color") ? mat.GetColor("_Color") : Color.clear;
                rec.smoothnessAfter = mat.HasProperty("_Smoothness") ? mat.GetFloat("_Smoothness") : -1f;
                rec.renderQueue = mat.renderQueue;
                rec.sha256After = ComputeFileSha256(Path.GetFullPath(path));

                if (!rec.shaderSupported)
                {
                    report.totalUnresolvedErrors++;
                }

                report.materials.Add(rec);
            }
        }

        private static void AuditPrefabs(Bulk01AuditReport report, bool applyFixes)
        {
            string[] guids = AssetDatabase.FindAssets("t:Prefab", new[] { BaseAssetDir });
            Array.Sort(guids, StringComparer.Ordinal);

            foreach (string guid in guids)
            {
                string path = AssetDatabase.GUIDToAssetPath(guid);
                GameObject prefab = AssetDatabase.LoadAssetAtPath<GameObject>(path);
                if (prefab == null)
                {
                    report.totalUnresolvedErrors++;
                    continue;
                }

                var rec = new PrefabAuditRecord
                {
                    assetPath = path,
                    prefabName = prefab.name,
                    validTransformAndBounds = true
                };

                foreach (Transform tr in prefab.GetComponentsInChildren<Transform>(true))
                {
                    rec.totalGameObjects++;
                    rec.missingScriptCount += GameObjectUtility.GetMonoBehavioursWithMissingScriptCount(tr.gameObject);
                    if (!IsTransformValid(tr))
                    {
                        rec.validTransformAndBounds = false;
                    }
                }

                foreach (MeshRenderer mr in prefab.GetComponentsInChildren<MeshRenderer>(true))
                {
                    rec.meshRendererCount++;
                    MeshFilter mf = mr.GetComponent<MeshFilter>();
                    if (mf == null || mf.sharedMesh == null || mr.sharedMaterial == null ||
                        mr.sharedMaterial.shader == null || mr.sharedMaterial.shader.name == "Hidden/InternalErrorShader")
                    {
                        rec.nullMeshOrMaterialCount++;
                    }
                }

                foreach (Collider col in prefab.GetComponentsInChildren<Collider>(true))
                {
                    rec.colliderCount++;
                    if (!col.enabled)
                    {
                        if (applyFixes)
                        {
                            col.enabled = true;
                            EditorUtility.SetDirty(prefab);
                            report.totalFixesApplied++;
                        }
                        else
                        {
                            rec.validTransformAndBounds = false;
                        }
                    }
                }

                rec.passed = rec.missingScriptCount == 0 && rec.nullMeshOrMaterialCount == 0 && rec.validTransformAndBounds;
                if (!rec.passed)
                {
                    report.totalUnresolvedErrors++;
                }
                report.prefabs.Add(rec);
            }
        }

        private static void AuditScenes(Bulk01AuditReport report, bool applyFixes)
        {
            string[] scenePaths =
            {
                Cn02ProjectSetup.ScenePath,
                Cn01ProjectSetup.ScenePath,
                Cn00ProjectSetup.BootstrapScenePath
            };

            foreach (string scenePath in scenePaths)
            {
                if (!File.Exists(scenePath))
                {
                    report.totalUnresolvedErrors++;
                    continue;
                }

                Scene scene = EditorSceneManager.OpenScene(scenePath, OpenSceneMode.Single);
                var rec = new SceneAuditRecord
                {
                    scenePath = scenePath,
                    sceneName = scene.name,
                    enabledInBuildSettings = Array.Exists(EditorBuildSettings.scenes, s => s.enabled && s.path == scenePath)
                };

                GameObject[] roots = scene.GetRootGameObjects();
                rec.rootObjectCount = roots.Length;

                var rootByName = new Dictionary<string, GameObject>(StringComparer.Ordinal);
                foreach (GameObject r in roots)
                {
                    if (!rootByName.ContainsKey(r.name))
                    {
                        rootByName[r.name] = r;
                    }
                }

                if (scene.name == "BaLangHuyen")
                {
                    foreach (string req in WorldSceneValidationReport.RequiredRootNames)
                    {
                        if (!rootByName.ContainsKey(req))
                        {
                            rec.missingRoots.Add(req);
                        }
                    }
                    rec.hasAllRequiredRoots = rec.missingRoots.Count == 0;
                }
                else
                {
                    rec.hasAllRequiredRoots = true;
                }

                foreach (GameObject root in roots)
                {
                    if (PrefabUtility.IsPartOfPrefabInstance(root))
                    {
                        CheckPrefabInstance(root, rec);
                    }

                    foreach (Transform tr in root.GetComponentsInChildren<Transform>(true))
                    {
                        rec.totalGameObjects++;
                        GameObject go = tr.gameObject;

                        if (!go.activeInHierarchy)
                        {
                            rec.unexpectedlyDisabledCount++;
                        }

                        rec.missingScriptCount += GameObjectUtility.GetMonoBehavioursWithMissingScriptCount(go);

                        if (go != root && PrefabUtility.IsOutermostPrefabInstanceRoot(go))
                        {
                            CheckPrefabInstance(go, rec);
                        }

                        if (!IsTransformValid(tr))
                        {
                            rec.abnormalTransformOrBoundsCount++;
                        }

                        MeshRenderer mr = go.GetComponent<MeshRenderer>();
                        if (mr != null)
                        {
                            rec.meshRendererCount++;
                            MeshFilter mf = go.GetComponent<MeshFilter>();
                            if (mf != null && mf.sharedMesh != null)
                            {
                                rec.validMeshFilterCount++;
                            }
                            else
                            {
                                rec.nullMaterialSlotCount++;
                            }

                            if (!mr.enabled || mr.sharedMaterial == null || mr.sharedMaterial.shader == null ||
                                mr.sharedMaterial.shader.name == "Hidden/InternalErrorShader")
                            {
                                rec.nullMaterialSlotCount++;
                            }

                            if (mr.bounds.size.sqrMagnitude <= 0.0001f)
                            {
                                rec.abnormalTransformOrBoundsCount++;
                            }
                        }

                        Collider col = go.GetComponent<Collider>();
                        if (col != null)
                        {
                            bool isUnderGroundRoot = IsDescendantOfRoot(tr, "Ground");
                            if (scene.name == "BaLangHuyen")
                            {
                                if (isUnderGroundRoot)
                                {
                                    if (go.layer != 8)
                                    {
                                        if (applyFixes)
                                        {
                                            go.layer = 8;
                                            EditorSceneManager.MarkSceneDirty(scene);
                                            rec.fixesAppliedCount++;
                                            rec.fixNotes.Add($"Set Ground layer (8) on {go.name}");
                                            report.totalFixesApplied++;
                                        }
                                        else
                                        {
                                            rec.wrongLayerColliderCount++;
                                        }
                                    }
                                    if (go.layer == 8 && col.enabled && !col.isTrigger)
                                    {
                                        rec.groundColliderCount++;
                                    }
                                }
                                else
                                {
                                    if (go.layer == 8)
                                    {
                                        if (applyFixes)
                                        {
                                            go.layer = 0;
                                            EditorSceneManager.MarkSceneDirty(scene);
                                            rec.fixesAppliedCount++;
                                            rec.fixNotes.Add($"Reset non-ground collider {go.name} from layer 8 to Default (0)");
                                            report.totalFixesApplied++;
                                        }
                                        else
                                        {
                                            rec.wrongLayerColliderCount++;
                                        }
                                    }
                                    if (col.enabled && !col.isTrigger)
                                    {
                                        rec.obstacleColliderCount++;
                                    }
                                }
                            }
                            else
                            {
                                if (go.layer == 8) rec.groundColliderCount++;
                                else rec.obstacleColliderCount++;
                            }
                        }

                        Camera cam = go.GetComponent<Camera>();
                        if (cam != null && cam.CompareTag("MainCamera"))
                        {
                            rec.hasMainCamera = true;
                            if ((cam.nearClipPlane < 0.05f || cam.farClipPlane < 150f) && applyFixes)
                            {
                                cam.nearClipPlane = Mathf.Clamp(cam.nearClipPlane, 0.1f, 0.5f);
                                cam.farClipPlane = Mathf.Max(cam.farClipPlane, 300f);
                                EditorSceneManager.MarkSceneDirty(scene);
                                rec.fixesAppliedCount++;
                                rec.fixNotes.Add($"Adjusted MainCamera clipping planes near={cam.nearClipPlane} far={cam.farClipPlane}");
                                report.totalFixesApplied++;
                            }
                            rec.cameraNearClip = cam.nearClipPlane;
                            rec.cameraFarClip = cam.farClipPlane;
                        }

                        Light light = go.GetComponent<Light>();
                        if (light != null && light.type == LightType.Directional)
                        {
                            rec.hasDirectionalLight = true;
                            rec.directionalLightIntensity = light.intensity;
                        }
                    }
                }

                if (scene.name == "BaLangHuyen")
                {
                    Physics.SyncTransforms();
                    WorldSceneValidationReport val = WorldSceneValidator.ValidateLoadedScene(scene, MapSceneCatalog.BaLangHuyen);
                    rec.cameraCanSeeGroundAndSpawn = val.IsOfflineSceneReady && val.AllGroundProbesHit && val.HasValidSpawnOnGround;
                }
                else if (scene.name == "MovementTest")
                {
                    Physics.SyncTransforms();
                    rec.cameraCanSeeGroundAndSpawn = rec.hasMainCamera && rec.groundColliderCount >= 1;
                }
                else
                {
                    rec.cameraCanSeeGroundAndSpawn = true;
                }

                if (applyFixes && rec.fixesAppliedCount > 0)
                {
                    EditorSceneManager.SaveScene(scene);
                }

                rec.passed =
                    rec.enabledInBuildSettings &&
                    rec.hasAllRequiredRoots &&
                    rec.unexpectedlyDisabledCount == 0 &&
                    rec.nullMaterialSlotCount == 0 &&
                    rec.wrongLayerColliderCount == 0 &&
                    rec.abnormalTransformOrBoundsCount == 0 &&
                    rec.missingScriptCount == 0 &&
                    rec.brokenPrefabInstanceCount == 0 &&
                    rec.cameraCanSeeGroundAndSpawn &&
                    (scene.name != "BaLangHuyen" || (rec.hasMainCamera && rec.hasDirectionalLight && rec.groundColliderCount >= 4 && rec.obstacleColliderCount >= 6));

                if (!rec.passed)
                {
                    report.totalUnresolvedErrors++;
                }

                report.scenes.Add(rec);
            }
        }

        private static void CheckPrefabInstance(GameObject instanceRoot, SceneAuditRecord rec)
        {
            PrefabInstanceStatus status = PrefabUtility.GetPrefabInstanceStatus(instanceRoot);
            UnityEngine.Object source = PrefabUtility.GetCorrespondingObjectFromSource(instanceRoot);
            if (status == PrefabInstanceStatus.Connected && source != null)
            {
                rec.connectedPrefabInstanceCount++;
            }
            else
            {
                rec.brokenPrefabInstanceCount++;
            }
        }

        private static bool IsDescendantOfRoot(Transform tr, string rootName)
        {
            Transform cur = tr;
            while (cur.parent != null)
            {
                cur = cur.parent;
            }
            return string.Equals(cur.name, rootName, StringComparison.Ordinal);
        }

        private static bool IsTransformValid(Transform tr)
        {
            Vector3 p = tr.localPosition;
            Vector3 s = tr.localScale;
            Quaternion r = tr.localRotation;
            if (!float.IsFinite(p.x) || !float.IsFinite(p.y) || !float.IsFinite(p.z)) return false;
            if (!float.IsFinite(s.x) || !float.IsFinite(s.y) || !float.IsFinite(s.z)) return false;
            if (!float.IsFinite(r.x) || !float.IsFinite(r.y) || !float.IsFinite(r.z) || !float.IsFinite(r.w)) return false;
            if (Mathf.Abs(s.x) < 0.0001f || Mathf.Abs(s.y) < 0.0001f || Mathf.Abs(s.z) < 0.0001f) return false;
            if (Mathf.Abs(s.x) > 10000f || Mathf.Abs(s.y) > 10000f || Mathf.Abs(s.z) > 10000f) return false;
            return true;
        }

        private static string ExtractGuidFromMeta(string metaPath)
        {
            foreach (string line in File.ReadLines(metaPath))
            {
                if (line.StartsWith("guid:", StringComparison.OrdinalIgnoreCase))
                {
                    return line.Substring(5).Trim();
                }
            }
            return string.Empty;
        }

        private static string ComputeFileSha256(string path)
        {
            if (!File.Exists(path)) return string.Empty;
            using SHA256 sha = SHA256.Create();
            byte[] bytes = File.ReadAllBytes(path);
            byte[] hash = sha.ComputeHash(bytes);
            var sb = new StringBuilder(hash.Length * 2);
            foreach (byte b in hash) sb.Append(b.ToString("X2"));
            return sb.ToString();
        }

        private static void WriteReportArtifacts(Bulk01AuditReport report)
        {
            string repoRoot = Path.GetFullPath(Path.Combine(Application.dataPath, "../.."));
            string jsonPath = Path.Combine(repoRoot, "reports/evidence/cn02-bulk02/bulk01-asset-audit.json");
            string mdPath = Path.Combine(repoRoot, "reports/CN02_BULK01_ASSET_AUDIT.md");

            Directory.CreateDirectory(Path.GetDirectoryName(jsonPath));
            Directory.CreateDirectory(Path.GetDirectoryName(mdPath));

            File.WriteAllText(jsonPath, JsonUtility.ToJson(report, true), Encoding.UTF8);

            var md = new StringBuilder();
            md.AppendLine("# Báo cáo Nghiệm thu BULK-01 — Kiểm tra và Sửa hàng loạt Scene, Material, Prefab");
            md.AppendLine();
            md.AppendLine($"- **Thời gian (UTC):** `{report.timestampUtc}`");
            md.AppendLine($"- **Unity Version:** `{report.unityVersion}`");
            md.AppendLine($"- **Tổng số file trong `Assets/_JXClient/**` đã quét:** `{report.totalAssetsScanned}`");
            md.AppendLine($"- **Missing `.meta`:** `{report.missingMetaCount}`");
            md.AppendLine($"- **Duplicate GUID:** `{report.duplicateGuidCount}`");
            md.AppendLine($"- **Tổng số lỗi còn tồn đọng:** `{report.totalUnresolvedErrors}`");
            md.AppendLine($"- **Kết luận BULK-01:** `{(report.allChecksPassed ? "PASS" : "FAIL")}`");
            md.AppendLine();
            md.AppendLine("## 1. Kiểm toán và Hoàn thiện 8 Material URP (Antigravity Lane)");
            md.AppendLine();
            md.AppendLine("| Material | Shader | `_BaseColor` | `_Color` (Synced) | Smoothness | RenderQueue | Missing Tex | Biện pháp xử lý / Trạng thái | SHA256 |");
            md.AppendLine("|---|---|---|---|---|---|---|---|---|");
            foreach (MaterialAuditRecord m in report.materials)
            {
                md.AppendLine($"| `{m.materialName}` | `{m.shaderNameAfter}` | `({m.baseColorAfter.r:F2}, {m.baseColorAfter.g:F2}, {m.baseColorAfter.b:F2}, {m.baseColorAfter.a:F2})` | `({m.colorPropertyAfter.r:F2}, {m.colorPropertyAfter.g:F2}, {m.colorPropertyAfter.b:F2}, {m.colorPropertyAfter.a:F2})` | `{m.smoothnessAfter:F2}` | `{m.renderQueue}` | `{m.missingTextureCount}` | {m.fixApplied} | `{m.sha256After.Substring(0, 12)}...` |");
            }
            md.AppendLine();
            md.AppendLine("## 2. Kiểm toán Prefab (`Assets/_JXClient/**/Prefabs`)");
            md.AppendLine();
            md.AppendLine("| Prefab | Đường dẫn | GameObjects | MeshRenderers | Colliders | Missing Scripts | Null Mesh/Mat | Transform/Bounds | Kết quả |");
            md.AppendLine("|---|---|---|---|---|---|---|---|---|");
            foreach (PrefabAuditRecord p in report.prefabs)
            {
                md.AppendLine($"| `{p.prefabName}` | `{p.assetPath}` | `{p.totalGameObjects}` | `{p.meshRendererCount}` | `{p.colliderCount}` | `{p.missingScriptCount}` | `{p.nullMeshOrMaterialCount}` | `{(p.validTransformAndBounds ? "Valid" : "Invalid")}` | `{(p.passed ? "PASS" : "FAIL")}` |");
            }
            md.AppendLine();
            md.AppendLine("## 3. Kiểm toán Scene Hierarchy, Collider, Layer, Camera & Lighting");
            md.AppendLine();
            md.AppendLine("| Scene | In Build | Roots | Total GOs | Required 7 Roots | Renderers | Ground Colliders (L8) | Obstacle Colliders (L0) | Connected Prefabs | Broken Prefabs | Camera Clipping | Lighting | Kết quả |");
            md.AppendLine("|---|---|---|---|---|---|---|---|---|---|---|---|---|");
            foreach (SceneAuditRecord s in report.scenes)
            {
                md.AppendLine($"| `{s.sceneName}` | `{s.enabledInBuildSettings}` | `{s.rootObjectCount}` | `{s.totalGameObjects}` | `{s.hasAllRequiredRoots}` | `{s.meshRendererCount}` | `{s.groundColliderCount}` | `{s.obstacleColliderCount}` | `{s.connectedPrefabInstanceCount}` | `{s.brokenPrefabInstanceCount}` | `{s.cameraNearClip:F2}..{s.cameraFarClip:F0}` | `{(s.hasDirectionalLight ? $"Dir ({s.directionalLightIntensity:F2})" : "N/A")}` | `{(s.passed ? "PASS" : "FAIL")}` |");
            }

            File.WriteAllText(mdPath, md.ToString(), Encoding.UTF8);
        }
    }
}
