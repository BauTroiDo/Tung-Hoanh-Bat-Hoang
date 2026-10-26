using JXClient.Editor;
using NUnit.Framework;

namespace JXClient.Tests.EditMode
{
    public sealed class Cn02Bulk01AssetAuditTests
    {
        [Test]
        public void Bulk01AssetAudit_AllMaterialsPrefabsAndScenesAreHealthy()
        {
            Bulk01AuditReport report = Cn02BulkAssetAuditor.ExecuteAudit(applyDeterministicFixes: false, writeReportFiles: false);

            Assert.That(report.missingMetaCount, Is.Zero, $"Missing .meta files: {string.Join(", ", report.missingMetaFiles)}");
            Assert.That(report.duplicateGuidCount, Is.Zero, $"Duplicate GUIDs: {string.Join(", ", report.duplicateGuids)}");
            Assert.That(report.materials.Count, Is.GreaterThanOrEqualTo(10), "Expected all 10 URP materials in Assets/_JXClient (8 BLH/Avatar + 2 CN01)");

            string[] requiredAntigravityMaterials =
            {
                "BLH_BoundaryRock", "BLH_GrassTerrain", "BLH_HouseWall", "BLH_PlazaTile",
                "BLH_StoneRoad", "BLH_TileRoof", "BLH_WoodTimber", "Avatar"
            };
            foreach (string reqMat in requiredAntigravityMaterials)
            {
                Assert.That(report.materials.Exists(m => m.materialName == reqMat), Is.True, $"Missing required material {reqMat}");
            }

            foreach (MaterialAuditRecord mat in report.materials)
            {
                Assert.That(mat.shaderSupported, Is.True, $"Material {mat.materialName} uses unsupported shader {mat.shaderNameAfter}");
                Assert.That(mat.missingTextureCount, Is.Zero, $"Material {mat.materialName} has missing texture references");
                Assert.That(mat.baseColorAfter.a, Is.EqualTo(1f).Within(0.01f), $"Material {mat.materialName} base color alpha should be 1");
            }

            Assert.That(report.prefabs.Count, Is.EqualTo(3), "Expected 3 prefabs in Assets/_JXClient");
            foreach (PrefabAuditRecord prefab in report.prefabs)
            {
                Assert.That(prefab.passed, Is.True, $"Prefab {prefab.prefabName} failed audit");
            }

            Assert.That(report.scenes.Count, Is.EqualTo(3), "Expected 3 scenes in Assets/_JXClient/Scenes");
            foreach (SceneAuditRecord scene in report.scenes)
            {
                Assert.That(scene.passed, Is.True, $"Scene {scene.sceneName} failed audit");
            }

            Assert.That(report.totalUnresolvedErrors, Is.Zero);
            Assert.That(report.allChecksPassed, Is.True);
        }
    }
}
