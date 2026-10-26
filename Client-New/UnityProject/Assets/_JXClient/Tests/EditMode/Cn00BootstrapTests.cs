using System;
using System.IO;
using JXClient.Core;
using JXClient.Domain;
using NUnit.Framework;
using UnityEditor;
using UnityEditor.SceneManagement;
using UnityEngine;
using UnityEngine.SceneManagement;

namespace JXClient.Tests.EditMode
{
    public sealed class Cn00BootstrapTests
    {
        [Test]
        public void DomainPrimitives_EnforceValidConstraints()
        {
            var playerId = new PlayerId(1001);
            Assert.That(playerId.IsValid, Is.True);
            Assert.That(playerId.Value, Is.EqualTo(1001));
            Assert.Throws<ArgumentOutOfRangeException>(() => _ = new PlayerId(0));
            Assert.Throws<ArgumentOutOfRangeException>(() => _ = new PlayerId(-5));

            Assert.That(MapId.BaLangHuyen.Value, Is.EqualTo(53));
            Assert.That(MapId.BaLangHuyen.IsValid, Is.True);
            Assert.Throws<ArgumentOutOfRangeException>(() => _ = new MapId(0));
            Assert.Throws<ArgumentOutOfRangeException>(() => _ = new MapId(70000));

            var pos = new WorldPosition(120.5f, 0f, -45.25f);
            Assert.That(pos.X, Is.EqualTo(120.5f));
            Assert.Throws<ArgumentException>(() => _ = new WorldPosition(float.NaN, 0f, 0f));
            Assert.Throws<ArgumentException>(() => _ = new WorldPosition(0f, float.PositiveInfinity, 0f));

            var intent = new MoveIntent(1u, 1u, pos);
            Assert.That(intent.Sequence, Is.EqualTo(1u));
            Assert.That(intent.MapEpoch, Is.EqualTo(1u));
            Assert.That(intent.Destination, Is.EqualTo(pos));
            Assert.Throws<ArgumentOutOfRangeException>(() => _ = new MoveIntent(0u, 1u, pos));
            Assert.Throws<ArgumentOutOfRangeException>(() => _ = new MoveIntent(1u, 0u, pos));
        }

        [Test]
        public void GameSessionRules_ForbidMoveBeforeWorldReady()
        {
            foreach (GameSessionState state in Enum.GetValues(typeof(GameSessionState)))
            {
                bool expected = state == GameSessionState.WorldReady;
                Assert.That(
                    GameSessionRules.CanSendMoveRequest(state),
                    Is.EqualTo(expected),
                    $"Unexpected CanSendMoveRequest result for state {state}");
            }
        }

        [Test]
        public void BootstrapScene_ExistsAndInitializesOfflineReadyState()
        {
            const string scenePath = "Assets/_JXClient/Scenes/Bootstrap.unity";
            Assert.That(File.Exists(scenePath), Is.True, $"Expected Bootstrap scene at {scenePath}");

            Scene scene = EditorSceneManager.OpenScene(scenePath, OpenSceneMode.Single);
            Assert.That(scene.name, Is.EqualTo("Bootstrap"));

            BootstrapRunner runner = UnityEngine.Object.FindFirstObjectByType<BootstrapRunner>();
            Assert.That(runner, Is.Not.Null, "BootstrapRunner must exist in Bootstrap.unity");

            runner.InitializeBootstrap();

            Assert.That(runner.IsBootstrapReady, Is.True, "BootstrapRunner must report IsBootstrapReady=true");
            Assert.That(runner.CurrentState, Is.EqualTo(GameSessionState.OfflineReady));
            Assert.That(GameSessionRules.CanSendMoveRequest(runner.CurrentState), Is.False);
            Assert.That(EditorBuildSettings.scenes.Length, Is.GreaterThanOrEqualTo(1));
            Assert.That(Array.Exists(EditorBuildSettings.scenes, entry => entry.enabled && entry.path == scenePath), Is.True);
        }
    }
}
