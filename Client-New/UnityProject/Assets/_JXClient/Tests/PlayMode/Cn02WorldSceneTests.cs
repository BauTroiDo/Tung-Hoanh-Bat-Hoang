using System.Collections;
using JXClient.Domain;
using JXClient.Movement;
using JXClient.World;
using NUnit.Framework;
using UnityEngine;
using UnityEngine.SceneManagement;
using UnityEngine.TestTools;

public class Cn02WorldSceneTests
{
    private MapSceneLoader _loader;

    [UnitySetUp]
    public IEnumerator SetUp()
    {
        _loader = new MapSceneLoader();
        yield return null;
    }

    [UnityTearDown]
    public IEnumerator TearDown()
    {
        if (_loader != null)
        {
            yield return _loader.UnloadCurrentMapRoutine();
        }
        Scene leftover = SceneManager.GetSceneByName("BaLangHuyen");
        if (leftover.IsValid() && leftover.isLoaded)
        {
            foreach (GameObject r in leftover.GetRootGameObjects())
            {
                UnityEngine.Object.DestroyImmediate(r);
            }
            AsyncOperation unloadOp = SceneManager.UnloadSceneAsync(leftover);
            if (unloadOp != null)
            {
                while (!unloadOp.isDone)
                {
                    yield return null;
                }
            }
        }
        yield return null;
        Physics.SyncTransforms();
    }

    [UnityTest]
    public IEnumerator MapSceneCatalog_DefinesBaLangHuyenSyntheticMetadataAndUnverifiedCalibration()
    {
        Assert.That(MapSceneCatalog.TryGet(MapId.BaLangHuyen, out MapSceneDefinition def), Is.True);
        Assert.That(def.MapId.Value, Is.EqualTo(53));
        Assert.That(def.SceneName, Is.EqualTo("BaLangHuyen"));
        Assert.That(def.IsSyntheticFallback, Is.True, "CN-02 fallback scene must explicitly declare IsSyntheticFallback=true");
        Assert.That(def.CalibrationStatus, Is.EqualTo(MapCalibrationStatus.Unverified), "Real 3+1 anchor calibration is not yet verified");
        Assert.That(def.GroundProbes.Count, Is.EqualTo(5));
        yield return null;
    }

    [UnityTest]
    public IEnumerator MapSceneLoader_RejectsUnknownMapAndSupportsCancellation()
    {
        WorldSceneValidationReport report = null;
        yield return _loader.LoadMapRoutine(new MapId(999), LoadSceneMode.Additive, r => report = r);
        Assert.That(report, Is.Null);
        Assert.That(_loader.LastError, Does.Contain("not registered"));

        // Start loading Map 53 and cancel immediately before completion
        IEnumerator loadEnum = _loader.LoadMapRoutine(MapId.BaLangHuyen, LoadSceneMode.Additive, r => report = r);
        Assert.That(loadEnum.MoveNext(), Is.True);
        Assert.That(_loader.IsLoading, Is.True);
        _loader.CancelCurrentLoad();
        Assert.That(_loader.IsLoading, Is.False);
        Assert.That(_loader.LastError, Does.Contain("cancelled"));

        // Wait until the underlying Unity SceneManager async load finishes so we can cleanly unload it
        for (int i = 0; i < 60 && !SceneManager.GetSceneByName("BaLangHuyen").isLoaded; i++)
        {
            yield return null;
        }
        while (loadEnum.MoveNext())
        {
            yield return null;
        }
        Assert.That(report, Is.Null);
    }

