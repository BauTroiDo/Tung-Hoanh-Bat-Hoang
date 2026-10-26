using UnityEngine;
using UnityEngine.InputSystem;
namespace JXClient.Movement
{
    public sealed class OrbitFollowCamera : MonoBehaviour
    {
        public Transform Target;
        public float Yaw = 0, Pitch = 50, Distance = 14;
        public void SetOrbit(float yaw, float pitch, float distance)
        { Yaw = yaw; Pitch = Mathf.Clamp(pitch, 25, 75); Distance = Mathf.Clamp(distance, 6, 24); }
        void LateUpdate()
        {
            if (Mouse.current != null)
            {
                float yaw = Yaw, pitch = Pitch;
                if (Mouse.current.rightButton.isPressed)
                { var delta = Mouse.current.delta.ReadValue(); yaw += delta.x * .2f; pitch -= delta.y * .15f; }
                SetOrbit(yaw, pitch, Distance - Mouse.current.scroll.ReadValue().y * .01f);
            }
            ApplyPose();
        }
        public void ApplyPose()
        {
            if (!Target) return;
            Vector3 focus = Target.position + Vector3.up;
            transform.position = focus + Quaternion.Euler(Pitch,Yaw,0) * new Vector3(0,0,-Distance);
            transform.LookAt(focus);
        }
    }
}
