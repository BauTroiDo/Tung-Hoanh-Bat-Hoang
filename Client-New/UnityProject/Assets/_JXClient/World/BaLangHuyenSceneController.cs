using System;
using System.Collections;
using System.Collections.Generic;
using System.IO;
using System.Text;
using JXClient.Domain;
using JXClient.Movement;
using UnityEngine;
using UnityEngine.SceneManagement;
using UnityEngine.UI;

namespace JXClient.World
{
    [Serializable]
    public sealed class Bulk03TestCaseResult
    {
        public string caseId;
        public string description;
        public string startPosition;
        public string targetPosition;
        public string endPosition;
        public string motionStateObserved;
        public string lastInputObserved;
        public int waypointCount;
        public float durationSeconds;
        public float averageFps;
        public bool passed;
        public string notes;
    }

    [Serializable]
    public sealed class Bulk03ProbeEntry
    {
        public string probeName;
        public string worldOrigin;
        public bool hitGround;
        public string hitPoint;
        public string hitNormal;
        public string colliderName;
    }

    [Serializable]
    public sealed class Bulk03RuntimeSummary
    {
        public string schema = "cn02-bulk03-runtime-gameplay-v1";
        public string timestampUtc;
        public string unityVersion;
        public string platform;
        public string graphicsDeviceName;
        public string graphicsDeviceType;
        public bool isBatchMode;
        public string sceneName;
        public string provenanceVersion;
        public bool isSyntheticFallback;
        public string calibrationStatus;
        public bool offlineSceneReady;
        public string sessionState;
        public bool moveRequestNetworkAllowed;
        public float measuredFps;
        public float managedMemoryMB;
        public List<string> capturedScreenshots = new List<string>();
        public List<Bulk03ProbeEntry> groundProbes = new List<Bulk03ProbeEntry>();
        public List<Bulk03TestCaseResult> gameplayCases = new List<Bulk03TestCaseResult>();
        public int totalCases;
        public int passedCases;
        public int failedCases;
        public bool allPassed;
    }

    /// <summary>
    /// Enforces horizontal scene camera bounds on the target and camera while keeping
    /// offline scene readiness explicitly distinct from network WorldReady.
    /// Also drives CN-02 / BULK-02 / BULK-03 multi-angle visual & gameplay automation.
    /// </summary>
    public sealed class BaLangHuyenSceneController : MonoBehaviour
    {
        public static readonly Vector3 UnreachableCourtyardCenter = new Vector3(16f, 0f, -16f);

        public OfflineMovement Player;
        public OrbitFollowCamera OrbitCamera;
        public Text HudLabel;
        public Vector3 BoundsCenter = new Vector3(0f, 4f, 0f);
        public Vector3 BoundsSize = new Vector3(56f, 20f, 56f);

        public WorldSceneValidationReport ValidationReport { get; private set; }
        public string ActiveCameraAngleLabel { get; private set; } = "Default (South-North 0 deg)";

        private int _frameCount;
        private float _elapsedTime;
        private float _measuredFps;

        public Bounds CameraBounds => new Bounds(BoundsCenter, BoundsSize);

        private void Awake()
        {
            Application.runInBackground = true;
            EnsureBulk02SceneEnhancements();
        }

        private void Start()
        {
            EnsureBulk02SceneEnhancements();
            RefreshValidation();

            string[] args = Environment.GetCommandLineArgs();
            if (Array.IndexOf(args, "-cn02Bulk03Test") >= 0)
            {
                StartCoroutine(RunCn02Bulk03Sequence());
            }
            else if (Array.IndexOf(args, "-cn02SmokeTest") >= 0)
            {
                StartCoroutine(RunCn02SmokeSequence());
            }
        }

