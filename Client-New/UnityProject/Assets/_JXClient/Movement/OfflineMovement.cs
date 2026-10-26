using System.Collections.Generic;
using UnityEngine;
using UnityEngine.EventSystems;
using UnityEngine.InputSystem;
using UnityEngine.UI;

namespace JXClient.Movement
{
    [RequireComponent(typeof(CharacterController))]
    public sealed class OfflineMovement : MonoBehaviour
    {
        public Camera ViewCamera;
        public LayerMask GroundMask = 1 << 8;
        [Min(.1f)] public float Speed = 4f;
        public OfflinePathfinder Pathfinder;
        public bool PathfindingEnabled = true;
        public bool UsePathfindingInManualTests = false;

        public int AcceptedIntents { get; private set; }
        public int RejectedIntents { get; private set; }
        public Vector3 Destination { get; private set; }
        public Vector3 FinalDestination { get; private set; }
        public bool IsMoving { get; private set; }
        public float ActualSpeed { get; private set; }
        public string MotionState { get; private set; } = "Idle";
        public string LastInput { get; private set; } = "Click the ground";
        public int CurrentWaypointIndex { get; private set; }
        public int WaypointCount => activeWaypoints.Count;
        public IReadOnlyList<Vector3> ActivePath => activeWaypoints;

        CharacterController controller;
        readonly List<RaycastResult> uiHits = new List<RaycastResult>();
        readonly List<Vector3> activeWaypoints = new List<Vector3>();

        void Awake()
        {
            controller = GetComponent<CharacterController>();
            if (!Pathfinder) Pathfinder = GetComponent<OfflinePathfinder>();
        }

        void Update()
        {
            if (Touchscreen.current != null && Touchscreen.current.primaryTouch.press.wasPressedThisFrame)
                TryPointer(Touchscreen.current.primaryTouch.position.ReadValue());
            else if (Mouse.current != null && Mouse.current.leftButton.wasPressedThisFrame)
                TryPointer(Mouse.current.position.ReadValue());
            Tick(Time.deltaTime);
        }

        public bool TryPointer(Vector2 screen)
        {
            if (!ViewCamera || !float.IsFinite(screen.x) || !float.IsFinite(screen.y) ||
                !ViewCamera.pixelRect.Contains(screen)) return Reject("Outside view");
            if (EventSystem.current != null)
            {
                uiHits.Clear();
                EventSystem.current.RaycastAll(new PointerEventData(EventSystem.current) { position = screen }, uiHits);
                foreach (var hit in uiHits)
                    if (hit.module is GraphicRaycaster) return Reject("UI consumed click");
            }
            // Inspect the nearest physical surface first: walls must not click through to ground.
            if (!Physics.Raycast(ViewCamera.ScreenPointToRay(screen), out RaycastHit ground, 500f,
                Physics.DefaultRaycastLayers, QueryTriggerInteraction.Ignore) ||
                (GroundMask.value & (1 << ground.collider.gameObject.layer)) == 0 || ground.normal.y < .5f)
                return Reject("No walkable ground");

            return AcceptOrRouteDestination(ground.point, screen, "pointer");
        }

        public bool TryMoveToWorld(Vector3 worldTarget)
        {
            if (!float.IsFinite(worldTarget.x) || !float.IsFinite(worldTarget.y) || !float.IsFinite(worldTarget.z))
                return Reject("Invalid world target");

            Ray downRay = new Ray(new Vector3(worldTarget.x, 35f, worldTarget.z), Vector3.down);
            if (!Physics.Raycast(downRay, out RaycastHit hit, 70f, Physics.DefaultRaycastLayers, QueryTriggerInteraction.Ignore) ||
                (GroundMask.value & (1 << hit.collider.gameObject.layer)) == 0 || hit.normal.y < .5f)
                return Reject("No walkable ground");

            return AcceptOrRouteDestination(hit.point, Vector2.zero, "world-intent");
        }

        bool AcceptOrRouteDestination(Vector3 targetPoint, Vector2 screen, string source)
        {
            if (!controller) controller = GetComponent<CharacterController>();
            if (!Pathfinder) Pathfinder = GetComponent<OfflinePathfinder>();

            bool usePathfinder = PathfindingEnabled && Pathfinder != null && (enabled || UsePathfindingInManualTests);
            if (usePathfinder)
            {
                if (!Pathfinder.TryBuildPath(transform.position, targetPoint, controller, GroundMask, out List<Vector3> path, out string reason))
                {
                    IsMoving = false;
                    activeWaypoints.Clear();
                    CurrentWaypointIndex = 0;
                    MotionState = "Unreachable";
                    return Reject(reason);
                }

                activeWaypoints.Clear();
                activeWaypoints.AddRange(path);
                CurrentWaypointIndex = 0;
                Destination = activeWaypoints[0];
                FinalDestination = targetPoint;
                AcceptedIntents++;
                IsMoving = true;
                LastInput = reason;
                Debug.Log($"[CN-01] INPUT accepted={AcceptedIntents} screen={screen} destination={FinalDestination} waypoints={activeWaypoints.Count} source={source}");
                return true;
            }

            activeWaypoints.Clear();
            activeWaypoints.Add(targetPoint);
            CurrentWaypointIndex = 0;
            Destination = targetPoint;
            FinalDestination = targetPoint;
            AcceptedIntents++;
            IsMoving = true;
            LastInput = "Ground destination accepted";
            Debug.Log($"[CN-01] INPUT accepted={AcceptedIntents} screen={screen} destination={Destination} source={source}");
            return true;
        }

        bool Reject(string reason)
        {
            RejectedIntents++;
            LastInput = reason;
            return false;
        }

        public void Tick(float deltaTime)
        {
            ActualSpeed = 0;
            if (!IsMoving || !float.IsFinite(deltaTime) || deltaTime <= 0)
            {
                if (MotionState != "Unreachable" && MotionState != "Blocked" && MotionState != "No ground")
                    MotionState = "Idle";
                else if (!IsMoving && MotionState != "Unreachable")
                    MotionState = "Idle";
                return;
            }
            if (!controller) controller = GetComponent<CharacterController>();
            // Own the controller threshold: tiny valid frame steps must accumulate as real movement.
            controller.minMoveDistance = 0;
            Vector3 delta = Destination - transform.position; delta.y = 0;
            while (delta.magnitude <= .08f)
            {
                if (activeWaypoints.Count > 0 && CurrentWaypointIndex + 1 < activeWaypoints.Count)
                {
                    CurrentWaypointIndex++;
                    Destination = activeWaypoints[CurrentWaypointIndex];
                    delta = Destination - transform.position;
                    delta.y = 0;
                }
                else
                {
                    IsMoving = false;
                    MotionState = "Idle";
                    return;
                }
            }
            float dt = Mathf.Min(deltaTime, .1f);
            Vector3 step = Vector3.ClampMagnitude(delta, Speed * dt);
            Vector3 next = transform.position + step;
            if (!Physics.Raycast(next + Vector3.up, Vector3.down, out RaycastHit support, 3f,
                GroundMask, QueryTriggerInteraction.Ignore) || support.normal.y < .5f)
            { IsMoving = false; MotionState = "No ground"; return; }
            Vector3 before = transform.position;
            controller.Move(step + Vector3.down * (10f * dt));
            Vector3 actual = transform.position - before; actual.y = 0;
            ActualSpeed = actual.magnitude / dt;
            if (actual.sqrMagnitude > .000001f) transform.rotation = Quaternion.LookRotation(actual);
            MotionState = ActualSpeed > .05f ? "Walking" : "Blocked";
        }
    }
}
