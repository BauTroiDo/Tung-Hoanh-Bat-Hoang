using System;
using System.Collections;
using UnityEngine;
using UnityEngine.UI;
namespace JXClient.Movement
{
    public sealed class MovementHud : MonoBehaviour
    {
        public OfflineMovement Player;
        public UnityEngine.UI.Text Label;
        void Awake() { Application.runInBackground = true; }
        void Update()
        {
            if (Player && Label) Label.text = "OFFLINE MOVEMENT TEST\nClick/tap ground to walk. Right-drag to orbit. Wheel to zoom.\n" +
                $"{Player.MotionState} | Speed {Player.ActualSpeed:F1} | Clicks {Player.AcceptedIntents}\n" +
                $"{Player.LastInput}\nPosition {Player.transform.position:F2}\nThis panel blocks ground clicks.";
        }
        IEnumerator Start()
        {
            if (Array.IndexOf(Environment.GetCommandLineArgs(), "-cn01SmokeTest") < 0) yield break;
            // Explicit automation evidence. No fabricated user/OS pointer claim.
            yield return null; yield return null;
            Vector3 start = Player.transform.position;
            Vector3 target = start + new Vector3(-3,0,2);
            Vector2 screen = Player.ViewCamera.WorldToScreenPoint(target);
            bool accepted = Player.TryPointer(screen);
            Debug.Log($"[CN-01] AUTOMATED_SMOKE start={start} accepted={accepted} target={target}");
            float deadline = Time.realtimeSinceStartup + 8;
            while (Player.IsMoving && Time.realtimeSinceStartup < deadline) yield return null;
            float distance = Vector3.Distance(Player.transform.position,target);
            bool passed = accepted && distance < .2f;
            Debug.Log($"[CN-01] AUTOMATED_SMOKE end={Player.transform.position} distance={distance:F3} passed={passed}");
            // Batchmode has no render loop guarantee; visual evidence comes from a rendered run.
            if (!Application.isBatchMode)
            {
                string[] args = Environment.GetCommandLineArgs();
                int i = Array.IndexOf(args,"-cn01Screenshot");
                if(i >= 0 && i + 1 < args.Length) { ScreenCapture.CaptureScreenshot(args[i+1]); yield return new WaitForEndOfFrame(); yield return null; }
            }
            Application.Quit(passed ? 0 : 2);
        }
    }
}