        public void EnsureBulk02SceneEnhancements()
        {
            Scene scene = gameObject.scene.IsValid() ? gameObject.scene : SceneManager.GetActiveScene();
            if (!scene.IsValid() || !scene.isLoaded) return;

            if (Player == null)
            {
                foreach (GameObject r in scene.GetRootGameObjects())
                {
                    Player = r.GetComponentInChildren<OfflineMovement>();
                    if (Player != null) break;
                }
            }

            if (Player != null)
            {
                OfflinePathfinder pf = Player.GetComponent<OfflinePathfinder>();
                if (pf == null)
                {
                    pf = Player.gameObject.AddComponent<OfflinePathfinder>();
                }
                pf.GridCenter = Vector3.zero;
                pf.GridSize = new Vector2(56f, 56f);
                pf.CellSize = 0.5f;
                pf.ClearanceRadius = 0.52f;
                Player.Pathfinder = pf;
                Player.PathfindingEnabled = true;
            }

            GameObject collidersRoot = null;
            foreach (GameObject r in scene.GetRootGameObjects())
            {
                if (r.name == "Colliders")
                {
                    collidersRoot = r;
                    break;
                }
            }

            if (collidersRoot != null)
            {
                Material wallMat = null;
                Renderer existingRenderer = collidersRoot.GetComponentInChildren<Renderer>();
                if (existingRenderer != null)
                {
                    wallMat = existingRenderer.sharedMaterial;
                }

                // Ensure complete West/East perimeter fences so player cannot fall off terrain edges
                EnsureColliderBlock(collidersRoot.transform, "Perimeter_WestFence", new Vector3(-26f, 1.0f, 0f), new Vector3(1.0f, 2.0f, 48f), wallMat);
                EnsureColliderBlock(collidersRoot.transform, "Perimeter_EastFence", new Vector3(26f, 1.0f, 0f), new Vector3(1.0f, 2.0f, 48f), wallMat);

                // Ensure enclosed low-walled courtyard around (16, 0, -16) for deterministic Unreachable ground click testing
                Transform courtyard = collidersRoot.transform.Find("Obstacle_EnclosedCourtyard");
                if (courtyard == null)
                {
                    GameObject cyGo = new GameObject("Obstacle_EnclosedCourtyard");
                    cyGo.transform.SetParent(collidersRoot.transform, false);
                    cyGo.transform.localPosition = UnreachableCourtyardCenter;
                    courtyard = cyGo.transform;

                    EnsureColliderBlock(courtyard, "CourtyardWall_North", new Vector3(0f, 0.6f, 2.2f), new Vector3(4.8f, 1.2f, 0.4f), wallMat);
                    EnsureColliderBlock(courtyard, "CourtyardWall_South", new Vector3(0f, 0.6f, -2.2f), new Vector3(4.8f, 1.2f, 0.4f), wallMat);
                    EnsureColliderBlock(courtyard, "CourtyardWall_East", new Vector3(2.2f, 0.6f, 0f), new Vector3(0.4f, 1.2f, 4.8f), wallMat);
                    EnsureColliderBlock(courtyard, "CourtyardWall_West", new Vector3(-2.2f, 0.6f, 0f), new Vector3(0.4f, 1.2f, 4.8f), wallMat);
                }
            }

            Physics.SyncTransforms();
        }

        private static void EnsureColliderBlock(Transform parent, string name, Vector3 localPos, Vector3 localScale, Material sharedMat)
        {
            if (parent.Find(name) != null) return;
            GameObject cube = GameObject.CreatePrimitive(PrimitiveType.Cube);
            cube.name = name;
            cube.layer = 0;
            cube.transform.SetParent(parent, false);
            cube.transform.localPosition = localPos;
            cube.transform.localScale = localScale;
            if (sharedMat != null)
            {
                cube.GetComponent<Renderer>().sharedMaterial = sharedMat;
            }
        }

        public WorldSceneValidationReport RefreshValidation()
        {
            ValidationReport = WorldSceneValidator.ValidateLoadedScene(
                gameObject.scene.IsValid() ? gameObject.scene : SceneManager.GetActiveScene(),
                MapSceneCatalog.BaLangHuyen);
            return ValidationReport;
        }

