using System;
using System.IO;
using JXClient.Movement;
using UnityEditor;
using UnityEditor.Build.Reporting;
using UnityEditor.SceneManagement;
using UnityEngine;
using UnityEngine.EventSystems;
using UnityEngine.InputSystem.UI;
using UnityEngine.UI;

namespace JXClient.Editor
{
    public static class Cn01ProjectSetup
    {
        const string Base = "Assets/_JXClient";
        public const string ScenePath = Base + "/Scenes/MovementTest.unity";
        static void Folder(string path)
        {
            if (AssetDatabase.IsValidFolder(path)) return;
            string parent=path.Substring(0,path.LastIndexOf('/')); Folder(parent);
            AssetDatabase.CreateFolder(parent,path.Substring(path.LastIndexOf('/')+1));
        }
        static Material Material(string name, Color color)
        {
            string path=Base+"/Movement/Materials/"+name+".mat";
            var mat=AssetDatabase.LoadAssetAtPath<Material>(path);
            if(!mat){mat=new Material(Shader.Find("Universal Render Pipeline/Lit"));AssetDatabase.CreateAsset(mat,path);}
            mat.color=color; EditorUtility.SetDirty(mat); return mat;
        }
        public static void Generate()
        {
            Folder(Base+"/Scenes");Folder(Base+"/Prefabs");Folder(Base+"/Movement/Materials");
            var tags=new SerializedObject(AssetDatabase.LoadAllAssetsAtPath("ProjectSettings/TagManager.asset")[0]);
            var layer=tags.FindProperty("layers").GetArrayElementAtIndex(8);
            if(!string.IsNullOrEmpty(layer.stringValue) && layer.stringValue!="Ground") throw new InvalidOperationException("Layer 8 already owned");
            layer.stringValue="Ground";tags.ApplyModifiedProperties();
            var scene=EditorSceneManager.NewScene(NewSceneSetup.EmptyScene,NewSceneMode.Single);
            var ground=GameObject.CreatePrimitive(PrimitiveType.Plane);ground.name="WalkableGround";ground.layer=8;ground.transform.localScale=new Vector3(3,1,3);
            ground.GetComponent<Renderer>().sharedMaterial=Material("Ground",new Color(.16f,.34f,.27f));
            var wall=GameObject.CreatePrimitive(PrimitiveType.Cube);wall.name="CollisionWall";wall.transform.position=new Vector3(2,1,1.5f);wall.transform.localScale=new Vector3(1,2,5);
            wall.GetComponent<Renderer>().sharedMaterial=Material("Wall",new Color(.34f,.4f,.48f));
            var player=GameObject.CreatePrimitive(PrimitiveType.Capsule);player.name="OfflinePlayer";
            UnityEngine.Object.DestroyImmediate(player.GetComponent<Collider>());
            // Keep root at feet; visual capsule is a child centered one unit above ground.
            var mesh=player.GetComponent<MeshFilter>().sharedMesh;
            UnityEngine.Object.DestroyImmediate(player.GetComponent<MeshFilter>());
            UnityEngine.Object.DestroyImmediate(player.GetComponent<MeshRenderer>());
            var visual=new GameObject("Avatar");visual.transform.SetParent(player.transform,false);visual.transform.localPosition=Vector3.up;
            visual.AddComponent<MeshFilter>().sharedMesh=mesh;visual.AddComponent<MeshRenderer>().sharedMaterial=Material("Avatar",new Color(.94f,.66f,.19f));
            var cc=player.AddComponent<CharacterController>();cc.center=Vector3.up;cc.height=2;cc.radius=.4f;cc.stepOffset=.25f;
            var mover=player.AddComponent<OfflineMovement>();mover.GroundMask=1<<8;
            PrefabUtility.SaveAsPrefabAsset(player,Base+"/Prefabs/OfflinePlayer.prefab");
            player.transform.position=new Vector3(0,0,-3);
            var cameraGo=new GameObject("Main Camera");cameraGo.tag="MainCamera";
            var camera=cameraGo.AddComponent<Camera>();camera.backgroundColor=new Color(.08f,.12f,.17f);camera.clearFlags=CameraClearFlags.SolidColor;cameraGo.AddComponent<AudioListener>();
            var orbit=cameraGo.AddComponent<OrbitFollowCamera>();orbit.Target=player.transform;orbit.SetOrbit(0,50,16);orbit.ApplyPose();mover.ViewCamera=camera;
            var sun=new GameObject("Sun").AddComponent<Light>();sun.type=LightType.Directional;sun.intensity=1.5f;sun.transform.rotation=Quaternion.Euler(50,-30,0);
            RenderSettings.ambientLight=new Color(.35f,.4f,.45f);
            var canvas=new GameObject("MovementHud",typeof(Canvas),typeof(CanvasScaler),typeof(GraphicRaycaster));canvas.GetComponent<Canvas>().renderMode=RenderMode.ScreenSpaceOverlay;
            var scaler=canvas.GetComponent<CanvasScaler>();scaler.uiScaleMode=CanvasScaler.ScaleMode.ScaleWithScreenSize;scaler.referenceResolution=new Vector2(1280,720);scaler.matchWidthOrHeight=.5f;
            var panel=new GameObject("InputShield",typeof(RectTransform),typeof(UnityEngine.UI.Image));panel.transform.SetParent(canvas.transform,false);
            var rect=panel.GetComponent<RectTransform>();rect.anchorMin=rect.anchorMax=new Vector2(0,1);rect.pivot=new Vector2(0,1);rect.anchoredPosition=new Vector2(16,-16);rect.sizeDelta=new Vector2(470,175);
            panel.GetComponent<UnityEngine.UI.Image>().color=new Color(.025f,.045f,.065f,.95f);
            var textGo=new GameObject("Status",typeof(RectTransform),typeof(UnityEngine.UI.Text));textGo.transform.SetParent(panel.transform,false);
            var tr=textGo.GetComponent<RectTransform>();tr.anchorMin=Vector2.zero;tr.anchorMax=Vector2.one;tr.offsetMin=new Vector2(12,12);tr.offsetMax=new Vector2(-12,-12);
            var text=textGo.GetComponent<UnityEngine.UI.Text>();text.font=Resources.GetBuiltinResource<Font>("LegacyRuntime.ttf");text.fontSize=16;text.color=Color.white;text.raycastTarget=false;
            var events=new GameObject("EventSystem",typeof(EventSystem),typeof(InputSystemUIInputModule));events.GetComponent<InputSystemUIInputModule>().AssignDefaultActions();
            var hud=canvas.AddComponent<MovementHud>();hud.Player=mover;hud.Label=text;
            if(!EditorSceneManager.SaveScene(scene,ScenePath))throw new InvalidOperationException("Scene save failed");
            EditorBuildSettings.scenes=new[]{new EditorBuildSettingsScene(ScenePath,true),new EditorBuildSettingsScene(Cn00ProjectSetup.BootstrapScenePath,true)};
            AssetDatabase.SaveAssets();AssetDatabase.Refresh();Debug.Log("[CN-01] MovementTest scene/prefab generated");
        }
        public static void Build()
        {
            string output=Path.GetFullPath(Path.Combine(Application.dataPath,"../../Builds/CN01-Windows/JXClientNew.exe"));
            Directory.CreateDirectory(Path.GetDirectoryName(output));
            var report=BuildPipeline.BuildPlayer(new BuildPlayerOptions{scenes=new[]{ScenePath,Cn00ProjectSetup.BootstrapScenePath},target=BuildTarget.StandaloneWindows64,locationPathName=output,options=BuildOptions.None});
            Debug.Log($"[CN-01] BUILD result={report.summary.result} errors={report.summary.totalErrors} warnings={report.summary.totalWarnings} bytes={report.summary.totalSize}");
            if(report.summary.result!=BuildResult.Succeeded)EditorApplication.Exit(1);
        }
    }
}
