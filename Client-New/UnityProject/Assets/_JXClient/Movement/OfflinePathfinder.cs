using System;
using System.Collections.Generic;
using UnityEngine;

namespace JXClient.Movement
{
    /// <summary>
    /// Deterministic offline 2D grid A* pathfinder with line-of-sight smoothing over Unity physics colliders.
    /// Routes around walls, houses, gates, and obstacles on Layer 8 (Ground) and rejects unreachable targets.
    /// Strictly offline: never emits or simulates server-authoritative MoveIntent network packets.
    /// </summary>
    public sealed class OfflinePathfinder : MonoBehaviour
    {
        public Vector3 GridCenter = Vector3.zero;
        public Vector2 GridSize = new Vector2(56f, 56f);
        [Min(0.25f)] public float CellSize = 0.5f;
        [Min(0.2f)] public float ClearanceRadius = 0.52f;
        [Min(0.5f)] public float CapsuleHeight = 1.9f;
        public int MaxSearchIterations = 16000;

        private static readonly int[] NeighborDx = { 0, 1, 0, -1, 1, 1, -1, -1 };
        private static readonly int[] NeighborDz = { 1, 0, -1, 0, 1, -1, 1, -1 };
        private static readonly float[] NeighborCost = { 1f, 1f, 1f, 1f, 1.41421356f, 1.41421356f, 1.41421356f, 1.41421356f };

        private readonly Collider[] _overlapBuffer = new Collider[16];

        public bool TrySampleWalkableGround(Vector3 worldPos, LayerMask groundMask, out Vector3 groundPoint)
        {
            groundPoint = Vector3.zero;
            if (!float.IsFinite(worldPos.x) || !float.IsFinite(worldPos.y) || !float.IsFinite(worldPos.z))
                return false;

            Ray ray = new Ray(new Vector3(worldPos.x, 35f, worldPos.z), Vector3.down);
            if (!Physics.Raycast(ray, out RaycastHit hit, 70f, groundMask, QueryTriggerInteraction.Ignore))
                return false;

            if (hit.normal.y < 0.5f)
                return false;

            groundPoint = hit.point;
            return true;
        }

        public bool IsPointWalkable(Vector3 worldPos, float clearanceRadius, CharacterController ignoreController, LayerMask groundMask, out Vector3 groundPoint)
        {
            if (!TrySampleWalkableGround(worldPos, groundMask, out groundPoint))
                return false;

            float r = Mathf.Max(0.2f, clearanceRadius);
            Vector3 bottom = groundPoint + Vector3.up * (r + 0.15f);
            Vector3 top = groundPoint + Vector3.up * Mathf.Max(r + 0.2f, CapsuleHeight - r);

            int count = Physics.OverlapCapsuleNonAlloc(
                bottom,
                top,
                r,
                _overlapBuffer,
                Physics.DefaultRaycastLayers,
                QueryTriggerInteraction.Ignore);

            for (int i = 0; i < count; i++)
            {
                Collider col = _overlapBuffer[i];
                if (col == null || !col.enabled || col.isTrigger) continue;
                if (ignoreController != null && (col == ignoreController || col.transform.IsChildOf(ignoreController.transform))) continue;
                if ((groundMask.value & (1 << col.gameObject.layer)) != 0) continue;
                return false;
            }

            return true;
        }

        public bool HasDirectWalkableLine(Vector3 start, Vector3 end, float clearanceRadius, CharacterController ignoreController, LayerMask groundMask)
        {
            Vector3 flatDelta = new Vector3(end.x - start.x, 0f, end.z - start.z);
            float distance = flatDelta.magnitude;
            if (distance <= 0.05f) return true;

            int steps = Mathf.Max(2, Mathf.CeilToInt(distance / 0.22f));
            for (int i = 1; i <= steps; i++)
            {
                float t = (float)i / steps;
                Vector3 sample = Vector3.Lerp(start, end, t);
                if (!IsPointWalkable(sample, clearanceRadius, ignoreController, groundMask, out _))
                    return false;
            }
            return true;
        }