        public Vector3 ClampToSceneBounds(Vector3 position)
        {
            Bounds b = CameraBounds;
            return new Vector3(
                Mathf.Clamp(position.x, b.min.x, b.max.x),
                Mathf.Clamp(position.y, b.min.y, b.max.y),
                Mathf.Clamp(position.z, b.min.z, b.max.z));
        }

        private void LateUpdate()
        {
            _frameCount++;
            _elapsedTime += Time.unscaledDeltaTime;
            if (_elapsedTime >= 0.25f)
            {
                _measuredFps = _frameCount / _elapsedTime;
                _frameCount = 0;
                _elapsedTime = 0f;
            }

            if (Player != null)
            {
                Vector3 pos = Player.transform.position;
                Vector3 clamped = ClampToSceneBounds(pos);
                if ((clamped - pos).sqrMagnitude > 0.0001f)
                {
                    CharacterController cc = Player.GetComponent<CharacterController>();
                    if (cc != null) cc.enabled = false;
                    Player.transform.position = new Vector3(clamped.x, pos.y, clamped.z);
                    if (cc != null) cc.enabled = true;
                }
            }

            if (OrbitCamera != null)
            {
                OrbitCamera.ApplyPose();
                OrbitCamera.transform.position = ClampToSceneBounds(OrbitCamera.transform.position);
                if (OrbitCamera.Target != null)
                {
                    OrbitCamera.transform.LookAt(OrbitCamera.Target.position + Vector3.up);
                }
            }

            UpdateHud();
        }

        private void UpdateHud()
        {
            if (HudLabel == null || Player == null) return;
            MapSceneDefinition def = MapSceneCatalog.BaLangHuyen;
            bool sceneReady = ValidationReport != null && ValidationReport.IsOfflineSceneReady;
            GameSessionState state = ValidationReport != null ? ValidationReport.SessionState : GameSessionState.LoadingScene;

            HudLabel.text =
                $"{def.DisplayTitle}\n" +
                $"Provenance: {def.ProvenanceVersion} | Calibration: {def.CalibrationStatus} (3+1 anchors NOT VERIFIED)\n" +
                $"OfflineSceneReady: {sceneReady} | SessionState: {state} (MoveRequestAllowed={GameSessionRules.CanSendMoveRequest(state)})\n" +
                $"View: {ActiveCameraAngleLabel} | FPS: {_measuredFps:F1}\n" +
                $"Player: {Player.MotionState} | Speed {Player.ActualSpeed:F1} | Intents {Player.AcceptedIntents} (Rej {Player.RejectedIntents}) | Pos {Player.transform.position:F2}\n" +
                $"LastInput: {Player.LastInput} | Waypoints: {Player.WaypointCount}";
        }

