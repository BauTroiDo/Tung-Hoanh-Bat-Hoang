using System;
using System.IO;
using JXClient.Editor.Calibration;
using NUnit.Framework;
using UnityEngine;
using UnityEngine.UIElements;

public class AnchorCalibrationEditorTests
{
    [Test] public void CapturedSceneDraftExportsAndRoundTripsWithoutInventingLegacyPoints()
    {
        var draft=AnchorCalibrationTools.CaptureDraft();
        Assert.That(draft.anchors.Length,Is.EqualTo(4));Assert.That(draft.sceneSha256.Length,Is.EqualTo(64));
        Assert.That(draft.anchors[0].sceneX,Is.EqualTo(0));Assert.That(draft.anchors[1].sceneZ,Is.EqualTo(18));
        foreach(var row in draft.anchors)Assert.That(row.hasLegacy,Is.False);
        draft.independentReviewRequired=false; // Export/import must restore this review requirement.
        string path=AnchorCalibrationTools.Export(draft);
        try {var loaded=AnchorCalibrationTools.Load(path);Assert.That(loaded.status,Is.EqualTo("DRAFT_UNPAIRED"));Assert.That(loaded.independentReviewRequired,Is.True);}
        finally{File.Delete(path);}
    }
    [Test] public void StaleSceneFingerprintIsRejectedOnImport()
    {
        var draft=AnchorCalibrationTools.CaptureDraft();draft.sceneSha256=new string('0',64);
        string path=Path.Combine(Path.GetTempPath(),"anchor-stale-"+Guid.NewGuid().ToString("N")+".json");
        File.WriteAllText(path,JsonUtility.ToJson(draft));
        try {Assert.Throws<InvalidDataException>(()=>AnchorCalibrationTools.Load(path));}finally{File.Delete(path);}
    }
    [Test] public void ChangedSceneMarkerCoordinatesCannotReuseCurrentSceneFingerprint()
    {
        var draft=AnchorCalibrationTools.CaptureDraft();draft.anchors[0].sceneX+=10;
        string path=Path.Combine(Path.GetTempPath(),"anchor-tampered-"+Guid.NewGuid().ToString("N")+".json");File.WriteAllText(path,JsonUtility.ToJson(draft));
        try{Assert.Throws<InvalidDataException>(()=>AnchorCalibrationTools.Load(path));}finally{File.Delete(path);}
    }
    [Test] public void IncompleteDraftShapeIsRejectedOnImport()
    {
        var draft=AnchorCalibrationTools.CaptureDraft();Array.Resize(ref draft.anchors,3);
        string path=Path.Combine(Path.GetTempPath(),"anchor-shape-"+Guid.NewGuid().ToString("N")+".json");File.WriteAllText(path,JsonUtility.ToJson(draft));
        try{Assert.Throws<InvalidDataException>(()=>AnchorCalibrationTools.Load(path));}finally{File.Delete(path);}
    }
    [Test] public void ChangedEvidenceFileCannotRemainReadyForReview()
    {
        // Synthetic metadata fixture only; it does not provide measured Map53 calibration.
        var draft=AnchorCalibrationTools.CaptureDraft();draft.referenceBuild="TEST-FIXTURE-ONLY";draft.tolerance=.01;
        draft.legacyMinX=draft.legacyMinY=-28;draft.legacyMaxX=draft.legacyMaxY=28;
        string proof=Path.Combine(Path.GetTempPath(),"anchor-proof-"+Guid.NewGuid().ToString("N")+".txt");File.WriteAllText(proof,"synthetic evidence fixture");
        try{
            foreach(var row in draft.anchors){row.hasLegacy=true;row.legacyX=row.sceneX;row.legacyY=row.sceneZ;row.evidenceKind=JXClient.Domain.Coordinates.AnchorEvidenceKind.MeasuredReference;row.evidencePath=proof;row.evidenceSha256=AnchorCalibrationTools.HashFile(proof);}
            AnchorCalibrationTools.RefreshAssessment(draft);Assert.That(draft.status,Is.EqualTo("ReadyForReview"));
            File.AppendAllText(proof," changed");AnchorCalibrationTools.RefreshAssessment(draft);
            Assert.That(draft.status,Is.EqualTo("NeedsEvidence"));Assert.That(draft.independentReviewRequired,Is.True);
        }finally{File.Delete(proof);}
    }
    [Test] public void OpenCommandShowsTargetMapAndPrefilledMarkerForm()
    {
        UnityEditor.SceneManagement.EditorSceneManager.OpenScene("Assets/_JXClient/Scenes/Bootstrap.unity",UnityEditor.SceneManagement.OpenSceneMode.Single);
        AnchorCalibrationWindow.Open();
        var window=UnityEditor.EditorWindow.GetWindow<AnchorCalibrationWindow>();
        try{
            Assert.That(UnityEngine.SceneManagement.SceneManager.GetActiveScene().path,Is.EqualTo(AnchorCalibrationTools.ScenePath));
            Assert.That(window.rootVisualElement.Q<TextField>(),Is.Not.Null,"Reference-build form must be visible immediately.");
            Assert.That(window.rootVisualElement.Query<Foldout>().ToList().Exists(f=>f.text.Contains("AnchorCandidate_CenterPlaza")),Is.True);
        }finally{UnityEngine.Object.DestroyImmediate(window);}
    }
    [Test] public void EditorWindowCreatesCaptureImportAssessAndExportControls()
    {
        var window=ScriptableObject.CreateInstance<AnchorCalibrationWindow>();
        try {window.CreateGUI();foreach(string name in new[]{"captureMarkers","importDraft","assessDraft","exportDraft"})Assert.That(window.rootVisualElement.Q<Button>(name),Is.Not.Null,name);}
        finally{UnityEngine.Object.DestroyImmediate(window);}
    }
}