        public bool TryBuildPath(
            Vector3 startWorld,
            Vector3 goalWorld,
            CharacterController ignoreController,
            LayerMask groundMask,
            out List<Vector3> waypoints,
            out string reason)
        {
            waypoints = new List<Vector3>();
            Physics.SyncTransforms();

            if (!TrySampleWalkableGround(goalWorld, groundMask, out Vector3 goalGround))
            {
                reason = "No walkable ground";
                return false;
            }

            if (!IsPointWalkable(goalGround, ClearanceRadius * 0.85f, ignoreController, groundMask, out goalGround))
            {
                reason = "Unreachable target";
                return false;
            }

            if (HasDirectWalkableLine(startWorld, goalGround, ClearanceRadius, ignoreController, groundMask))
            {
                waypoints.Add(goalGround);
                reason = "Ground destination accepted";
                return true;
            }

            int width = Mathf.Max(8, Mathf.RoundToInt(GridSize.x / CellSize));
            int height = Mathf.Max(8, Mathf.RoundToInt(GridSize.y / CellSize));
            float minX = GridCenter.x - GridSize.x * 0.5f;
            float minZ = GridCenter.z - GridSize.y * 0.5f;

            Vector2Int startCell = WorldToCell(startWorld, minX, minZ, width, height);
            Vector2Int goalCell = WorldToCell(goalGround, minX, minZ, width, height);

            byte[,] walkState = new byte[width, height]; // 0 = unknown, 1 = walkable, 2 = blocked
            Vector3[,] groundPoints = new Vector3[width, height];

            bool EvalCell(int cx, int cz)
            {
                if (cx < 0 || cx >= width || cz < 0 || cz >= height) return false;
                byte s = walkState[cx, cz];
                if (s != 0) return s == 1;

                Vector3 world = CellToWorld(cx, cz, minX, minZ, startWorld.y);
                if (IsPointWalkable(world, ClearanceRadius, ignoreController, groundMask, out Vector3 gp))
                {
                    walkState[cx, cz] = 1;
                    groundPoints[cx, cz] = gp;
                    return true;
                }
                walkState[cx, cz] = 2;
                return false;
            }

            if (!EvalCell(startCell.x, startCell.y))
            {
                if (!TryFindNearestWalkableCell(startCell, 3, EvalCell, out startCell))
                {
                    reason = "Start position blocked";
                    return false;
                }
            }

            if (!EvalCell(goalCell.x, goalCell.y))
            {
                if (!TryFindNearestWalkableCell(goalCell, 2, EvalCell, out goalCell))
                {
                    reason = "Unreachable target";
                    return false;
                }
            }

            int totalCells = width * height;
            float[] gScore = new float[totalCells];
            int[] cameFrom = new int[totalCells];
            bool[] closed = new bool[totalCells];
            for (int i = 0; i < totalCells; i++)
            {
                gScore[i] = float.PositiveInfinity;
                cameFrom[i] = -1;
            }

            int startIdx = startCell.y * width + startCell.x;
            int goalIdx = goalCell.y * width + goalCell.x;
            gScore[startIdx] = 0f;

            var openList = new List<(int idx, float f)>(128)
            {
                (startIdx, Heuristic(startCell.x, startCell.y, goalCell.x, goalCell.y))
            };

            bool found = false;
            int iterations = 0;

            while (openList.Count > 0 && iterations < MaxSearchIterations)
            {
                iterations++;
                int bestPos = 0;
                float bestF = openList[0].f;
                for (int i = 1; i < openList.Count; i++)
                {
                    if (openList[i].f < bestF)
                    {
                        bestF = openList[i].f;
                        bestPos = i;
                    }
                }

                int currentIdx = openList[bestPos].idx;
                openList.RemoveAt(bestPos);

                if (closed[currentIdx]) continue;
                closed[currentIdx] = true;

                if (currentIdx == goalIdx)
                {
                    found = true;
                    break;
                }

                int cx = currentIdx % width;
                int cz = currentIdx / width;
                float currentG = gScore[currentIdx];

                for (int dir = 0; dir < 8; dir++)
                {
                    int nx = cx + NeighborDx[dir];
                    int nz = cz + NeighborDz[dir];
                    if (nx < 0 || nx >= width || nz < 0 || nz >= height) continue;

                    int nIdx = nz * width + nx;
                    if (closed[nIdx]) continue;
                    if (!EvalCell(nx, nz)) continue;

                    // Prevent diagonal corner-cutting across obstacle edges
                    if (dir >= 4)
                    {
                        if (!EvalCell(cx + NeighborDx[dir], cz) || !EvalCell(cx, cz + NeighborDz[dir]))
                            continue;
                    }

                    float tentativeG = currentG + NeighborCost[dir];
                    if (tentativeG < gScore[nIdx])
                    {
                        cameFrom[nIdx] = currentIdx;
                        gScore[nIdx] = tentativeG;
                        float f = tentativeG + Heuristic(nx, nz, goalCell.x, goalCell.y);
                        openList.Add((nIdx, f));
                    }
                }
            }

            if (!found)
            {
                reason = "Unreachable target";
                return false;
            }

            var rawPoints = new List<Vector3>();
            int trace = goalIdx;
            while (trace != -1 && trace != startIdx)
            {
                int tx = trace % width;
                int tz = trace / width;
                rawPoints.Add(groundPoints[tx, tz]);
                trace = cameFrom[trace];
            }
            rawPoints.Reverse();
            if (rawPoints.Count == 0 || Vector3.Distance(rawPoints[rawPoints.Count - 1], goalGround) > 0.1f)
            {
                rawPoints.Add(goalGround);
            }
            else
            {
                rawPoints[rawPoints.Count - 1] = goalGround;
            }

            waypoints = SmoothWaypoints(startWorld, rawPoints, ignoreController, groundMask);
            reason = $"Ground path accepted ({waypoints.Count} waypoints)";
            return true;
        }

