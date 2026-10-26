using System;
using System.IO;
using JXClient.Movement;
using JXClient.World;
using UnityEditor;
using UnityEditor.Build.Reporting;
using UnityEditor.SceneManagement;
using UnityEngine;
using UnityEngine.EventSystems;
using UnityEngine.InputSystem.UI;
using UnityEngine.SceneManagement;
using UnityEngine.UI;

namespace JXClient.Editor
{
    public static class Cn02ProjectSetup
    {
        private const string Base = "Assets/_JXClient";
        public const string ScenePath = Base + "/Scenes/BaLangHuyen.unity";
        private const string EnvDir = Base + "/Environment";
        private const string MatDir = EnvDir + "/Materials";
        private const string PrefabDir = EnvDir + "/Prefabs";

        private static void EnsureFolder(string path)
        {
            if (AssetDatabase.IsValidFolder(path)) return;
            int idx = path.LastIndexOf('/');
            string parent = path.Substring(0, idx);
            string child = path.Substring(idx + 1);
            EnsureFolder(parent);
            AssetDatabase.CreateFolder(parent, child);
        }

        private static Material CreateOrUpdateMaterial(string name, Color color, float smoothness = 0.2f)
        {
            string path = $"{MatDir}/{name}.mat";
            Material mat = AssetDatabase.LoadAssetAtPath<Material>(path);
            if (mat == null)
            {
                Shader shader = Shader.Find("Universal Render Pipeline/Lit") ?? Shader.Find("Standard");
                mat = new Material(shader);
                AssetDatabase.CreateAsset(mat, path);
            }
            mat.color = color;
            if (mat.HasProperty("_BaseColor")) mat.SetColor("_BaseColor", color);
            if (mat.HasProperty("_Smoothness")) mat.SetFloat("_Smoothness", smoothness);
            EditorUtility.SetDirty(mat);
            return mat;
        }

        private static GameObject CreateGateArchwayPrefab(Material woodMat, Material roofMat)
        {
            string prefabPath = $"{PrefabDir}/BLH_SyntheticGateArchway.prefab";
            GameObject root = new GameObject("BLH_SyntheticGateArchway");

            GameObject leftPillar = GameObject.CreatePrimitive(PrimitiveType.Cube);
            leftPillar.name = "LeftPillar";
            leftPillar.transform.SetParent(root.transform, false);
            leftPillar.transform.localPosition = new Vector3(-3.2f, 2.2f, 0f);
            leftPillar.transform.localScale = new Vector3(0.8f, 4.4f, 0.8f);
            leftPillar.GetComponent<Renderer>().sharedMaterial = woodMat;

            GameObject rightPillar = GameObject.CreatePrimitive(PrimitiveType.Cube);
            rightPillar.name = "RightPillar";
            rightPillar.transform.SetParent(root.transform, false);
            rightPillar.transform.localPosition = new Vector3(3.2f, 2.2f, 0f);
            rightPillar.transform.localScale = new Vector3(0.8f, 4.4f, 0.8f);
            rightPillar.GetComponent<Renderer>().sharedMaterial = woodMat;

            GameObject topBeam = GameObject.CreatePrimitive(PrimitiveType.Cube);
            topBeam.name = "TopBeam";
            topBeam.transform.SetParent(root.transform, false);
            topBeam.transform.localPosition = new Vector3(0f, 4.6f, 0f);
            topBeam.transform.localScale = new Vector3(8.4f, 0.9f, 1.4f);
            topBeam.GetComponent<Renderer>().sharedMaterial = roofMat;

            GameObject saved = PrefabUtility.SaveAsPrefabAsset(root, prefabPath);
            UnityEngine.Object.DestroyImmediate(root);
            return saved;
        }

