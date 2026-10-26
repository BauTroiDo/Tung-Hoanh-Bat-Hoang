using System;
using System.Collections;
using System.Collections.Generic;
using JXClient.Domain;
using JXClient.Movement;
using UnityEngine;
using UnityEngine.SceneManagement;

namespace JXClient.World
{
    public readonly struct GroundProbeHitResult
    {
        public string ProbeName { get; }
        public Vector3 ProbeOrigin { get; }
        public bool HitGround { get; }
        public Vector3 HitPoint { get; }
        public Vector3 HitNormal { get; }
        public string ColliderName { get; }

        public GroundProbeHitResult(
            string probeName,
            Vector3 probeOrigin,
            bool hitGround,
            Vector3 hitPoint,
            Vector3 hitNormal,
            string colliderName)
        {
            ProbeName = probeName;
            ProbeOrigin = probeOrigin;
            HitGround = hitGround;
            HitPoint = hitPoint;
            HitNormal = hitNormal;
            ColliderName = colliderName;
        }
    }

    public sealed class WorldSceneValidationReport
    {
        public static readonly string[] RequiredRootNames =
        {
            "Ground",
            "Environment",
            "StaticProps",
            "SpawnPoints",
            "Colliders",
            "Lighting",
            "Nav"
        };

        public MapSceneDefinition Definition { get; }
        public bool HasAllRequiredRoots { get; }
        public IReadOnlyList<string> MissingRoots { get; }
        public bool HasVisibleGroundRenderer { get; }
        public bool HasEnabledGroundCollider { get; }
        public bool AllGroundProbesHit { get; }
        public IReadOnlyList<GroundProbeHitResult> ProbeResults { get; }
        public bool HasValidSpawnOnGround { get; }
        public Vector3 ValidatedSpawnPosition { get; }
        public bool HasValidCameraAndBounds { get; }
        public Bounds SceneCameraBounds { get; }
        public bool IsOfflineSceneReady { get; }
        public GameSessionState SessionState { get; }

        public WorldSceneValidationReport(
            MapSceneDefinition definition,
            bool hasAllRequiredRoots,
            IReadOnlyList<string> missingRoots,
            bool hasVisibleGroundRenderer,
            bool hasEnabledGroundCollider,
            bool allGroundProbesHit,
            IReadOnlyList<GroundProbeHitResult> probeResults,
            bool hasValidSpawnOnGround,
            Vector3 validatedSpawnPosition,
            bool hasValidCameraAndBounds,
            Bounds sceneCameraBounds)
        {
            Definition = definition;
            HasAllRequiredRoots = hasAllRequiredRoots;
            MissingRoots = missingRoots;
            HasVisibleGroundRenderer = hasVisibleGroundRenderer;
            HasEnabledGroundCollider = hasEnabledGroundCollider;
            AllGroundProbesHit = allGroundProbesHit;
            ProbeResults = probeResults;
            HasValidSpawnOnGround = hasValidSpawnOnGround;
            ValidatedSpawnPosition = validatedSpawnPosition;
            HasValidCameraAndBounds = hasValidCameraAndBounds;
            SceneCameraBounds = sceneCameraBounds;

            IsOfflineSceneReady =
                hasAllRequiredRoots &&
                hasVisibleGroundRenderer &&
                hasEnabledGroundCollider &&
                allGroundProbesHit &&
                hasValidSpawnOnGround &&
                hasValidCameraAndBounds;

            // Offline scene readiness NEVER grants network WorldReady (which requires server snapshot & entity ID).
            SessionState = IsOfflineSceneReady ? GameSessionState.OfflineReady : GameSessionState.LoadingScene;
        }
    }

    public static class WorldSceneValidator
    {
        public static bool TrySampleGroundHeight(Scene scene, Vector3 horizontalPosition, int groundLayer,
            out Vector3 groundPoint, out RaycastHit hitInfo)
        {
            groundPoint = Vector3.zero;
            hitInfo = default;
            if (!scene.IsValid() || !scene.isLoaded) return false;
            var ray = new Ray(new Vector3(horizontalPosition.x, 40f, horizontalPosition.z), Vector3.down);
            float nearest = 80f;
            bool found = false;
            // Query owned colliders so foreign scenes cannot supply or hide this scene's ground.
            foreach (GameObject root in scene.GetRootGameObjects())
            {
                foreach (Collider collider in root.GetComponentsInChildren<Collider>(false))
                {
                    if (!collider.enabled || !collider.gameObject.activeInHierarchy || collider.isTrigger ||
                        collider.gameObject.layer != groundLayer) continue;
                    if (collider.Raycast(ray, out RaycastHit candidate, nearest) && candidate.normal.y >= 0.5f)
                    {
                        nearest = candidate.distance;
                        hitInfo = candidate;
                        groundPoint = candidate.point;
                        found = true;
                    }
                }
            }
            return found;
        }

