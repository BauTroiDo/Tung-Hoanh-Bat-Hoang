using System;
using System.Collections.Generic;
using JXClient.Domain;
using UnityEngine;

namespace JXClient.World
{
    public enum MapCalibrationStatus
    {
        Unverified = 0,
        SyntheticTestOnly = 1,
        Verified = 2
    }

    public readonly struct GroundProbeDefinition
    {
        public string Name { get; }
        public Vector3 HorizontalPosition { get; }

        public GroundProbeDefinition(string name, Vector3 horizontalPosition)
        {
            if (string.IsNullOrWhiteSpace(name))
                throw new ArgumentException("Probe name is required.", nameof(name));
            Name = name;
            HorizontalPosition = horizontalPosition;
        }
    }

    public sealed class MapSceneDefinition
    {
        public MapId MapId { get; }
        public string SceneName { get; }
        public string ScenePath { get; }
        public string DisplayTitle { get; }
        public string ProvenanceVersion { get; }
        public bool IsSyntheticFallback { get; }
        public MapCalibrationStatus CalibrationStatus { get; }
        public int GroundLayer { get; }
        public string OfflineSpawnMarkerName { get; }
        public Bounds CameraBounds { get; }
        public IReadOnlyList<GroundProbeDefinition> GroundProbes { get; }

        public MapSceneDefinition(
            MapId mapId,
            string sceneName,
            string scenePath,
            string displayTitle,
            string provenanceVersion,
            bool isSyntheticFallback,
            MapCalibrationStatus calibrationStatus,
            int groundLayer,
            string offlineSpawnMarkerName,
            Bounds cameraBounds,
            IReadOnlyList<GroundProbeDefinition> groundProbes)
        {
            MapId = mapId;
            SceneName = sceneName ?? throw new ArgumentNullException(nameof(sceneName));
            ScenePath = scenePath ?? throw new ArgumentNullException(nameof(scenePath));
            DisplayTitle = displayTitle ?? throw new ArgumentNullException(nameof(displayTitle));
            ProvenanceVersion = provenanceVersion ?? throw new ArgumentNullException(nameof(provenanceVersion));
            IsSyntheticFallback = isSyntheticFallback;
            CalibrationStatus = calibrationStatus;
            GroundLayer = groundLayer;
            OfflineSpawnMarkerName = offlineSpawnMarkerName ?? throw new ArgumentNullException(nameof(offlineSpawnMarkerName));
            CameraBounds = cameraBounds;
            GroundProbes = groundProbes ?? throw new ArgumentNullException(nameof(groundProbes));
        }
    }

    public static class MapSceneCatalog
    {
        public static readonly MapSceneDefinition BaLangHuyen = new MapSceneDefinition(
            mapId: MapId.BaLangHuyen,
            sceneName: "BaLangHuyen",
            scenePath: "Assets/_JXClient/Scenes/BaLangHuyen.unity",
            displayTitle: "Ba Lang Huyen (MapId=53 | SYNTHETIC FALLBACK — NOT ORIGINAL BLH RESTORATION)",
            provenanceVersion: "cn02-synthetic-v1",
            isSyntheticFallback: true,
            calibrationStatus: MapCalibrationStatus.Unverified,
            groundLayer: 8,
            offlineSpawnMarkerName: "OfflineSpawn_VillageCenter",
            cameraBounds: new Bounds(new Vector3(0f, 4f, 0f), new Vector3(56f, 20f, 56f)),
            groundProbes: new[]
            {
                new GroundProbeDefinition("Probe_CenterPlaza", new Vector3(0f, 0f, 0f)),
                new GroundProbeDefinition("Probe_NorthGatePath", new Vector3(0f, 0f, 18f)),
                new GroundProbeDefinition("Probe_SouthGatePath", new Vector3(0f, 0f, -18f)),
                new GroundProbeDefinition("Probe_EastBridgeApproach", new Vector3(18f, 0f, 0f)),
                new GroundProbeDefinition("Probe_WestMarketApproach", new Vector3(-18f, 0f, 0f))
            });

        private static readonly Dictionary<int, MapSceneDefinition> ById = new Dictionary<int, MapSceneDefinition>
        {
            { MapId.BaLangHuyen.Value, BaLangHuyen }
        };

        public static bool TryGet(MapId mapId, out MapSceneDefinition definition)
        {
            return ById.TryGetValue(mapId.Value, out definition);
        }
    }
}
