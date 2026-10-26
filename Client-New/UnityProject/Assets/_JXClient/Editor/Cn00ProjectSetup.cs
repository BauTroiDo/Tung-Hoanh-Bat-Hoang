using System;
using System.IO;
using System.Linq;
using JXClient.Core;
using UnityEditor;
using UnityEditor.Build.Reporting;
using UnityEditor.SceneManagement;
using UnityEngine;
using UnityEngine.SceneManagement;

namespace JXClient.Editor
{
    public static class Cn00ProjectSetup
    {
        public const string BootstrapScenePath = "Assets/_JXClient/Scenes/Bootstrap.unity";

        [MenuItem("JXClient/CN-00/Generate Bootstrap Scene")]
        public static void GenerateBootstrapScene()
        {
            string sceneDir = Path.GetDirectoryName(BootstrapScenePath);
            if (!string.IsNullOrEmpty(sceneDir) && !Directory.Exists(sceneDir))
            {
                Directory.CreateDirectory(sceneDir);
            }

            Scene scene = EditorSceneManager.NewScene(NewSceneSetup.EmptyScene, NewSceneMode.Single);

            // 1. Directional Light
            GameObject lightGo = new GameObject("Directional Light");
            Light light = lightGo.AddComponent<Light>();
            light.type = LightType.Directional;
            light.color = new Color(1.0f, 0.96f, 0.88f, 1.0f);
            light.intensity = 1.2f;
            lightGo.transform.rotation = Quaternion.Euler(50f, -30f, 0f);

            // 2. Test Ground Plane with BoxCollider/MeshCollider
            GameObject groundGo = GameObject.CreatePrimitive(PrimitiveType.Plane);
            groundGo.name = "BootstrapTestGround";
            groundGo.transform.position = Vector3.zero;
            groundGo.transform.localScale = new Vector3(4f, 1f, 4f);

            // 3. Visual reference marker on the test ground
            GameObject markerGo = GameObject.CreatePrimitive(PrimitiveType.Cube);
            markerGo.name = "OriginReferenceCube";
            markerGo.transform.position = new Vector3(0f, 0.5f, 0f);
            markerGo.transform.localScale = new Vector3(1f, 1f, 1f);

            // 4. Main Camera
            GameObject cameraGo = new GameObject("Main Camera");
            cameraGo.tag = "MainCamera";
            Camera cam = cameraGo.AddComponent<Camera>();
            cam.clearFlags = CameraClearFlags.SolidColor;
            cam.backgroundColor = new Color(0.11f, 0.14f, 0.18f, 1.0f);
            cameraGo.AddComponent<AudioListener>();
            cameraGo.transform.position = new Vector3(0f, 8f, -12f);
            cameraGo.transform.LookAt(new Vector3(0f, 0.5f, 0f));

            // 5. BootstrapRunner
            GameObject runnerGo = new GameObject("BootstrapRunner");
            BootstrapRunner runner = runnerGo.AddComponent<BootstrapRunner>();

            SerializedObject so = new SerializedObject(runner);
            so.FindProperty("bootstrapCamera").objectReferenceValue = cam;
            so.FindProperty("testGroundCollider").objectReferenceValue = groundGo.GetComponent<Collider>();
            so.ApplyModifiedPropertiesWithoutUndo();

            bool saved = EditorSceneManager.SaveScene(scene, BootstrapScenePath);
            if (!saved)
            {
                throw new InvalidOperationException($"Failed to save scene at {BootstrapScenePath}");
            }

            EditorBuildSettings.scenes = new[]
            {
                new EditorBuildSettingsScene(BootstrapScenePath, true)
            };

            AssetDatabase.SaveAssets();
            AssetDatabase.Refresh();
            Debug.Log($"[CN-00][Cn00ProjectSetup] Generated {BootstrapScenePath} and set as sole EditorBuildSettings scene.");
        }

        [MenuItem("JXClient/CN-00/Build Windows x64")]
        public static void BuildWin64()
        {
            if (!File.Exists(BootstrapScenePath))
            {
                GenerateBootstrapScene();
            }

            string projectRoot = Directory.GetCurrentDirectory();
            string clientNewRoot = Directory.GetParent(projectRoot)?.FullName ?? projectRoot;
            string outputDir = Path.Combine(clientNewRoot, "Builds", "Windows");
            Directory.CreateDirectory(outputDir);

            string exePath = Path.Combine(outputDir, "JXClientNew.exe");

            BuildPlayerOptions options = new BuildPlayerOptions
            {
                scenes = new[] { BootstrapScenePath },
                locationPathName = exePath,
                target = BuildTarget.StandaloneWindows64,
                options = BuildOptions.None
            };

            Debug.Log($"[CN-00][Cn00ProjectSetup] Starting StandaloneWindows64 build to: {exePath}");
            BuildReport report = BuildPipeline.BuildPlayer(options);
            BuildSummary summary = report.summary;

            Debug.Log(
                $"[CN-00][Cn00ProjectSetup] Build finished: result={summary.result}, " +
                $"totalErrors={summary.totalErrors}, totalWarnings={summary.totalWarnings}, " +
                $"totalSize={summary.totalSize} bytes, outputPath={summary.outputPath}");

            if (summary.result != BuildResult.Succeeded)
            {
                EditorApplication.Exit(1);
                throw new InvalidOperationException($"Win64 build failed with status: {summary.result}");
            }
        }
    }
}