        private static GameObject CreateVillageHousePrefab(Material wallMat, Material roofMat)
        {
            string prefabPath = $"{PrefabDir}/BLH_SyntheticVillageHouse.prefab";
            GameObject root = new GameObject("BLH_SyntheticVillageHouse");

            GameObject body = GameObject.CreatePrimitive(PrimitiveType.Cube);
            body.name = "HouseBody";
            body.transform.SetParent(root.transform, false);
            body.transform.localPosition = new Vector3(0f, 1.6f, 0f);
            body.transform.localScale = new Vector3(4.5f, 3.2f, 3.6f);
            body.GetComponent<Renderer>().sharedMaterial = wallMat;

            GameObject roof = GameObject.CreatePrimitive(PrimitiveType.Cube);
            roof.name = "HouseRoof";
            roof.transform.SetParent(root.transform, false);
            roof.transform.localPosition = new Vector3(0f, 3.6f, 0f);
            roof.transform.localScale = new Vector3(5.3f, 0.8f, 4.4f);
            roof.GetComponent<Renderer>().sharedMaterial = roofMat;

            GameObject saved = PrefabUtility.SaveAsPrefabAsset(root, prefabPath);
            UnityEngine.Object.DestroyImmediate(root);
            return saved;
        }

        [MenuItem("JXClient/CN-02/Generate BaLangHuyen Scene")]
        public static void Generate()
        {
            EnsureFolder(Base + "/Scenes");
            EnsureFolder(EnvDir);
            EnsureFolder(MatDir);
            EnsureFolder(PrefabDir);

            // Ensure Layer 8 is Ground
            SerializedObject tags = new SerializedObject(AssetDatabase.LoadAllAssetsAtPath("ProjectSettings/TagManager.asset")[0]);
            SerializedProperty layer8 = tags.FindProperty("layers").GetArrayElementAtIndex(8);
            if (string.IsNullOrEmpty(layer8.stringValue))
            {
                layer8.stringValue = "Ground";
                tags.ApplyModifiedProperties();
            }

            Material grassMat = CreateOrUpdateMaterial("BLH_GrassTerrain", new Color(0.22f, 0.38f, 0.24f), 0.15f);
            Material roadMat = CreateOrUpdateMaterial("BLH_StoneRoad", new Color(0.46f, 0.44f, 0.39f), 0.25f);
            Material plazaMat = CreateOrUpdateMaterial("BLH_PlazaTile", new Color(0.54f, 0.50f, 0.43f), 0.3f);
            Material wallMat = CreateOrUpdateMaterial("BLH_HouseWall", new Color(0.68f, 0.63f, 0.54f), 0.2f);
            Material roofMat = CreateOrUpdateMaterial("BLH_TileRoof", new Color(0.28f, 0.32f, 0.38f), 0.35f);
            Material woodMat = CreateOrUpdateMaterial("BLH_WoodTimber", new Color(0.42f, 0.27f, 0.16f), 0.2f);
            Material rockMat = CreateOrUpdateMaterial("BLH_BoundaryRock", new Color(0.36f, 0.38f, 0.40f), 0.15f);

            GameObject gatePrefab = CreateGateArchwayPrefab(woodMat, roofMat);
            GameObject housePrefab = CreateVillageHousePrefab(wallMat, roofMat);

            Scene scene = EditorSceneManager.NewScene(NewSceneSetup.EmptyScene, NewSceneMode.Single);

            // Required Root 1: Ground
            GameObject groundRoot = new GameObject("Ground");
            GameObject mainMeadow = GameObject.CreatePrimitive(PrimitiveType.Plane);
            mainMeadow.name = "BLH_MainMeadowGround";
            mainMeadow.layer = 8;
            mainMeadow.transform.SetParent(groundRoot.transform, false);
            mainMeadow.transform.localPosition = Vector3.zero;
            mainMeadow.transform.localScale = new Vector3(6f, 1f, 6f); // 60m x 60m walkable ground
            mainMeadow.GetComponent<Renderer>().sharedMaterial = grassMat;

            // Walkable road strips slightly elevated (0.02m) on Ground layer
            GameObject nsRoad = GameObject.CreatePrimitive(PrimitiveType.Cube);
            nsRoad.name = "BLH_NorthSouthRoad";
            nsRoad.layer = 8;
            nsRoad.transform.SetParent(groundRoot.transform, false);
            nsRoad.transform.localPosition = new Vector3(0f, 0.01f, 0f);
            nsRoad.transform.localScale = new Vector3(4.5f, 0.02f, 48f);
            nsRoad.GetComponent<Renderer>().sharedMaterial = roadMat;

            GameObject ewRoad = GameObject.CreatePrimitive(PrimitiveType.Cube);
            ewRoad.name = "BLH_EastWestRoad";
            ewRoad.layer = 8;
            ewRoad.transform.SetParent(groundRoot.transform, false);
            ewRoad.transform.localPosition = new Vector3(0f, 0.015f, 0f);
            ewRoad.transform.localScale = new Vector3(48f, 0.02f, 4.5f);
            ewRoad.GetComponent<Renderer>().sharedMaterial = roadMat;

            GameObject centerPlaza = GameObject.CreatePrimitive(PrimitiveType.Cube);
            centerPlaza.name = "BLH_CenterPlazaGround";
            centerPlaza.layer = 8;
            centerPlaza.transform.SetParent(groundRoot.transform, false);
            centerPlaza.transform.localPosition = new Vector3(0f, 0.02f, 0f);
            centerPlaza.transform.localScale = new Vector3(12f, 0.03f, 12f);
            centerPlaza.GetComponent<Renderer>().sharedMaterial = plazaMat;

            // Required Root 2: Environment
            GameObject envRoot = new GameObject("Environment");
            GameObject northGate = (GameObject)PrefabUtility.InstantiatePrefab(gatePrefab, scene);
            northGate.name = "NorthVillageGate";
            northGate.transform.SetParent(envRoot.transform, false);
            northGate.transform.localPosition = new Vector3(0f, 0f, 22f);

            GameObject southGate = (GameObject)PrefabUtility.InstantiatePrefab(gatePrefab, scene);
            southGate.name = "SouthVillageGate";
            southGate.transform.SetParent(envRoot.transform, false);
            southGate.transform.localPosition = new Vector3(0f, 0f, -22f);

            // Required Root 3: StaticProps
            GameObject propsRoot = new GameObject("StaticProps");
            Vector3[] housePositions =
            {
                new Vector3(-11f, 0f, 10f),
                new Vector3(11f, 0f, 10f),
                new Vector3(-11f, 0f, -10f),
                new Vector3(11f, 0f, -10f)
            };
            float[] houseYaws = { 90f, -90f, 90f, -90f };
            for (int i = 0; i < housePositions.Length; i++)
            {
                GameObject house = (GameObject)PrefabUtility.InstantiatePrefab(housePrefab, scene);
                house.name = $"VillageHouse_{i + 1}";
                house.transform.SetParent(propsRoot.transform, false);
                house.transform.localPosition = housePositions[i];
                house.transform.localRotation = Quaternion.Euler(0f, houseYaws[i], 0f);
            }

            // Required Root 4: Colliders (dedicated obstacle & perimeter walls blocking movement)
            GameObject collidersRoot = new GameObject("Colliders");
            CreateObstacleBlock(collidersRoot.transform, "Obstacle_WestWall", new Vector3(-5.5f, 1.1f, 2f), new Vector3(1.2f, 2.2f, 6f), rockMat);
            CreateObstacleBlock(collidersRoot.transform, "Obstacle_EastLanternPillar", new Vector3(4.5f, 1.2f, -3.5f), new Vector3(1.4f, 2.4f, 1.4f), woodMat);
            CreateObstacleBlock(collidersRoot.transform, "Perimeter_NorthWestFence", new Vector3(-16f, 1.0f, 24f), new Vector3(20f, 2.0f, 1.0f), rockMat);
            CreateObstacleBlock(collidersRoot.transform, "Perimeter_NorthEastFence", new Vector3(16f, 1.0f, 24f), new Vector3(20f, 2.0f, 1.0f), rockMat);
            CreateObstacleBlock(collidersRoot.transform, "Perimeter_SouthWestFence", new Vector3(-16f, 1.0f, -24f), new Vector3(20f, 2.0f, 1.0f), rockMat);
            CreateObstacleBlock(collidersRoot.transform, "Perimeter_SouthEastFence", new Vector3(16f, 1.0f, -24f), new Vector3(20f, 2.0f, 1.0f), rockMat);

            // Required Root 5: SpawnPoints
            GameObject spawnRoot = new GameObject("SpawnPoints");
            CreateMarker(spawnRoot.transform, "OfflineSpawn_VillageCenter", new Vector3(0f, 0.05f, -3f));
            CreateMarker(spawnRoot.transform, "AnchorCandidate_CenterPlaza", new Vector3(0f, 0.05f, 0f));
            CreateMarker(spawnRoot.transform, "AnchorCandidate_NorthGate", new Vector3(0f, 0.05f, 18f));
            CreateMarker(spawnRoot.transform, "AnchorCandidate_EastRoad", new Vector3(18f, 0.05f, 0f));
            CreateMarker(spawnRoot.transform, "AnchorCandidate_SouthWestCorner", new Vector3(-14f, 0.05f, -14f));

            // Required Root 6: Lighting
            GameObject lightingRoot = new GameObject("Lighting");
            GameObject sunGo = new GameObject("DirectionalSun");
            sunGo.transform.SetParent(lightingRoot.transform, false);
            Light sun = sunGo.AddComponent<Light>();
            sun.type = LightType.Directional;
            sun.color = new Color(1.0f, 0.95f, 0.86f);
            sun.intensity = 1.45f;
            sunGo.transform.rotation = Quaternion.Euler(48f, -35f, 0f);
            RenderSettings.ambientLight = new Color(0.36f, 0.41f, 0.44f);

            // Required Root 7: Nav
            GameObject navRoot = new GameObject("Nav");
            foreach (GroundProbeDefinition probe in MapSceneCatalog.BaLangHuyen.GroundProbes)
            {
                CreateMarker(navRoot.transform, probe.Name, probe.HorizontalPosition);
            }

            // Instantiate OfflinePlayer from CN01 prefab
            GameObject playerPrefab = AssetDatabase.LoadAssetAtPath<GameObject>(Base + "/Prefabs/OfflinePlayer.prefab");
            if (playerPrefab == null)
            {
                Cn01ProjectSetup.Generate();
                playerPrefab = AssetDatabase.LoadAssetAtPath<GameObject>(Base + "/Prefabs/OfflinePlayer.prefab");
            }

            GameObject player = (GameObject)PrefabUtility.InstantiatePrefab(playerPrefab, scene);
            player.name = "OfflinePlayer";
            player.transform.position = new Vector3(0f, 0.04f, -3f);
            OfflineMovement mover = player.GetComponent<OfflineMovement>();
            mover.GroundMask = 1 << 8;

            // Main Camera
            GameObject cameraGo = new GameObject("Main Camera");
            cameraGo.tag = "MainCamera";
            Camera cam = cameraGo.AddComponent<Camera>();
            cam.clearFlags = CameraClearFlags.SolidColor;
            cam.backgroundColor = new Color(0.18f, 0.24f, 0.30f);
            cameraGo.AddComponent<AudioListener>();
            OrbitFollowCamera orbit = cameraGo.AddComponent<OrbitFollowCamera>();
            orbit.Target = player.transform;
            orbit.SetOrbit(0f, 48f, 18f);
            orbit.ApplyPose();
            mover.ViewCamera = cam;

            // UI HUD Shield & Scene Controller
            GameObject canvasGo = new GameObject("BaLangHuyenHud", typeof(Canvas), typeof(CanvasScaler), typeof(GraphicRaycaster));
            canvasGo.GetComponent<Canvas>().renderMode = RenderMode.ScreenSpaceOverlay;
            CanvasScaler scaler = canvasGo.GetComponent<CanvasScaler>();
            scaler.uiScaleMode = CanvasScaler.ScaleMode.ScaleWithScreenSize;
            scaler.referenceResolution = new Vector2(1280f, 720f);
            scaler.matchWidthOrHeight = 0.5f;

            GameObject shieldPanel = new GameObject("InputShield", typeof(RectTransform), typeof(Image));
            shieldPanel.transform.SetParent(canvasGo.transform, false);
            RectTransform rect = shieldPanel.GetComponent<RectTransform>();
            rect.anchorMin = rect.anchorMax = new Vector2(0f, 1f);
            rect.pivot = new Vector2(0f, 1f);
            rect.anchoredPosition = new Vector2(16f, -16f);
            rect.sizeDelta = new Vector2(650f, 185f);
            shieldPanel.GetComponent<Image>().color = new Color(0.03f, 0.05f, 0.08f, 0.94f);

            GameObject textGo = new GameObject("StatusText", typeof(RectTransform), typeof(Text));
            textGo.transform.SetParent(shieldPanel.transform, false);
            RectTransform tr = textGo.GetComponent<RectTransform>();
            tr.anchorMin = Vector2.zero;
            tr.anchorMax = Vector2.one;
            tr.offsetMin = new Vector2(12f, 12f);
            tr.offsetMax = new Vector2(-12f, -12f);
            Text statusText = textGo.GetComponent<Text>();
            statusText.font = Resources.GetBuiltinResource<Font>("LegacyRuntime.ttf");
            statusText.fontSize = 14;
            statusText.color = Color.white;
            statusText.raycastTarget = false;

            GameObject eventsGo = new GameObject("EventSystem", typeof(EventSystem), typeof(InputSystemUIInputModule));
            eventsGo.GetComponent<InputSystemUIInputModule>().AssignDefaultActions();

            BaLangHuyenSceneController controller = canvasGo.AddComponent<BaLangHuyenSceneController>();
            controller.Player = mover;
            controller.OrbitCamera = orbit;
            controller.HudLabel = statusText;
            controller.BoundsCenter = MapSceneCatalog.BaLangHuyen.CameraBounds.center;
            controller.BoundsSize = MapSceneCatalog.BaLangHuyen.CameraBounds.size;

            if (!EditorSceneManager.SaveScene(scene, ScenePath))
            {
                throw new InvalidOperationException($"Failed to save scene at {ScenePath}");
            }

            // Keep BaLangHuyen, MovementTest, and Bootstrap all enabled in EditorBuildSettings
            EditorBuildSettings.scenes = new[]
            {
                new EditorBuildSettingsScene(ScenePath, true),
                new EditorBuildSettingsScene(Cn01ProjectSetup.ScenePath, true),
                new EditorBuildSettingsScene(Cn00ProjectSetup.BootstrapScenePath, true)
            };

            AssetDatabase.SaveAssets();
            AssetDatabase.Refresh();
            Debug.Log("[CN-02] Generated editable synthetic BaLangHuyen.unity scene, environment materials/prefabs, and updated EditorBuildSettings.");
        }

