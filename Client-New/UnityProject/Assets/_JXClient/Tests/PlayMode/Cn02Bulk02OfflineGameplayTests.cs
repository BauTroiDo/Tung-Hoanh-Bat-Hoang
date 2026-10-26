using System.Collections;
using JXClient.Domain;
using JXClient.Movement;
using JXClient.World;
using NUnit.Framework;
using UnityEngine;
using UnityEngine.SceneManagement;
using UnityEngine.TestTools;

public class Cn02Bulk02OfflineGameplayTests
{
    private MapSceneLoader _loader;
    private Scene _scene;
    private OfflineMovement _player;
    private BaLangHuyenSceneController _controller;

    [UnitySetUp]
    public IEnumerator SetUp()
    {
        _loader = new MapSceneLoader();
        WorldSceneValidationReport report = null;
        yield return _loader.LoadMapRoutine(MapId.BaLangHuyen, LoadSceneMode.Additive, r => report = r);

        Assert.That(report, Is.Not.Null);
        Assert.That(report.IsOfflineSceneReady, Is.True);

        _scene = _loader.LoadedScene;
        foreach (GameObject root in _scene.GetRootGameObjects())
        {
            if (_player == null) _player = root.GetComponentInChildren<OfflineMovement>();
            if (_controller == null) _controller = root.GetComponentInChildren<BaLangHuyenSceneController>();
        }

        Assert.That(_player, Is.Not.Null);
        Assert.That(_controller, Is.Not.Null);

        _controller.EnsureBulk02SceneEnhancements();
        _player.enabled = false;
        _player.UsePathfindingInManualTests = true;
        Physics.SyncTransforms();
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

    private void TeleportPlayer(Vector3 pos)
    {
        CharacterController cc = _player.GetComponent<CharacterController>();
        if (cc != null) cc.enabled = false;
        _player.transform.position = pos;
        if (cc != null) cc.enabled = true;
        Physics.SyncTransforms();
    }

    [UnityTest]
    public IEnumerator BaLangHuyen_MultiPointGroundAndObstacleColliders_AreWalkableAndSolidAcrossMap()
    {
        Vector3[] samplePositions =
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

        foreach (Vector3 pos in samplePositions)
        {
            bool hit = WorldSceneValidator.TrySampleGroundHeight(_scene, pos, 8, out Vector3 groundPt, out RaycastHit hitInfo);
            Assert.That(hit, Is.True, $"Expected Layer-8 walkable ground at {pos}");
            Assert.That(hitInfo.normal.y, Is.GreaterThanOrEqualTo(0.5f));
            Assert.That(groundPt.y, Is.InRange(-0.1f, 0.2f));
        }

        // Verify obstacle colliders block walkability at house/wall centers
        CharacterController cc = _player.GetComponent<CharacterController>();
        Assert.That(_player.Pathfinder, Is.Not.Null);
        Assert.That(_player.Pathfinder.IsPointWalkable(new Vector3(-5.5f, 0f, 2f), 0.5f, cc, _player.GroundMask, out _), Is.False, "Obstacle_WestWall center must not be walkable");
        Assert.That(_player.Pathfinder.IsPointWalkable(new Vector3(4.5f, 0f, -3.5f), 0.5f, cc, _player.GroundMask, out _), Is.False, "Obstacle_EastLanternPillar center must not be walkable");
        Assert.That(_player.Pathfinder.IsPointWalkable(new Vector3(-11f, 0f, 10f), 0.5f, cc, _player.GroundMask, out _), Is.False, "VillageHouse_1 center must not be walkable");
        yield return null;
    }

    [UnityTest]
    public IEnumerator BaLangHuyen_OfflinePathfinder_RoutesAroundWestWallAndEastPillarAndVillageHouse()
    {
        // 1. Route around Obstacle_WestWall (-5.5, 1.1, 2) from (-2, 0.05, 2) to (-8.5, 0, 2)
        TeleportPlayer(new Vector3(-2f, 0.05f, 2f));
        Vector3 targetBehindWestWall = new Vector3(-8.5f, 0f, 2f);
        Assert.That(_player.TryMoveToWorld(targetBehindWestWall), Is.True, $"Expected A* path around WestWall: {_player.LastInput}");
        Assert.That(_player.WaypointCount, Is.GreaterThanOrEqualTo(2), "Path around Obstacle_WestWall must use multiple waypoints");

        bool observedWalking = false;
        Bounds westWallForbidden = new Bounds(new Vector3(-5.5f, 1.1f, 2f), new Vector3(1.1f, 2.0f, 5.9f));
        for (int i = 0; i < 320 && _player.IsMoving; i++)
        {
            _player.Tick(0.02f);
            if (_player.MotionState == "Walking") observedWalking = true;
            Assert.That(westWallForbidden.Contains(_player.transform.position), Is.False, $"Player penetrated Obstacle_WestWall at {_player.transform.position}");
        }
        yield return null;

        Assert.That(observedWalking, Is.True);
        Assert.That(_player.IsMoving, Is.False);
        Assert.That(_player.MotionState, Is.EqualTo("Idle"));
        float distWest = Vector2.Distance(
            new Vector2(_player.transform.position.x, _player.transform.position.z),
            new Vector2(targetBehindWestWall.x, targetBehindWestWall.z));
        Assert.That(distWest, Is.LessThan(0.35f), $"Expected player to reach {targetBehindWestWall}, ended at {_player.transform.position}");

        // 2. Route around Obstacle_EastLanternPillar (4.5, 1.2, -3.5) from (2, 0.05, -3.5) to (7.5, 0, -3.5)
        TeleportPlayer(new Vector3(2f, 0.05f, -3.5f));
        Vector3 targetBehindPillar = new Vector3(7.5f, 0f, -3.5f);
        Assert.That(_player.TryMoveToWorld(targetBehindPillar), Is.True, $"Expected A* path around EastLanternPillar: {_player.LastInput}");
        Assert.That(_player.WaypointCount, Is.GreaterThanOrEqualTo(2));

        for (int i = 0; i < 260 && _player.IsMoving; i++)
        {
            _player.Tick(0.02f);
        }
        yield return null;

        float distPillar = Vector2.Distance(
            new Vector2(_player.transform.position.x, _player.transform.position.z),
            new Vector2(targetBehindPillar.x, targetBehindPillar.z));
        Assert.That(distPillar, Is.LessThan(0.35f), $"Expected player to reach {targetBehindPillar}, ended at {_player.transform.position}");

        // 3. Route around VillageHouse_1 (-11, 0, 10) from (-7.5, 0.05, 10) to (-14.5, 0, 10)
        TeleportPlayer(new Vector3(-7.5f, 0.05f, 10f));
        Vector3 targetBehindHouse = new Vector3(-14.5f, 0f, 10f);
        Assert.That(_player.TryMoveToWorld(targetBehindHouse), Is.True, $"Expected A* path around VillageHouse_1: {_player.LastInput}");
        Assert.That(_player.WaypointCount, Is.GreaterThanOrEqualTo(2));

        for (int i = 0; i < 360 && _player.IsMoving; i++)
        {
            _player.Tick(0.02f);
        }
        yield return null;

        float distHouse = Vector2.Distance(
            new Vector2(_player.transform.position.x, _player.transform.position.z),
            new Vector2(targetBehindHouse.x, targetBehindHouse.z));
        Assert.That(distHouse, Is.LessThan(0.35f), $"Expected player to reach {targetBehindHouse}, ended at {_player.transform.position}");
    }

    [UnityTest]
    public IEnumerator BaLangHuyen_UnreachableTargets_InsideHouseAndEnclosedCourtyard_AreRejected()
    {
        TeleportPlayer(new Vector3(8f, 0.05f, -12f));
        Vector3 startPos = _player.transform.position;

        // 1. Target inside enclosed stone courtyard (16, 0, -16) has walkable Layer-8 ground but no walkable path
        bool acceptedCourtyard = _player.TryMoveToWorld(BaLangHuyenSceneController.UnreachableCourtyardCenter);
        Assert.That(acceptedCourtyard, Is.False, "Enclosed courtyard target must be rejected as unreachable");
        Assert.That(_player.IsMoving, Is.False);
        Assert.That(_player.MotionState, Is.EqualTo("Unreachable"));
        Assert.That(_player.LastInput, Is.EqualTo("Unreachable target"));
        Assert.That(Vector3.Distance(_player.transform.position, startPos), Is.LessThan(0.01f));

        // 2. Also test via screen-space TryPointer on enclosed courtyard ground (Yaw=0 projects (16,0,-16) to bottom-right, clear of top-left UI shield)
        _controller.OrbitCamera.SetOrbit(0f, 65f, 22f);
        _controller.OrbitCamera.ApplyPose();
        Physics.SyncTransforms();
        yield return null;

        Vector2 screenCourtyard = _player.ViewCamera.WorldToScreenPoint(BaLangHuyenSceneController.UnreachableCourtyardCenter);
        bool pointerCourtyard = _player.TryPointer(screenCourtyard);
        Assert.That(pointerCourtyard, Is.False);
        Assert.That(_player.MotionState, Is.EqualTo("Unreachable"));
        Assert.That(_player.LastInput, Is.EqualTo("Unreachable target"));

        // 3. Target directly inside solid VillageHouse_2 (11, 0, 10)
        bool acceptedInsideHouse = _player.TryMoveToWorld(new Vector3(11f, 0f, 10f));
        Assert.That(acceptedInsideHouse, Is.False);
        Assert.That(_player.IsMoving, Is.False);
    }

    [UnityTest]
    public IEnumerator BaLangHuyen_CameraFourDirectionsAndZoom_MaintainBoundsAndValidGroundClicks()
    {
        TeleportPlayer(new Vector3(0f, 0.05f, 0f));
        float[] yaws = { 0f, 90f, 180f, 270f };
        float[] pitches = { 35f, 45f, 60f, 70f };
        float[] distances = { 10f, 14f, 18f, 22f };
        Vector3[] clickTargets =
        {
            new Vector3(3f, 0f, -2f),
            new Vector3(-2f, 0f, -3f),
            new Vector3(-3f, 0f, 2f),
            new Vector3(2f, 0f, 3f)
        };

        for (int i = 0; i < 4; i++)
        {
            _controller.OrbitCamera.SetOrbit(yaws[i], pitches[i], distances[i]);
            _controller.OrbitCamera.ApplyPose();
            Vector3 clampedCam = _controller.ClampToSceneBounds(_controller.OrbitCamera.transform.position);
            Assert.That(_controller.CameraBounds.Contains(clampedCam), Is.True);
            Physics.SyncTransforms();
            yield return null;

            Vector2 screenPt = _player.ViewCamera.WorldToScreenPoint(clickTargets[i]);
            Assert.That(_player.TryPointer(screenPt), Is.True, $"Failed ground click at yaw={yaws[i]}: {_player.LastInput}");
            for (int step = 0; step < 80 && _player.IsMoving; step++)
            {
                _player.Tick(0.02f);
            }
            float dist = Vector2.Distance(
                new Vector2(_player.transform.position.x, _player.transform.position.z),
                new Vector2(clickTargets[i].x, clickTargets[i].z));
            Assert.That(dist, Is.LessThan(0.35f));
        }

        // Verifyoffline session state never leaks into network WorldReady
        WorldSceneValidationReport val = _controller.RefreshValidation();
        Assert.That(val.SessionState, Is.EqualTo(GameSessionState.OfflineReady));
        Assert.That(GameSessionRules.CanSendMoveRequest(val.SessionState), Is.False);
    }
}