        public static WorldSceneValidationReport ValidateLoadedScene(Scene scene, MapSceneDefinition definition)
        {
            if (!scene.IsValid() || !scene.isLoaded)
            {
                return new WorldSceneValidationReport(
                    definition,
                    false,
                    WorldSceneValidationReport.RequiredRootNames,
                    false,
                    false,
                    false,
                    Array.Empty<GroundProbeHitResult>(),
                    false,
                    Vector3.zero,
                    false,
                    default);
            }

            Physics.SyncTransforms();

            GameObject[] roots = scene.GetRootGameObjects();
            var rootByName = new Dictionary<string, GameObject>(StringComparer.Ordinal);
            foreach (GameObject root in roots)
            {
                if (!rootByName.ContainsKey(root.name))
                {
                    rootByName.Add(root.name, root);
                }
            }

            var missingRoots = new List<string>();
            foreach (string req in WorldSceneValidationReport.RequiredRootNames)
            {
                if (!rootByName.ContainsKey(req))
                {
                    missingRoots.Add(req);
                }
            }
            bool hasAllRoots = missingRoots.Count == 0;

            bool hasVisibleGroundRenderer = false;
            bool hasEnabledGroundCollider = false;
            if (rootByName.TryGetValue("Ground", out GameObject groundRoot))
            {
                foreach (Renderer r in groundRoot.GetComponentsInChildren<Renderer>(false))
                {
                    if (r.enabled && r.sharedMaterial != null)
                    {
                        hasVisibleGroundRenderer = true;
                        break;
                    }
                }

                foreach (Collider c in groundRoot.GetComponentsInChildren<Collider>(false))
                {
                    if (c.enabled && c.gameObject.layer == definition.GroundLayer)
                    {
                        hasEnabledGroundCollider = true;
                        break;
                    }
                }
            }

            var probeResults = new List<GroundProbeHitResult>(definition.GroundProbes.Count);
            bool allProbesHit = definition.GroundProbes.Count >= 5;
            foreach (GroundProbeDefinition probe in definition.GroundProbes)
            {
                Vector3 probeOrigin = new Vector3(probe.HorizontalPosition.x, 40f, probe.HorizontalPosition.z);
                if (TrySampleGroundHeight(scene, probe.HorizontalPosition, definition.GroundLayer, out Vector3 hitPt, out RaycastHit hit))
                {
                    probeResults.Add(new GroundProbeHitResult(
                        probe.Name,
                        probeOrigin,
                        true,
                        hitPt,
                        hit.normal,
                        hit.collider.gameObject.name));
                }
                else
                {
                    allProbesHit = false;
                    probeResults.Add(new GroundProbeHitResult(
                        probe.Name,
                        probeOrigin,
                        false,
                        Vector3.zero,
                        Vector3.zero,
                        string.Empty));
                }
            }

            bool hasValidSpawn = false;
            Vector3 validatedSpawnPos = Vector3.zero;
            if (rootByName.TryGetValue("SpawnPoints", out GameObject spawnRoot))
            {
                Transform spawnTransform = spawnRoot.transform.Find(definition.OfflineSpawnMarkerName);
                if (spawnTransform != null &&
                    TrySampleGroundHeight(scene, spawnTransform.position, definition.GroundLayer, out Vector3 groundSpawn, out _))
                {
                    hasValidSpawn = true;
                    validatedSpawnPos = groundSpawn;
                }
            }

            bool hasValidCamera = false;
            Bounds cameraBounds = definition.CameraBounds;
            foreach (GameObject root in roots)
            {
                Camera cam = root.GetComponentInChildren<Camera>(false);
                if (cam != null && cam.CompareTag("MainCamera"))
                {
                    hasValidCamera = cameraBounds.size.x > 10f && cameraBounds.size.z > 10f && cameraBounds.Contains(validatedSpawnPos);
                    break;
                }
            }

            return new WorldSceneValidationReport(
                definition,
                hasAllRoots,
                missingRoots,
                hasVisibleGroundRenderer,
                hasEnabledGroundCollider,
                allProbesHit,
                probeResults,
                hasValidSpawn,
                validatedSpawnPos,
                hasValidCamera,
                cameraBounds);
        }
    }

