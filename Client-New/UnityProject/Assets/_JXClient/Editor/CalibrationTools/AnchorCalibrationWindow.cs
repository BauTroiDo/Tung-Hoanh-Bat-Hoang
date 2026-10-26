using System;
using System.IO;
using JXClient.Domain.Coordinates;
using UnityEditor;
using UnityEditor.UIElements;
using UnityEditor.SceneManagement;
using UnityEngine.SceneManagement;
using JXClient.World;
using UnityEngine;
using UnityEngine.UIElements;

namespace JXClient.Editor.Calibration
{
    public sealed class AnchorCalibrationWindow : EditorWindow
    {
        AnchorCalibrationDocument document;
        Label status;
        [MenuItem("JXClient/CN-02/Anchor Evidence (3+1)")]
        public static void Open()
        {
            if (EditorApplication.isPlayingOrWillChangePlaymode)
                throw new InvalidOperationException("Stop Play Mode before opening the calibration scene.");
            if (SceneManager.GetActiveScene().path != AnchorCalibrationTools.ScenePath)
            {
                // Unity asks about unsaved scene edits instead of silently discarding them.
                if (!EditorSceneManager.SaveCurrentModifiedScenesIfUserWantsTo()) return;
                EditorSceneManager.OpenScene(AnchorCalibrationTools.ScenePath, OpenSceneMode.Single);
            }
            var window = GetWindow<AnchorCalibrationWindow>("Map53 Anchor Evidence");
            window.document = AnchorCalibrationTools.CaptureDraft();
            window.CreateGUI();
            var view = SceneView.lastActiveSceneView;
            if (view != null)
            {
                var bounds = MapSceneCatalog.BaLangHuyen.CameraBounds;
                view.LookAt(bounds.center, Quaternion.Euler(50, 0, 0), bounds.extents.magnitude);
            }
            window.Focus();
        }
        public void CreateGUI()
        {
            minSize = new Vector2(640, 600);
            rootVisualElement.Clear();
            rootVisualElement.Add(new Label("MAP53 — 3 FIT ANCHORS + 1 INDEPENDENT HOLDOUT"));
            rootVisualElement.Add(new HelpBox("Capture scene points, enter measured legacy points, attach evidence and declare a tolerance. Nothing here marks the map verified or changes the server.", HelpBoxMessageType.Info));
            var buttons = new VisualElement(); buttons.style.flexDirection = FlexDirection.Row;
            AddButton(buttons, "captureMarkers", "Capture markers", () => { document = AnchorCalibrationTools.CaptureDraft(); CreateGUI(); });
            AddButton(buttons, "importDraft", "Import draft", () => {
                string path = EditorUtility.OpenFilePanel("Anchor JSON", "", "json");
                if (!string.IsNullOrEmpty(path)) { document = AnchorCalibrationTools.Load(path); CreateGUI(); }
            });
            AddButton(buttons, "assessDraft", "Assess candidate", () => {
                if (document == null) throw new InvalidOperationException("Capture or import a draft first.");
                AnchorCalibrationTools.RefreshAssessment(document); ShowStatus();
            });
            AddButton(buttons, "exportDraft", "Export draft", () => {
                if (document == null) throw new InvalidOperationException("Capture or import a draft first.");
                string path = AnchorCalibrationTools.Export(document); ShowStatus(); status.text += "\nSaved: " + path;
            });
            rootVisualElement.Add(buttons);
            status = new Label(); status.style.whiteSpace = WhiteSpace.Normal; rootVisualElement.Add(status); ShowStatus();
            if (document == null) return;
            var scroll = new ScrollView(); rootVisualElement.Add(scroll);
            scroll.Add(new Label("Scene: " + document.sceneProvenance + " | SHA256 " + document.sceneSha256));
            var build = new TextField("Reference build identity") { value = document.referenceBuild };
            build.RegisterValueChangedCallback(e => document.referenceBuild = e.newValue); scroll.Add(build);
            AddNumber(scroll, "Allowed holdout error (scene units)", document.tolerance, v => document.tolerance = v);
            var bounds = new Foldout { text = "Legacy / scene horizontal bounds", value = false }; scroll.Add(bounds);
            AddNumber(bounds,"Legacy min X",document.legacyMinX,v=>document.legacyMinX=v); AddNumber(bounds,"Legacy min Y",document.legacyMinY,v=>document.legacyMinY=v);
            AddNumber(bounds,"Legacy max X",document.legacyMaxX,v=>document.legacyMaxX=v); AddNumber(bounds,"Legacy max Y",document.legacyMaxY,v=>document.legacyMaxY=v);
            AddNumber(bounds,"Scene min X",document.sceneMinX,v=>document.sceneMinX=v); AddNumber(bounds,"Scene min Z",document.sceneMinZ,v=>document.sceneMinZ=v);
            AddNumber(bounds,"Scene max X",document.sceneMaxX,v=>document.sceneMaxX=v); AddNumber(bounds,"Scene max Z",document.sceneMaxZ,v=>document.sceneMaxZ=v);
            for (int i = 0; i < document.anchors.Length; i++)
            {
                var row = document.anchors[i];
                var group = new Foldout { text = (i == 3 ? "Holdout: " : "Fit: ") + row.label, value = i == 0 }; scroll.Add(group);
                group.Add(new Label($"Captured scene X/Z: ({row.sceneX:R}, {row.sceneZ:R})"));
                var known = new Toggle("Legacy point measured") { value = row.hasLegacy }; known.RegisterValueChangedCallback(e => row.hasLegacy = e.newValue); group.Add(known);
                AddNumber(group,"Legacy X",row.legacyX,v=>row.legacyX=v);AddNumber(group,"Legacy Y",row.legacyY,v=>row.legacyY=v);
                var kind = new EnumField("Evidence origin", row.evidenceKind); kind.RegisterValueChangedCallback(e=>row.evidenceKind=(AnchorEvidenceKind)e.newValue);group.Add(kind);
                var evidence = new Label(row.evidencePath + "\n" + row.evidenceSha256); evidence.style.whiteSpace=WhiteSpace.Normal; group.Add(evidence);
                group.Add(new Button(() => Run(() => {
                    string path=EditorUtility.OpenFilePanel("Evidence file (up to 16 MiB)","","");
                    if(string.IsNullOrEmpty(path))return;
                    if(new FileInfo(path).Length>16*1024*1024)throw new InvalidDataException("Evidence file exceeds 16 MiB.");
                    row.evidencePath=path;row.evidenceSha256=AnchorCalibrationTools.HashFile(path);evidence.text=path+"\n"+row.evidenceSha256;
                })) { text="Attach evidence file + SHA256" });
            }
        }
        void AddButton(VisualElement parent,string name,string text,Action action) => parent.Add(new Button(()=>Run(action)){name=name,text=text});
        static void AddNumber(VisualElement parent,string label,double value,Action<double> update)
        {var field=new DoubleField(label){value=value};field.RegisterValueChangedCallback(e=>update(e.newValue));parent.Add(field);}
        void Run(Action action){try{action();}catch(Exception ex){if(status!=null)status.text="Cannot complete: "+ex.Message;}}
        void ShowStatus(){if(status!=null)status.text=document==null?"No draft loaded. Capture current markers to begin.":document.status+"\n"+document.message+(document.hasResidual?$"\nHoldout error: {document.holdoutError:R}":"")+"\nIndependent review required; map calibration remains unchanged.";}
    }
}
