using System.Collections;
using JXClient.Domain;
using JXClient.World;
using NUnit.Framework;
using UnityEngine;
using UnityEngine.SceneManagement;
using UnityEngine.TestTools;

public class Cn02OwnershipRegressionTests
{
    MapSceneLoader loader;
    Scene foreign;
    bool loadStarted;
    [UnitySetUp] public IEnumerator SetUp()
    {
        loader=new MapSceneLoader();
        var existing=SceneManager.GetSceneByName("BaLangHuyen");
        if(existing.IsValid() && existing.isLoaded) yield return SceneManager.UnloadSceneAsync(existing);
        yield return null;
    }
    [UnityTearDown] public IEnumerator TearDown()
    {
        // Also cleans the scene left by the intentionally failing pre-fix cancellation case.
        if(loadStarted) for(int i=0;i<60;i++) yield return null;
        for(int i=SceneManager.sceneCount-1;i>=0;i--) {
            var scene=SceneManager.GetSceneAt(i);
            if(scene.name=="BaLangHuyen"&&scene.isLoaded) yield return SceneManager.UnloadSceneAsync(scene);
        }
        if(foreign.IsValid()&&foreign.isLoaded) yield return SceneManager.UnloadSceneAsync(foreign);
        yield return null; Physics.SyncTransforms();
    }
    [UnityTest] public IEnumerator CancelWhilePendingKeepsCleanupAliveAndCallsBackOnce()
    {
        int callbacks=0; WorldSceneValidationReport completed=null;
        var routine=loader.LoadMapRoutine(MapId.BaLangHuyen,LoadSceneMode.Additive,r=>{callbacks++;completed=r;});
        loadStarted=true;
        Assert.That(routine.MoveNext(),Is.True,"Load must be pending before cancellation.");
        loader.CancelCurrentLoad();
        Assert.That(routine.MoveNext(),Is.True,"Cancelled pending operation must remain scheduled for cleanup.");
        while(routine.MoveNext()) yield return routine.Current;
        Assert.That(callbacks,Is.EqualTo(1)); Assert.That(completed,Is.Null);
        Assert.That(SceneManager.GetSceneByName("BaLangHuyen").isLoaded,Is.False);
        Assert.That(loader.LoadedScene.IsValid(),Is.False);
    }
    [UnityTest] public IEnumerator CancelCleanupBlocksReplacementUntilOriginalRoutineFinishes()
    {
        loadStarted=true;
        var original=loader.LoadMapRoutine(MapId.BaLangHuyen);
        Assert.That(original.MoveNext(),Is.True);
        loader.CancelCurrentLoad();
        int rejectedCallbacks=0;
        var replacement=loader.LoadMapRoutine(MapId.BaLangHuyen,LoadSceneMode.Additive,r=>{rejectedCallbacks++;Assert.That(r,Is.Null);});
        bool admitted=replacement.MoveNext();
        Assert.That(admitted,Is.False,"Cancellation cleanup must retain exclusive ownership until drained.");
        Assert.That(rejectedCallbacks,Is.EqualTo(1));
        while(original.MoveNext())yield return original.Current;
        yield return loader.LoadMapRoutine(MapId.BaLangHuyen);
        Assert.That(loader.LoadedScene.isLoaded,Is.True,"A later load should succeed after cleanup.");
    }
    [UnityTest] public IEnumerator DuplicateTargetCannotUnloadAnAlreadyLoadedMap()
    {
        yield return Load();var existing=loader.LoadedScene;int callbacks=0;
        var duplicate=loader.LoadMapRoutine(MapId.BaLangHuyen,LoadSceneMode.Additive,r=>{callbacks++;Assert.That(r,Is.Null);});
        bool admitted=duplicate.MoveNext();
        if(admitted){loader.CancelCurrentLoad();while(duplicate.MoveNext())yield return duplicate.Current;}
        Assert.That(!admitted && existing.isLoaded && callbacks==1,Is.True,
            $"Duplicate must be rejected, preserve original and callback once: admitted={admitted}, originalLoaded={existing.isLoaded}, callbacks={callbacks}");
    }
    IEnumerator Load()
    {
        loadStarted=true;
        yield return loader.LoadMapRoutine(MapId.BaLangHuyen);
        Assert.That(loader.LoadedScene.isLoaded,Is.True);
    }
    void AddForeignGround()
    {
        foreign=SceneManager.CreateScene("ForeignGroundRegression");
        var plane=GameObject.CreatePrimitive(PrimitiveType.Plane);
        plane.name="ForeignGround";plane.layer=8;plane.transform.position=new Vector3(0,3,0);plane.transform.localScale=new Vector3(10,1,10);
        SceneManager.MoveGameObjectToScene(plane,foreign);Physics.SyncTransforms();
    }
    [UnityTest] public IEnumerator ForeignSceneCannotSupplyMissingGroundOrSpawn()
    {
        yield return Load(); var scene=loader.LoadedScene; Transform groundRoot=null;
        foreach(var root in scene.GetRootGameObjects()) {
            if(root.name=="Ground")groundRoot=root.transform;
            foreach(var collider in root.GetComponentsInChildren<Collider>())if(collider.gameObject.layer==8)collider.enabled=false;
        }
        Assert.That(groundRoot,Is.Not.Null);
        var far=GameObject.CreatePrimitive(PrimitiveType.Cube);far.name="OwnGroundFarAway";far.layer=8;
        far.transform.SetParent(groundRoot);far.transform.position=new Vector3(300,0,300);
        AddForeignGround();
        var result=WorldSceneValidator.ValidateLoadedScene(scene,MapSceneCatalog.BaLangHuyen);
        Assert.That(result.HasEnabledGroundCollider,Is.True,"A distant target-scene collider must not validate unrelated probe locations.");
        Assert.That(result.AllGroundProbesHit,Is.False);
        Assert.That(result.HasValidSpawnOnGround,Is.False);
        Assert.That(result.IsOfflineSceneReady,Is.False);
    }
    [UnityTest] public IEnumerator ForeignForegroundDoesNotHideValidOwnSceneGround()
    {
        yield return Load();AddForeignGround();
        var result=WorldSceneValidator.ValidateLoadedScene(loader.LoadedScene,MapSceneCatalog.BaLangHuyen);
        Assert.That(result.IsOfflineSceneReady,Is.True);
        foreach(var probe in result.ProbeResults){Assert.That(probe.ColliderName,Is.Not.EqualTo("ForeignGround"));Assert.That(probe.HitPoint.y,Is.LessThan(.5f));}
        Assert.That(result.ValidatedSpawnPosition.y,Is.LessThan(.5f));
    }
}