    [UnityTest]
    public IEnumerator BaLangHuyenScene_LoadsValidHierarchyFiveGroundProbesAndUnloadsCleanly()
    {
        WorldSceneValidationReport report = null;
        yield return _loader.LoadMapRoutine(MapId.BaLangHuyen, LoadSceneMode.Additive, r => report = r);

        Assert.That(report, Is.Not.Null);
        Assert.That(report.HasAllRequiredRoots, Is.True, $"Missing roots: {string.Join(", ", report.MissingRoots)}");
        Assert.That(report.HasVisibleGroundRenderer, Is.True);
        Assert.That(report.HasEnabledGroundCollider, Is.True);
        Assert.That(report.AllGroundProbesHit, Is.True);
        Assert.That(report.ProbeResults.Count, Is.EqualTo(5));

        foreach (GroundProbeHitResult p in report.ProbeResults)
        {
            Assert.That(p.HitGround, Is.True, $"Ground probe {p.ProbeName} failed to hit Layer 8 Ground");
            Assert.That(p.HitNormal.y, Is.GreaterThanOrEqualTo(0.5f));
        }

        Assert.That(report.HasValidSpawnOnGround, Is.True);
        Assert.That(report.HasValidCameraAndBounds, Is.True);
        Assert.That(report.IsOfflineSceneReady, Is.True);

        // Offline scene load MUST NOT grant network WorldReady
        Assert.That(report.SessionState, Is.EqualTo(GameSessionState.OfflineReady));
        Assert.That(GameSessionRules.CanSendMoveRequest(report.SessionState), Is.False);

        yield return _loader.UnloadCurrentMapRoutine();
        Assert.That(_loader.LoadedScene.isLoaded, Is.False);
    }

    [UnityTest]
    public IEnumerator BaLangHuyenScene_OfflinePlayerWalksRoadStopsAtObstacleAndRespectsCameraBounds()
    {
        WorldSceneValidationReport report = null;
        yield return _loader.LoadMapRoutine(MapId.BaLangHuyen, LoadSceneMode.Additive, r => report = r);
        Assert.That(report, Is.Not.Null);
        Assert.That(report.IsOfflineSceneReady, Is.True);

        Scene scene = _loader.LoadedScene;
        OfflineMovement player = null;
        BaLangHuyenSceneController controller = null;
        foreach (GameObject root in scene.GetRootGameObjects())
        {
            if (player == null) player = root.GetComponentInChildren<OfflineMovement>();
            if (controller == null) controller = root.GetComponentInChildren<BaLangHuyenSceneController>();
        }

        Assert.That(player, Is.Not.Null);
        Assert.That(controller, Is.Not.Null);

        player.enabled = false;
        Physics.SyncTransforms();

        // 1. Walk along clear village road from (0, 0.04, -3) to (0, 0, 5)
        Vector3 roadTarget = new Vector3(0f, 0f, 5f);
        Vector2 screenRoad = player.ViewCamera.WorldToScreenPoint(roadTarget);
        Assert.That(player.TryPointer(screenRoad), Is.True, $"Failed road click: {player.LastInput}");
        for (int i = 0; i < 140 && player.IsMoving; i++)
        {
            player.Tick(0.02f);
            yield return null;
        }
        Assert.That(Vector3.Distance(new Vector3(player.transform.position.x, 0f, player.transform.position.z), roadTarget), Is.LessThan(0.35f));

        // 2. Walk toward West obstacle wall at (-5.5, 1.1, 2): look from North (Yaw=180, Pitch=60, Dist=16) so West (-X) projects to the RIGHT side of screen, completely clear of top-left UI shield
        CharacterController cc = player.GetComponent<CharacterController>();
        cc.enabled = false;
        player.transform.position = new Vector3(-2f, 0.05f, 2f);
        cc.enabled = true;
        controller.OrbitCamera.SetOrbit(180f, 60f, 16f);
        controller.OrbitCamera.ApplyPose();
        Physics.SyncTransforms();

        Vector3 behindWall = new Vector3(-8.5f, 0f, 2f);
        Vector2 screenBehindWall = player.ViewCamera.WorldToScreenPoint(behindWall);
        Assert.That(player.TryPointer(screenBehindWall), Is.True, $"Failed ground click behind wall: {player.LastInput} (screen={screenBehindWall})");
        for (int i = 0; i < 120 && player.IsMoving; i++)
        {
            player.Tick(0.02f);
            yield return null;
        }
        Assert.That(player.transform.position.x, Is.GreaterThan(-5.1f), "Player must be stopped by Obstacle_WestWall without penetrating x=-5.5");
        Assert.That(player.transform.position.x, Is.LessThan(-3.5f), "Player must advance toward Obstacle_WestWall before stopping");

        // 3. Camera bounds clamping check
        Vector3 clamped = controller.ClampToSceneBounds(new Vector3(500f, 100f, -500f));
        Assert.That(controller.CameraBounds.Contains(clamped), Is.True);
    }
}