        private IEnumerator RunCn02SmokeSequence()
        {
            yield return null;
            yield return null;

            WorldSceneValidationReport report = RefreshValidation();
            Debug.Log(
                $"[CN-02] VALIDATION scene={MapSceneCatalog.BaLangHuyen.SceneName} " +
                $"offlineReady={report.IsOfflineSceneReady} sessionState={report.SessionState} " +
                $"roots={report.HasAllRequiredRoots} groundRenderer={report.HasVisibleGroundRenderer} " +
                $"groundCollider={report.HasEnabledGroundCollider} probes5={report.AllGroundProbesHit} " +
                $"spawnPos={report.ValidatedSpawnPosition:F2} cameraBounds={report.SceneCameraBounds}");

            foreach (GroundProbeHitResult p in report.ProbeResults)
            {
                Debug.Log($"[CN-02] PROBE name={p.ProbeName} hit={p.HitGround} point={p.HitPoint:F2} normal={p.HitNormal:F2} collider={p.ColliderName}");
            }

            string outDir = GetScreenshotOutDir();
            if (!string.IsNullOrEmpty(outDir))
            {
                Directory.CreateDirectory(outDir);
            }

            yield return CaptureAngleRoutine(outDir, "view1_spawn_south.png", "Angle 1: South Overview (Yaw=0, Pitch=45, Dist=18)", 0f, 45f, 18f);

            Vector3 startPos = Player.transform.position;
            Vector3 walkTarget = new Vector3(4f, 0f, -1f);
            OrbitCamera.SetOrbit(0f, 50f, 18f);
            OrbitCamera.ApplyPose();
            yield return null;

            Vector2 screenTarget = Player.ViewCamera.WorldToScreenPoint(walkTarget);
            bool accepted = Player.TryPointer(screenTarget);
            Debug.Log($"[CN-02] WALK_START start={startPos:F2} target={walkTarget:F2} screen={screenTarget:F1} accepted={accepted}");

            float walkStartRealtime = Time.realtimeSinceStartup;
            int simFrames = 0;
            while (Player.IsMoving && (Time.realtimeSinceStartup - walkStartRealtime) < 8f)
            {
                simFrames++;
                yield return null;
            }

            float elapsed = Mathf.Max(0.001f, Time.realtimeSinceStartup - walkStartRealtime);
            float avgFps = simFrames / elapsed;
            float distToTarget = Vector3.Distance(new Vector3(Player.transform.position.x, 0f, Player.transform.position.z), walkTarget);
            bool walkPassed = report.IsOfflineSceneReady && accepted && distToTarget < 0.35f;
            Debug.Log(
                $"[CN-02] WALK_END end={Player.transform.position:F2} distance={distToTarget:F3} " +
                $"frames={simFrames} elapsed={elapsed:F2}s avgFps={avgFps:F1} totalMemMB={(GC.GetTotalMemory(false) / (1024f * 1024f)):F2} passed={walkPassed}");

            yield return CaptureAngleRoutine(outDir, "view2_east_walked.png", "Angle 2: East View Post-Walk (Yaw=90, Pitch=40, Dist=16)", 90f, 40f, 16f);
            yield return CaptureAngleRoutine(outDir, "view3_north_elevated.png", "Angle 3: North Elevated (Yaw=180, Pitch=60, Dist=22)", 180f, 60f, 22f);
            yield return CaptureAngleRoutine(outDir, "view4_west_close.png", "Angle 4: West Diagonal Close (Yaw=240, Pitch=35, Dist=12)", 240f, 35f, 12f);

            Application.Quit(walkPassed ? 0 : 2);
        }

