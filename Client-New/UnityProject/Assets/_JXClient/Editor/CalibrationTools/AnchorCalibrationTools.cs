using System;
using System.IO;
using System.Security.Cryptography;
using JXClient.Domain.Coordinates;
using JXClient.World;
using UnityEditor;
using UnityEditor.SceneManagement;
using UnityEngine;
using UnityEngine.SceneManagement;

namespace JXClient.Editor.Calibration
{
    [Serializable] public sealed class AnchorDraftRow
    {
        public string label;
        public double sceneX, sceneZ, legacyX, legacyY;
        public bool hasLegacy;
        public AnchorEvidenceKind evidenceKind;
        public string evidencePath = "", evidenceSha256 = "";
    }
    [Serializable] public sealed class AnchorCalibrationDocument
    {
        public int schemaVersion = 1, mapId = 53;
        public string scenePath, sceneSha256, sceneProvenance, referenceBuild = "", createdUtc;
        public double legacyMinX, legacyMinY, legacyMaxX, legacyMaxY;
        public double sceneMinX, sceneMinZ, sceneMaxX, sceneMaxZ, tolerance;
        public AnchorDraftRow[] anchors;
        public string status, message;
        public bool hasResidual, independentReviewRequired = true;
        public double holdoutError;
    }
    public static class AnchorCalibrationTools
    {
        public const string ScenePath = "Assets/_JXClient/Scenes/BaLangHuyen.unity";
        static readonly string[] MarkerNames = { "AnchorCandidate_CenterPlaza", "AnchorCandidate_NorthGate", "AnchorCandidate_EastRoad", "AnchorCandidate_SouthWestCorner" };
        public static AnchorCalibrationDocument CaptureDraft()
        {
            Scene scene = SceneManager.GetSceneByPath(ScenePath);
            bool opened = !scene.IsValid() || !scene.isLoaded;
            if (opened) scene = EditorSceneManager.OpenScene(ScenePath, OpenSceneMode.Additive);
            try
            {
                if (scene.isDirty) throw new InvalidOperationException("Save the scene before capturing its marker fingerprint.");
                Transform spawn = null;
                foreach (var root in scene.GetRootGameObjects()) if (root.name == "SpawnPoints") spawn = root.transform;
                if (!spawn) throw new InvalidDataException("SpawnPoints root not found.");
                var bounds = MapSceneCatalog.BaLangHuyen.CameraBounds;
                var document = new AnchorCalibrationDocument {
                    scenePath = ScenePath, sceneSha256 = HashFile(ScenePath),
                    sceneProvenance = MapSceneCatalog.BaLangHuyen.ProvenanceVersion,
                    sceneMinX = bounds.min.x, sceneMinZ = bounds.min.z,
                    sceneMaxX = bounds.max.x, sceneMaxZ = bounds.max.z,
                    createdUtc = DateTime.UtcNow.ToString("o"), anchors = new AnchorDraftRow[4]
                };
                for (int i = 0; i < MarkerNames.Length; i++)
                {
                    Transform marker = spawn.Find(MarkerNames[i]);
                    if (!marker) throw new InvalidDataException("Missing marker: " + MarkerNames[i]);
                    document.anchors[i] = new AnchorDraftRow {
                        label = MarkerNames[i], sceneX = marker.position.x, sceneZ = marker.position.z,
                        hasLegacy = false, evidenceKind = AnchorEvidenceKind.Unknown
                    };
                }
                RefreshAssessment(document);
                return document;
            }
            finally { if (opened) EditorSceneManager.CloseScene(scene, true); }
        }
        public static string HashFile(string path)
        {
            using (var stream = File.OpenRead(path))
            using (var sha = SHA256.Create()) return BitConverter.ToString(sha.ComputeHash(stream)).Replace("-", "");
        }
        public static void RefreshAssessment(AnchorCalibrationDocument document)
        {
            document.independentReviewRequired = true;
            document.hasResidual = false;
            document.holdoutError = 0;
            if (document.anchors == null || document.anchors.Length != 4) { Set(document, "InvalidInput", "Four observations are required."); return; }
            foreach (var row in document.anchors)
                if (row == null || !row.hasLegacy) { Set(document, "DRAFT_UNPAIRED", "Scene points captured; legacy observations/evidence have not been entered."); return; }
            try
            {
                var rows = new AnchorObservation[4];
                for (int i = 0; i < rows.Length; i++)
                {
                    var row = document.anchors[i];
                    var kind = row.evidenceKind;
                    // Provenance claims alone cannot bypass actual file integrity or a missing reference build.
                    if (kind == AnchorEvidenceKind.MeasuredReference &&
                        (string.IsNullOrWhiteSpace(document.referenceBuild) || !EvidenceMatches(row))) kind = AnchorEvidenceKind.Unknown;
                    rows[i] = new AnchorObservation(row.label,
                        new CoordinateAnchor(new PlanarPoint(row.legacyX, row.legacyY), new PlanarPoint(row.sceneX, row.sceneZ)),
                        kind, row.evidencePath, row.evidenceSha256);
                }
                var result = CalibrationEvidence.Assess(rows,
                    new PlanarBounds(document.legacyMinX, document.legacyMinY, document.legacyMaxX, document.legacyMaxY),
                    new PlanarBounds(document.sceneMinX, document.sceneMinZ, document.sceneMaxX, document.sceneMaxZ), document.tolerance);
                Set(document, result.Kind.ToString(), result.Message);
                document.hasResidual = result.HoldoutError.HasValue;
                if (result.HoldoutError.HasValue) document.holdoutError = result.HoldoutError.Value;
            }
            catch (ArgumentException ex) { Set(document, "InvalidInput", ex.Message); }
        }
        static bool EvidenceMatches(AnchorDraftRow row)
        {
            try {
                return !string.IsNullOrWhiteSpace(row.evidencePath) && File.Exists(row.evidencePath) &&
                    new FileInfo(row.evidencePath).Length <= 16 * 1024 * 1024 &&
                    string.Equals(HashFile(row.evidencePath), row.evidenceSha256, StringComparison.OrdinalIgnoreCase);
            }
            catch (Exception ex) when (ex is IOException || ex is UnauthorizedAccessException || ex is ArgumentException) { return false; }
        }
        static void Set(AnchorCalibrationDocument document, string status, string message) { document.status = status; document.message = message; }
        static void ValidateIdentity(AnchorCalibrationDocument document)
        {
            if (document == null || document.schemaVersion != 1 || document.mapId != 53 || document.scenePath != ScenePath ||
                document.sceneProvenance != MapSceneCatalog.BaLangHuyen.ProvenanceVersion ||
                !string.Equals(document.sceneSha256, HashFile(ScenePath), StringComparison.OrdinalIgnoreCase))
                throw new InvalidDataException("Draft scene/build fingerprint is stale or incompatible; capture current markers again.");
            if (document.anchors == null || document.anchors.Length != 4)
                throw new InvalidDataException("A draft must contain exactly four marker observations.");
            var captured = CaptureDraft();
            for (int i = 0; i < captured.anchors.Length; i++)
            {
                var row = document.anchors[i]; var current = captured.anchors[i];
                if (row == null || row.label != current.label || row.sceneX != current.sceneX || row.sceneZ != current.sceneZ)
                    throw new InvalidDataException("Draft marker positions/identity differ from the fingerprinted scene; capture again.");
            }
        }
        public static string Export(AnchorCalibrationDocument document)
        {
            ValidateIdentity(document); RefreshAssessment(document);
            // Draft JSON may be incomplete, but never emit NaN/Infinity coordinates or tolerances.
            foreach (var row in document.anchors ?? Array.Empty<AnchorDraftRow>())
            {
                if (row == null) throw new InvalidDataException("Null anchor row.");
                _ = new PlanarPoint(row.sceneX, row.sceneZ); _ = new PlanarPoint(row.legacyX, row.legacyY);
            }
            foreach (double value in new[] { document.tolerance, document.legacyMinX, document.legacyMinY, document.legacyMaxX,
                document.legacyMaxY, document.sceneMinX, document.sceneMinZ, document.sceneMaxX, document.sceneMaxZ })
                if (double.IsNaN(value) || double.IsInfinity(value)) throw new InvalidDataException("Non-finite draft number.");
            string folder = Path.GetFullPath(Path.Combine(Application.dataPath, "../../reports/evidence/anchors/drafts"));
            Directory.CreateDirectory(folder);
            string path = Path.Combine(folder, "map53-anchors-" + DateTime.UtcNow.ToString("yyyyMMdd-HHmmss") + "-" + Guid.NewGuid().ToString("N") + ".json");
            File.WriteAllText(path, JsonUtility.ToJson(document, true));
            return path;
        }
        public static AnchorCalibrationDocument Load(string path)
        {
            if (new FileInfo(path).Length > 256 * 1024) throw new InvalidDataException("Anchor draft exceeds 256 KiB.");
            var document = JsonUtility.FromJson<AnchorCalibrationDocument>(File.ReadAllText(path));
            ValidateIdentity(document); RefreshAssessment(document); return document;
        }
        public static void ExportCurrentSceneDraft()
        {
            string path = Export(CaptureDraft());
            Debug.Log("[CN02-ANCHORS] Unpaired draft exported; calibration remains UNVERIFIED: " + path);
        }
    }
}
