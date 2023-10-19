// Copyright Cristian Pagán Díaz. All Rights Reserved.

using UnityEngine;

namespace Game
{
    public class PlayerController : Entity
    {
        [SerializeField] private Camera m_Camera;

        private WorldSession m_WorldSession;

        protected override void OnSpawn(object args)
        {
            m_WorldSession = (WorldSession)args;
        }

        private void Update()
        {
            if (Input.GetMouseButton(InputExtension.RightMouseButton))
            {
                Ray ray = m_Camera.ScreenPointToRay(Input.mousePosition);
                Plane plane = new Plane(Vector3.up, transform.position.y);

                float enter;
                if (plane.Raycast(ray, out enter))
                {
                    Vector3 destination = ray.GetPoint(enter);
                    MoveTo(destination);
                }
            }
        }

        public override void MoveTo(Vector3 destination)
        {
            base.MoveTo(destination);
            SendMovement(destination);
        }

        public void SendKeepAlive()
        {
#if UNITY_EDITOR
            if (m_WorldSession == null)
                return;
#endif

            m_WorldSession.SendKeepAlive();
        }

        public void SendMovement(Vector3 destination)
        {
#if UNITY_EDITOR
            if (m_WorldSession == null)
                return;
#endif

            m_WorldSession.SendMovement(destination);
        }
    }
}