        private IEnumerator RunCn02Bulk03Sequence()
        {
            yield return null;
            yield return null;

            EnsureBulk02SceneEnhancements();
            WorldSceneValidationReport report = RefreshValidation();

            var summary = new Bulk03RuntimeSummary
            {
                timestampUtc = DateTime.UtcNow.ToString("o"),
                unityVersion = Application.unityVersion,
                platform = Application.platform.ToString(),
                graphicsDeviceName = SystemInfo.graphicsDeviceName,
                graphicsDeviceType = SystemInfo.graphicsDeviceType.ToString(),
                isBatchMode = Application.isBatchMode,
                sceneName = MapSceneCatalog.BaLangHuyen.SceneName,
                provenanceVersion = MapSceneCatalog.BaLangHuyen.ProvenanceVersion,
                isSyntheticFallback = MapSceneCatalog.BaLangHuyen.IsSyntheticFallback,
                calibrationStatus = MapSceneCatalog.BaLangHuyen.CalibrationStatus.ToString(),
                offlineSceneReady = report.IsOfflineSceneReady,
                sessionState = report.SessionState.ToString(),
                moveRequestNetworkAllowed = GameSessionRules.CanSendMoveRequest(report.SessionState)
            };

            string outDir = GetScreenshotOutDir();
            if (!string.IsNullOrEmpty(outDir))
            {
                Directory.CreateDirectory(outDir);
            }

            // Probe 9 positions across the village (5 standard catalog probes + 4 corner/road probes)
            Scene activeScene = gameObject.scene.IsValid() ? gameObject.scene : SceneManager.GetActiveScene();
            Vector3[] extraProbes =
            {
                new Vector3(0f, 0f, 0f),
                new Vector3(0f, 0f, 18f),
                new Vector3(0f, 0f, -18f),
                new Vector3(18f, 0f, 0f),
                new Vector3(-18f, 0f, 0f),
                new Vector3(-6f, 0f, -8f),
                new Vector3(6f, 0f, 8f),
                new Vector3(-16f, 0f, 16f),
                new Vector3(16f, 0f, 16f)
            };
            string[] extraNames =
            {
                "Probe_CenterPlaza", "Probe_NorthGatePath", "Probe_SouthGatePath",
                "Probe_EastBridgeApproach", "Probe_WestMarketApproach",
                "Probe_SouthWestPlazaApproach", "Probe_NorthEastPlazaApproach",
                "Probe_NorthWestMeadow", "Probe_NorthEastMeadow"
            };
            for (int i = 0; i < extraProbes.Length; i++)
            {
                bool hitOk = WorldSceneValidator.TrySampleGroundHeight(activeScene, extraProbes[i], 8, out Vector3 pt, out RaycastHit hit);
                summary.groundProbes.Add(new Bulk03ProbeEntry
                {
                    probeName = extraNames[i],
                    worldOrigin = extraProbes[i].ToString("F2"),
                    hitGround = hitOk,
                    hitPoint = pt.ToString("F2"),
                    hitNormal = hitOk ? hit.normal.ToString("F2") : Vector3.zero.ToString("F2"),
                    colliderName = hitOk && hit.collider != null ? hit.collider.gameObject.name : ""
                });
            }

            // Angle 1: Spawn South Overview
            yield return CaptureAngleRoutine(outDir, "bulk03_view1_south_spawn.png", "Angle 1: South Spawn Overview (Yaw=0, Pitch=45, Dist=18)", 0f, 45f, 18f, summary.capturedScreenshots);

            // Case 1: Idle state at spawn
            summary.gameplayCases.Add(new Bulk03TestCaseResult
            {
                caseId = "CASE-01-IDLE-SPAWN",
                description = "Verify initial Idle state on valid Layer-8 ground at OfflineSpawn_VillageCenter",
                startPosition = Player.transform.position.ToString("F2"),
                targetPosition = Player.transform.position.ToString("F2"),
                endPosition = Player.transform.position.ToString("F2"),
                motionStateObserved = Player.MotionState,
                lastInputObserved = Player.LastInput,
                waypointCount = 0,
                durationSeconds = 0.02f,
                averageFps = _measuredFps,
                passed = report.IsOfflineSceneReady && Player.MotionState == "Idle" && !Player.IsMoving,
                notes = "Initial spawn verified on ground"
            });

            // Case 2: Walk along Plaza to East Road
            yield return ExecuteWalkCase(
                summary,
                "CASE-02-WALK-EAST-PLAZA",
                "Offline MoveIntent along plaza to East Road (6, 0, 0)",
                new Vector3(6f, 0f, 0f),
                usePathfinding: true,
                expectReach: true);

            // Angle 2: East Gate View after Plaza Walk
            yield return CaptureAngleRoutine(outDir, "bulk03_view2_east_road.png", "Angle 2: East Road View (Yaw=90, Pitch=42, Dist=16)", 90f, 42f, 16f, summary.capturedScreenshots);

            // Case 3: Obstacle-Avoiding Pathfinding around Obstacle_WestWall (-5.5, 1.1, 2)
            TeleportPlayer(new Vector3(-2f, 0.05f, 2f));
            yield return null;
            yield return ExecuteWalkCase(
                summary,
                "CASE-03-PATHFIND-AROUND-WEST-WALL",
                "Obstacle-avoiding A* pathfinding around Obstacle_WestWall from (-2,0,2) to (-8.5,0,2)",
                new Vector3(-8.5f, 0f, 2f),
                usePathfinding: true,
                expectReach: true);

            // Angle 3: North Elevated View after Obstacle Avoidance
            yield return CaptureAngleRoutine(outDir, "bulk03_view3_north_obstacle_routed.png", "Angle 3: North Elevated Post-Avoidance (Yaw=180, Pitch=58, Dist=20)", 180f, 58f, 20f, summary.capturedScreenshots);

            // Case 4: Obstacle-Avoiding Pathfinding around Obstacle_EastLanternPillar (4.5, 1.2, -3.5)
            TeleportPlayer(new Vector3(2f, 0.05f, -3.5f));
            yield return null;
            yield return ExecuteWalkCase(
                summary,
                "CASE-04-PATHFIND-AROUND-EAST-PILLAR",
                "Obstacle-avoiding A* pathfinding around Obstacle_EastLanternPillar from (2,0,-3.5) to (7.5,0,-3.5)",
                new Vector3(7.5f, 0f, -3.5f),
                usePathfinding: true,
                expectReach: true);

            // Case 5: Direct collision Blocked state when Pathfinding is disabled against Obstacle_WestWall
            TeleportPlayer(new Vector3(-2f, 0.05f, 2f));
            yield return null;
            yield return ExecuteBlockedWallCase(
                summary,
                "CASE-05-BLOCKED-BY-WALL",
                "Direct movement without pathfinding stops safely at Obstacle_WestWall with MotionState=Blocked",
                new Vector3(-8.5f, 0f, 2f));

            // Case 6: Unreachable target inside enclosed stone courtyard (16, 0, -16)
            TeleportPlayer(new Vector3(8f, 0.05f, -12f));
            yield return null;
            float t0 = Time.realtimeSinceStartup;
            Vector3 posBeforeUnreachable = Player.transform.position;
            bool unreachableAccepted = Player.TryMoveToWorld(UnreachableCourtyardCenter);
            yield return null;
            float dtUnreachable = Mathf.Max(0.001f, Time.realtimeSinceStartup - t0);
            summary.gameplayCases.Add(new Bulk03TestCaseResult
            {
                caseId = "CASE-06-UNREACHABLE-COURTYARD",
                description = "Target inside enclosed stone courtyard (16, 0, -16) is rejected as Unreachable target",
                startPosition = posBeforeUnreachable.ToString("F2"),
                targetPosition = UnreachableCourtyardCenter.ToString("F2"),
                endPosition = Player.transform.position.ToString("F2"),
                motionStateObserved = Player.MotionState,
                lastInputObserved = Player.LastInput,
                waypointCount = Player.WaypointCount,
                durationSeconds = dtUnreachable,
                averageFps = _measuredFps,
                passed = !unreachableAccepted && !Player.IsMoving && Player.MotionState == "Unreachable" &&
                         Vector3.Distance(posBeforeUnreachable, Player.transform.position) < 0.05f,
                notes = $"Accepted={unreachableAccepted}, State={Player.MotionState}, Input={Player.LastInput}"
            });

            // Case 7: UI Shield & Off-screen click rejection
            Vector3 posBeforeUi = Player.transform.position;
            bool uiClickAccepted = Player.TryPointer(new Vector2(60f, Screen.height - 40f));
            string uiRejectReason = Player.LastInput;
            bool offscreenAccepted = Player.TryPointer(new Vector2(-25f, -25f));
            string offscreenReason = Player.LastInput;
            summary.gameplayCases.Add(new Bulk03TestCaseResult
            {
                caseId = "CASE-07-UI-AND-OFFSCREEN-REJECTION",
                description = "Clicks on top-left HUD shield and off-screen coordinates are rejected without moving player",
                startPosition = posBeforeUi.ToString("F2"),
                targetPosition = "UI(60, H-40) / Offscreen(-25,-25)",
                endPosition = Player.transform.position.ToString("F2"),
                motionStateObserved = Player.MotionState,
                lastInputObserved = $"{uiRejectReason} | {offscreenReason}",
                waypointCount = 0,
                durationSeconds = 0.01f,
                averageFps = _measuredFps,
                passed = !uiClickAccepted && uiRejectReason == "UI consumed click" &&
                         !offscreenAccepted && offscreenReason == "Outside view" &&
                         Vector3.Distance(posBeforeUi, Player.transform.position) < 0.05f,
                notes = $"UI='{uiRejectReason}', Offscreen='{offscreenReason}'"
            });

            // Case 8: North Gate Approach Walk
            TeleportPlayer(new Vector3(0f, 0.05f, 6f));
            yield return null;
            yield return ExecuteWalkCase(
                summary,
                "CASE-08-WALK-NORTH-GATE",
                "Offline MoveIntent along North road from (0,0,6) to North Gate (0,0,16)",
                new Vector3(0f, 0f, 16f),
                usePathfinding: true,
                expectReach: true);

            // Angle 4: West Diagonal Close View at North Gate
            yield return CaptureAngleRoutine(outDir, "bulk03_view4_west_northgate.png", "Angle 4: West Diagonal at North Gate (Yaw=240, Pitch=35, Dist=12)", 240f, 35f, 12f, summary.capturedScreenshots);

            summary.measuredFps = _measuredFps;
            summary.managedMemoryMB = GC.GetTotalMemory(false) / (1024f * 1024f);
            summary.totalCases = summary.gameplayCases.Count;
            foreach (Bulk03TestCaseResult c in summary.gameplayCases)
            {
                if (c.passed) summary.passedCases++;
                else summary.failedCases++;
            }
            summary.allPassed = summary.offlineSceneReady && summary.failedCases == 0 && summary.passedCases == summary.totalCases;

            if (!string.IsNullOrEmpty(outDir))
            {
                string jsonPath = Path.Combine(outDir, "bulk03-gameplay-results.json");
                File.WriteAllText(jsonPath, JsonUtility.ToJson(summary, true), Encoding.UTF8);
                Debug.Log($"[CN02-BULK03] SUMMARY json={jsonPath} total={summary.totalCases} passed={summary.passedCases} failed={summary.failedCases} fps={summary.measuredFps:F1}");
            }

            Application.Quit(summary.allPassed ? 0 : 2);
        }

