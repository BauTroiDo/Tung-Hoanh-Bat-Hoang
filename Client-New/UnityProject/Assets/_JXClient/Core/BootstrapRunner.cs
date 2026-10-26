using System;
using System.IO;
using JXClient.Domain;
using UnityEngine;
using UnityEngine.SceneManagement;

namespace JXClient.Core
{
    /// <summary>
    /// CN-00 Bootstrap entry point.
    /// Initializes offline test ground, camera, and status UI/console without connecting to GameServer.
    /// </summary>
    public sealed class BootstrapRunner : MonoBehaviour
    {
        [SerializeField] private Camera bootstrapCamera;
        [SerializeField] private Collider testGroundCollider;

        private readonly GUIStyle _boxStyle = new GUIStyle();
        private readonly GUIStyle _labelStyle = new GUIStyle();
        private bool _stylesInitialized;
        private int _renderedFrames;
        private bool _smokeQuitTriggered;

        public GameSessionState CurrentState { get; private set; } = GameSessionState.AppBoot;
        public bool IsBootstrapReady { get; private set; }
        public string StatusMessage { get; private set; } = "Initializing CN-00 Bootstrap...";

        private void Awake()
        {
            InitializeBootstrap();
        }

        public void InitializeBootstrap()
        {
            CurrentState = GameSessionState.AppBoot;

            if (bootstrapCamera == null)
            {
                bootstrapCamera = Camera.main;
            }

            if (testGroundCollider == null)
            {
                GameObject groundObj = GameObject.Find("BootstrapTestGround");
                if (groundObj != null)
                {
                    testGroundCollider = groundObj.GetComponent<Collider>();
                }
            }

            bool hasCamera = bootstrapCamera != null;
            bool hasGround = testGroundCollider != null && testGroundCollider.enabled;

            if (hasCamera && hasGround)
            {
                CurrentState = GameSessionState.OfflineReady;
                IsBootstrapReady = true;
                StatusMessage =
                    $"Gate CN-00 | Scene: {SceneManager.GetActiveScene().name} | State: {CurrentState} | " +
                    $"Unity: {Application.unityVersion} | GroundCollider: Ready | Network: Offline (No GS connection in CN-00)";
            }
            else
            {
                IsBootstrapReady = false;
                StatusMessage =
                    $"Gate CN-00 | INCOMPLETE: Camera={hasCamera}, GroundCollider={hasGround}";
            }

            Debug.Log($"[CN-00][BootstrapRunner] {StatusMessage}");
        }

        private void Update()
        {
            _renderedFrames++;
            if (!_smokeQuitTriggered && _renderedFrames >= 30 && HasCommandLineFlag("-cn00SmokeTest"))
            {
                _smokeQuitTriggered = true;
                TryCaptureSmokeScreenshot();
                Debug.Log($"[CN-00][BootstrapRunner] Smoke test verified after {_renderedFrames} frames. State={CurrentState}, Ready={IsBootstrapReady}. Quitting.");
                Application.Quit(IsBootstrapReady ? 0 : 2);
            }
        }

        private void TryCaptureSmokeScreenshot()
        {
            try
            {
                string[] args = Environment.GetCommandLineArgs();
                for (int i = 0; i < args.Length - 1; i++)
                {
                    if (string.Equals(args[i], "-cn00Screenshot", StringComparison.OrdinalIgnoreCase))
                    {
                        string path = args[i + 1];
                        string dir = Path.GetDirectoryName(path);
                        if (!string.IsNullOrEmpty(dir))
                        {
                            Directory.CreateDirectory(dir);
                        }
                        ScreenCapture.CaptureScreenshot(path);
                        Debug.Log($"[CN-00][BootstrapRunner] Requested screenshot capture to: {path}");
                        break;
                    }
                }
            }
            catch (Exception ex)
            {
                Debug.LogWarning($"[CN-00][BootstrapRunner] Screenshot capture skipped: {ex.Message}");
            }
        }

        private static bool HasCommandLineFlag(string flag)
        {
            string[] args = Environment.GetCommandLineArgs();
            for (int i = 0; i < args.Length; i++)
            {
                if (string.Equals(args[i], flag, StringComparison.OrdinalIgnoreCase))
                {
                    return true;
                }
            }
            return false;
        }

        private void OnGUI()
        {
            if (!_stylesInitialized)
            {
                _boxStyle.normal.textColor = Color.white;
                _boxStyle.fontSize = 15;
                _boxStyle.padding = new RectOffset(14, 14, 12, 12);

                _labelStyle.normal.textColor = new Color(0.88f, 0.96f, 1.0f, 1.0f);
                _labelStyle.fontSize = 14;
                _labelStyle.wordWrap = true;
                _stylesInitialized = true;
            }

            GUI.Box(new Rect(20, 20, 760, 175), "SwordOnline / JX1 3D — Client-New (CN-00 Bootstrap)");
            GUI.Label(
                new Rect(36, 54, 728, 125),
                $"Status: {(IsBootstrapReady ? "READY (OFFLINE BOOTSTRAP)" : "NOT READY")}\n" +
                $"Session State: {CurrentState} (MoveAllowed={GameSessionRules.CanSendMoveRequest(CurrentState)})\n" +
                $"Active Scene: {SceneManager.GetActiveScene().name} | Unity Version: {Application.unityVersion}\n" +
                $"Camera: {(bootstrapCamera != null ? bootstrapCamera.name : "Missing")} | " +
                $"Ground Collider: {(testGroundCollider != null ? testGroundCollider.name : "Missing")}\n" +
                "Network Mode: Disconnected by design (Gate CN-00 has no live GameServer/Bishop connection)",
                _labelStyle);
        }
    }
}