    public sealed class MapSceneLoader
    {
        public bool IsLoading { get; private set; }
        public MapSceneDefinition ActiveDefinition { get; private set; }
        public Scene LoadedScene { get; private set; }
        public WorldSceneValidationReport LastValidationReport { get; private set; }
        public string LastError { get; private set; } = string.Empty;

        private int _loadGeneration;
        private AsyncOperation _pendingLoad;

        public void CancelCurrentLoad()
        {
            if (IsLoading)
            {
                _loadGeneration++;
                IsLoading = false;
                LastError = "Load cancelled by caller.";
            }
        }

        public IEnumerator LoadMapRoutine(
            MapId mapId,
            LoadSceneMode mode = LoadSceneMode.Additive,
            Action<WorldSceneValidationReport> onCompleted = null)
        {
            if (IsLoading || _pendingLoad != null)
            {
                LastError = "Another map load is already in progress or being cleaned up.";
                onCompleted?.Invoke(null);
                yield break;
            }

            if (!MapSceneCatalog.TryGet(mapId, out MapSceneDefinition definition))
            {
                LastError = $"MapId {mapId.Value} is not registered in MapSceneCatalog.";
                onCompleted?.Invoke(null);
                yield break;
            }

            // A second instance has the same scene name; cancellation must never select/unload the original.
            Scene existingTarget = SceneManager.GetSceneByPath(definition.ScenePath);
            if (existingTarget.IsValid() && existingTarget.isLoaded)
            {
                LastError = $"Scene '{definition.SceneName}' is already loaded; unload it before loading another instance.";
                onCompleted?.Invoke(null);
                yield break;
            }

            IsLoading = true;
            LastError = string.Empty;
            int myGeneration = ++_loadGeneration;

            AsyncOperation op = null;
            try { op = SceneManager.LoadSceneAsync(definition.SceneName, mode); }
            catch (Exception ex) { LastError = $"Scene load could not start: {ex.Message}"; }
            if (op == null)
            {
                IsLoading = false;
                if (string.IsNullOrEmpty(LastError)) LastError = $"SceneManager.LoadSceneAsync returned null for scene '{definition.SceneName}'.";
                onCompleted?.Invoke(null);
                yield break;
            }

            _pendingLoad = op;
            // Unity cannot abort this operation; remain scheduled until cancelled-scene cleanup finishes.
            while (!op.isDone)
            {
                yield return null;
            }

            Scene loaded = SceneManager.GetSceneByName(definition.SceneName);
            if (myGeneration != _loadGeneration)
            {
                loaded = SceneManager.GetSceneByName(definition.SceneName);
                if (loaded.IsValid() && loaded.isLoaded && mode == LoadSceneMode.Additive)
                {
                    foreach (GameObject r in loaded.GetRootGameObjects())
                    {
                        r.SetActive(false);
                    }
                    AsyncOperation cancelUnload = SceneManager.UnloadSceneAsync(loaded);
                    if (cancelUnload != null)
                    {
                        while (!cancelUnload.isDone)
                        {
                            yield return null;
                        }
                    }
                }
                _pendingLoad = null;
                LastError = "Load cancelled by caller.";
                onCompleted?.Invoke(null);
                yield break;
            }

            _pendingLoad = null;
            IsLoading = false;
            LoadedScene = loaded;
            ActiveDefinition = definition;
            LastValidationReport = WorldSceneValidator.ValidateLoadedScene(loaded, definition);

            if (!LastValidationReport.IsOfflineSceneReady)
            {
                LastError = $"Scene '{definition.SceneName}' failed offline validation.";
            }

            onCompleted?.Invoke(LastValidationReport);
        }

        public IEnumerator UnloadCurrentMapRoutine()
        {
            CancelCurrentLoad();
            while (_pendingLoad != null) yield return null;
            if (LoadedScene.IsValid() && LoadedScene.isLoaded)
            {
                string sceneName = LoadedScene.name;
                AsyncOperation unloadOp = SceneManager.UnloadSceneAsync(LoadedScene);
                if (unloadOp != null)
                {
                    while (!unloadOp.isDone)
                    {
                        yield return null;
                    }
                }
                Debug.Log($"[CN-02][MapSceneLoader] Unloaded scene '{sceneName}'.");
            }

            LoadedScene = default;
            ActiveDefinition = null;
            LastValidationReport = null;
        }
    }
}