        private IEnumerator ExecuteWalkCase(
            Bulk03RuntimeSummary summary,
            string caseId,
            string description,
            Vector3 target,
            bool usePathfinding,
            bool expectReach)
        {
            Player.PathfindingEnabled = usePathfinding;
            Vector3 startPos = Player.transform.position;
            float tStart = Time.realtimeSinceStartup;

            bool accepted = Player.TryMoveToWorld(target);
            int wpCount = Player.WaypointCount;
            string observedStateDuringWalk = Player.MotionState;

            int frames = 0;
            while (Player.IsMoving && (Time.realtimeSinceStartup - tStart) < 9f)
            {
                frames++;
                if (Player.MotionState == "Walking") observedStateDuringWalk = "Walking";
                yield return null;
            }

            float elapsed = Mathf.Max(0.001f, Time.realtimeSinceStartup - tStart);
            float avgFps = frames > 0 ? frames / elapsed : _measuredFps;
            float planarDist = Vector2.Distance(
                new Vector2(Player.transform.position.x, Player.transform.position.z),
                new Vector2(target.x, target.z));

            bool passed = expectReach
                ? (accepted && planarDist < 0.38f && Player.MotionState == "Idle" && observedStateDuringWalk == "Walking")
                : !accepted;

            summary.gameplayCases.Add(new Bulk03TestCaseResult
            {
                caseId = caseId,
                description = description,
                startPosition = startPos.ToString("F2"),
                targetPosition = target.ToString("F2"),
                endPosition = Player.transform.position.ToString("F2"),
                motionStateObserved = $"{observedStateDuringWalk} -> {Player.MotionState}",
                lastInputObserved = Player.LastInput,
                waypointCount = wpCount,
                durationSeconds = elapsed,
                averageFps = avgFps,
                passed = passed,
                notes = $"planarDist={planarDist:F3}m, frames={frames}"
            });

            Debug.Log($"[CN02-BULK03] CASE id={caseId} passed={passed} start={startPos:F2} end={Player.transform.position:F2} dist={planarDist:F3} wp={wpCount} state={observedStateDuringWalk}->{Player.MotionState}");
        }