        private List<Vector3> SmoothWaypoints(
            Vector3 startWorld,
            List<Vector3> rawPoints,
            CharacterController ignoreController,
            LayerMask groundMask)
        {
            if (rawPoints.Count <= 1) return rawPoints;

            var smoothed = new List<Vector3>();
            Vector3 anchor = startWorld;
            int index = 0;
            float smoothClearance = ClearanceRadius + 0.08f;

            while (index < rawPoints.Count)
            {
                int furthestVisible = index;
                for (int probe = rawPoints.Count - 1; probe > index; probe--)
                {
                    if (HasDirectWalkableLine(anchor, rawPoints[probe], smoothClearance, ignoreController, groundMask))
                    {
                        furthestVisible = probe;
                        break;
                    }
                }

                smoothed.Add(rawPoints[furthestVisible]);
                anchor = rawPoints[furthestVisible];
                index = furthestVisible + 1;
            }

            return smoothed;
        }

        private static bool TryFindNearestWalkableCell(Vector2Int origin, int maxRadius, Func<int, int, bool> eval, out Vector2Int result)
        {
            for (int r = 1; r <= maxRadius; r++)
            {
                for (int dz = -r; dz <= r; dz++)
                {
                    for (int dx = -r; dx <= r; dx++)
                    {
                        if (Mathf.Abs(dx) != r && Mathf.Abs(dz) != r) continue;
                        if (eval(origin.x + dx, origin.y + dz))
                        {
                            result = new Vector2Int(origin.x + dx, origin.y + dz);
                            return true;
                        }
                    }
                }
            }
            result = origin;
            return false;
        }

        private Vector2Int WorldToCell(Vector3 world, float minX, float minZ, int width, int height)
        {
            int cx = Mathf.Clamp(Mathf.FloorToInt((world.x - minX) / CellSize), 0, width - 1);
            int cz = Mathf.Clamp(Mathf.FloorToInt((world.z - minZ) / CellSize), 0, height - 1);
            return new Vector2Int(cx, cz);
        }

        private Vector3 CellToWorld(int cx, int cz, float minX, float minZ, float y)
        {
            return new Vector3(
                minX + (cx + 0.5f) * CellSize,
                y,
                minZ + (cz + 0.5f) * CellSize);
        }

        private static float Heuristic(int x0, int z0, int x1, int z1)
        {
            int dx = Mathf.Abs(x1 - x0);
            int dz = Mathf.Abs(z1 - z0);
            int min = Mathf.Min(dx, dz);
            int max = Mathf.Max(dx, dz);
            return 1.41421356f * min + (max - min);
        }
    }
}
