using System.Collections;
using JXClient.Movement;
using NUnit.Framework;
using UnityEngine;
using UnityEngine.TestTools;
using UnityEngine.EventSystems;
using UnityEngine.UI;

public class OfflineMovementTests
{
    GameObject root, ground, wall;
    OfflineMovement mover;
    Camera camera;
    [UnitySetUp] public IEnumerator Setup()
    {
        root = new GameObject("CN01Tests");
        ground = GameObject.CreatePrimitive(PrimitiveType.Plane);
        ground.transform.SetParent(root.transform); ground.layer = 8;
        var player = new GameObject("Player"); player.transform.SetParent(root.transform);
        var cc = player.AddComponent<CharacterController>(); cc.center = Vector3.up; cc.height = 2; cc.radius = .4f;
        mover = player.AddComponent<OfflineMovement>(); mover.enabled = false;
        var cam = new GameObject("Camera"); cam.transform.SetParent(root.transform);
        camera = cam.AddComponent<Camera>(); camera.transform.position = new Vector3(0,12,-14); camera.transform.LookAt(Vector3.zero);
        mover.ViewCamera = camera; mover.GroundMask = 1 << 8;
        yield return null; Physics.SyncTransforms();
    }
    [UnityTearDown] public IEnumerator Cleanup() { Object.Destroy(root); yield return null; }
    Vector2 Screen(Vector3 point) => camera.WorldToScreenPoint(point);
    [UnityTest] public IEnumerator GroundClickMovesAtBoundedSpeed()
    {
        Assert.That(mover.TryPointer(Screen(new Vector3(3,0,0))), Is.True);
        Assert.That(mover.AcceptedIntents, Is.EqualTo(1));
        var before = mover.transform.position;
        mover.Tick(.1f); yield return null;
        Assert.That(mover.transform.position.x, Is.GreaterThan(.1f));
        Assert.That(Vector3.Distance(before,mover.transform.position), Is.LessThanOrEqualTo(.61f));
        for(int i=0;i<100;i++) { mover.Tick(.02f); yield return null; }
        Assert.That(Vector3.Distance(mover.transform.position,new Vector3(3,0,0)), Is.LessThan(.2f));
    }
    [UnityTest] public IEnumerator TinyFrameStepIsNotDiscardedByController()
    {
        mover.GetComponent<CharacterController>().minMoveDistance=.001f;
        Assert.That(mover.TryPointer(Screen(new Vector3(3,0,0))),Is.True);
        mover.Tick(.00001f); yield return null;
        Assert.That(mover.transform.position.x,Is.GreaterThan(.00001f));
    }
    [UnityTest] public IEnumerator MouseInputEventReachesUpdateAndCreatesOneIntent()
    {
        var settings=UnityEngine.InputSystem.InputSystem.settings;
        var oldBackground=settings.backgroundBehavior;
        var oldEditor=settings.editorInputBehaviorInPlayMode;
        settings.backgroundBehavior=UnityEngine.InputSystem.InputSettings.BackgroundBehavior.IgnoreFocus;
        settings.editorInputBehaviorInPlayMode=UnityEngine.InputSystem.InputSettings.EditorInputBehaviorInPlayMode.AllDeviceInputAlwaysGoesToGameView;
        var mouse=UnityEngine.InputSystem.InputSystem.AddDevice<UnityEngine.InputSystem.Mouse>();
        UnityEngine.InputSystem.InputSystem.EnableDevice(mouse);
        try {
            mover.enabled=true;
            var point=Screen(new Vector3(3,0,0));
            UnityEngine.InputSystem.InputSystem.QueueStateEvent(mouse,new UnityEngine.InputSystem.LowLevel.MouseState{position=point}.WithButton(UnityEngine.InputSystem.LowLevel.MouseButton.Left));
            yield return null; yield return null;
            Assert.That(mover.AcceptedIntents,Is.EqualTo(1),$"last={mover.LastInput} current={UnityEngine.InputSystem.Mouse.current?.deviceId} test={mouse.deviceId} pressed={mouse.leftButton.wasPressedThisFrame} enabled={mouse.enabled} update={UnityEngine.InputSystem.InputSystem.settings.updateMode}");
            Assert.That(mover.IsMoving,Is.True);
        } finally { UnityEngine.InputSystem.InputSystem.RemoveDevice(mouse); settings.backgroundBehavior=oldBackground; settings.editorInputBehaviorInPlayMode=oldEditor; }
    }
    [UnityTest] public IEnumerator MissingGroundRejectsInput()
    {
        ground.GetComponent<Collider>().enabled=false; Physics.SyncTransforms();
        Assert.That(mover.TryPointer(Screen(new Vector3(3,0,0))), Is.False);
        mover.Tick(.1f); yield return null;
        Assert.That(mover.AcceptedIntents, Is.Zero);
        Assert.That(mover.transform.position.x, Is.Zero);
    }
    [UnityTest] public IEnumerator UiPanelConsumesClick()
    {
        var es = new GameObject("Events",typeof(EventSystem)); es.transform.SetParent(root.transform);
        var canvas = new GameObject("Canvas",typeof(Canvas),typeof(GraphicRaycaster)); canvas.transform.SetParent(root.transform);
        canvas.GetComponent<Canvas>().renderMode=RenderMode.ScreenSpaceOverlay;
        var panel = new GameObject("Panel",typeof(RectTransform),typeof(UnityEngine.UI.Image)); panel.transform.SetParent(canvas.transform,false);
        var rect=panel.GetComponent<RectTransform>(); rect.anchorMin=Vector2.zero; rect.anchorMax=Vector2.one; rect.offsetMin=rect.offsetMax=Vector2.zero;
        yield return null; Canvas.ForceUpdateCanvases();
        Assert.That(mover.TryPointer(Screen(new Vector3(3,0,0))), Is.False);
        Assert.That(mover.AcceptedIntents, Is.Zero);
    }
    [UnityTest] public IEnumerator WallStopsPlayerWithoutPenetration()
    {
        wall=GameObject.CreatePrimitive(PrimitiveType.Cube); wall.transform.SetParent(root.transform);
        wall.transform.position=new Vector3(1.5f,1,0); wall.transform.localScale=new Vector3(.5f,2,4); Physics.SyncTransforms();
        Assert.That(mover.TryPointer(Screen(new Vector3(3,0,0))), Is.True);
        for(int i=0;i<120;i++){mover.Tick(.02f);yield return null;}
        Assert.That(mover.transform.position.x, Is.LessThan(1f));
        Assert.That(mover.transform.position.x, Is.GreaterThan(.2f));
    }
    [UnityTest] public IEnumerator CameraOrbitAndZoomKeepGroundRaycastValid()
    {
        var orbit=camera.gameObject.AddComponent<OrbitFollowCamera>(); orbit.enabled=false; orbit.Target=mover.transform;
        int index=0; foreach(float yaw in new[]{0f,90f,210f}) {
            orbit.SetOrbit(yaw,new[]{35f,55f,70f}[index],new[]{6f,12f,24f}[index]); index++; orbit.ApplyPose(); Physics.SyncTransforms();
            Assert.That(mover.TryPointer(Screen(new Vector3(2,0,2))),Is.True,$"yaw={yaw}");
            yield return null;
        }
        Assert.That(mover.AcceptedIntents,Is.EqualTo(3));
    }
    [UnityTest] public IEnumerator InvalidInputAndGroundEdgeCannotMovePlayer()
    {
        Assert.That(mover.TryPointer(new Vector2(float.NaN,0)),Is.False);
        Assert.That(mover.TryPointer(new Vector2(-1,-1)),Is.False);
        Assert.That(mover.TryPointer(Screen(new Vector3(4,0,0))),Is.True);
        Object.Destroy(ground); yield return null;
        mover.Tick(.1f);
        Assert.That(mover.transform.position.x,Is.Zero);
    }
}