        private IEnumerator ExecuteBlockedWallCase(
            Bulk03RuntimeSummary summary,
            string caseId,
            string description,
            Vector3 targetBehindWall)
        {
            bool prevPf = Player.PathfindingEnabled;
            Player.PathfindingEnabled = false;
            Vector3 startPos = Player.transform.position;
            float tStart = Time.realtimeSinceStartup;

            bool accepted = Player.TryMoveToWorld(targetBehindWall);
            int frames = 0;
            int blockedFrames = 0;
            while (Player.IsMoving && (Time.realtimeSinceStartup - tStart) < 3.5f)
            {
                frames++;
                if (Player.MotionState == "Blocked")
                {
                    blockedFrames++;
                    if (blockedFrames >= 10) break;
                }
                yield return null;
            }

            string finalState = Player.MotionState;
            float elapsed = Mathf.Max(0.001f, Time.realtimeSinceStartup - tStart);
            float avgFps = frames > 0 ? frames / elapsed : _measuredFps;
            bool stoppedBeforeWall = Player.transform.position.x > -5.1f && Player.transform.position.x < -3.5f;
            bool passed = accepted && stoppedBeforeWall && (finalState == "Blocked" || blockedFrames > 0);

            Player.PathfindingEnabled = prevPf;

            summary.gameplayCases.Add(new Bulk03TestCaseResult
            {
                caseId = caseId,
                description = description,
                startPosition = startPos.ToString("F2"),
                targetPosition = targetBehindWall.ToString("F2"),
                endPosition = Player.transform.position.ToString("F2"),
                motionStateObserved = finalState,
                lastInputObserved = Player.LastInput,
                waypointCount = 1,
                durationSeconds = elapsed,
                averageFps = avgFps,
                passed = passed,
                notes = $"Stopped at x={Player.transform.position.x:F2} before wall x=-5.5 (blockedFrames={blockedFrames})"
            });

            Debug.Log($"[CN02-BULK03] CASE id={caseId} passed={passed} end={Player.transform.position:F2} state={finalState}");
        }

