using System.Collections;
using JXClient.Movement;
using NUnit.Framework;
using UnityEngine;
using UnityEngine.SceneManagement;
using UnityEngine.TestTools;

public class Cn01SceneTests
{
    [UnityTest] public IEnumerator RealMovementSceneHasGroundPlayerCameraAndShield()
    {
        yield return SceneManager.LoadSceneAsync("MovementTest",LoadSceneMode.Additive);
        var scene=SceneManager.GetSceneByName("MovementTest");
        OfflineMovement player=null;OrbitFollowCamera orbit=null;UnityEngine.UI.GraphicRaycaster shield=null;
        foreach(var root in scene.GetRootGameObjects()) {
            if(root.GetComponent<OfflineMovement>())player=root.GetComponent<OfflineMovement>();
            if(root.GetComponent<OrbitFollowCamera>())orbit=root.GetComponent<OrbitFollowCamera>();
            if(root.GetComponent<UnityEngine.UI.GraphicRaycaster>())shield=root.GetComponent<UnityEngine.UI.GraphicRaycaster>();
        }
        Assert.That(player,Is.Not.Null);Assert.That(player.GetComponent<CharacterController>(),Is.Not.Null);
        Assert.That(player.ViewCamera,Is.Not.Null);Assert.That(orbit.Target,Is.EqualTo(player.transform));Assert.That(shield,Is.Not.Null);
        Physics.SyncTransforms();
        Assert.That(Physics.Raycast(new Vector3(-3,10,0),Vector3.down,out var hit,20,1<<8),Is.True);
        Assert.That(hit.collider.gameObject.name,Is.EqualTo("WalkableGround"));
        yield return SceneManager.UnloadSceneAsync(scene);
    }
}