        private static void CreateObstacleBlock(Transform parent, string name, Vector3 pos, Vector3 scale, Material mat)
        {
            GameObject block = GameObject.CreatePrimitive(PrimitiveType.Cube);
            block.name = name;
            block.transform.SetParent(parent, false);
            block.transform.localPosition = pos;
            block.transform.localScale = scale;
            block.GetComponent<Renderer>().sharedMaterial = mat;
        }

        private static void CreateMarker(Transform parent, string name, Vector3 pos)
        {
            GameObject marker = new GameObject(name);
            marker.transform.SetParent(parent, false);
            marker.transform.localPosition = pos;
        }

        [MenuItem("JXClient/CN-02/Build Windows x64 (Builds/CN02-Windows)")]
        public static void Build()
        {
            if (!File.Exists(ScenePath))
            {
                Generate();
            }

            string outputExe = Path.GetFullPath(Path.Combine(Application.dataPath, "../../Builds/CN02-Windows/JXClientNew.exe"));
            Directory.CreateDirectory(Path.GetDirectoryName(outputExe));

            BuildPlayerOptions opts = new BuildPlayerOptions
            {
                scenes = new[]
                {
                    ScenePath,
                    Cn01ProjectSetup.ScenePath,
                    Cn00ProjectSetup.BootstrapScenePath
                },
                target = BuildTarget.StandaloneWindows64,
                locationPathName = outputExe,
                options = BuildOptions.None
            };

            BuildReport report = BuildPipeline.BuildPlayer(opts);
            Debug.Log(
                $"[CN-02] BUILD result={report.summary.result} errors={report.summary.totalErrors} " +
                $"warnings={report.summary.totalWarnings} bytes={report.summary.totalSize} path={outputExe}");

            if (report.summary.result != BuildResult.Succeeded)
            {
                EditorApplication.Exit(1);
                throw new InvalidOperationException($"CN-02 Win64 build failed: {report.summary.result}");
            }
        }
    }
}