        private void TeleportPlayer(Vector3 pos)
        {
            CharacterController cc = Player.GetComponent<CharacterController>();
            if (cc != null) cc.enabled = false;
            Player.transform.position = pos;
            if (cc != null) cc.enabled = true;
            Physics.SyncTransforms();
        }

        private IEnumerator CaptureAngleRoutine(
            string outDir,
            string fileName,
            string label,
            float yaw,
            float pitch,
            float dist,
            List<string> recordedFiles = null)
        {
            ActiveCameraAngleLabel = label;
            if (OrbitCamera != null)
            {
                OrbitCamera.SetOrbit(yaw, pitch, dist);
                OrbitCamera.ApplyPose();
                OrbitCamera.transform.position = ClampToSceneBounds(OrbitCamera.transform.position);
            }
            UpdateHud();
            yield return null;
            yield return new WaitForEndOfFrame();

            if (!Application.isBatchMode && !string.IsNullOrEmpty(outDir))
            {
                string fullPath = Path.Combine(outDir, fileName);
                ScreenCapture.CaptureScreenshot(fullPath);
                yield return null;
                yield return new WaitForEndOfFrame();
                yield return new WaitForSecondsRealtime(0.2f);
                if (File.Exists(fullPath) && recordedFiles != null)
                {
                    recordedFiles.Add(fileName);
                }
                Debug.Log($"[CN-02] CAPTURED angle='{label}' path={fullPath} exists={File.Exists(fullPath)}");
            }
        }

        private static string GetScreenshotOutDir()
        {
            string[] args = Environment.GetCommandLineArgs();
            int idx = Array.IndexOf(args, "-cn02ScreenshotDir");
            if (idx >= 0 && idx + 1 < args.Length)
            {
                return args[idx + 1];
            }
            return string.Empty;
        }
    }
}
