// Copyright Cristian Pagán Díaz. All Rights Reserved.

using NetworkSpawning;
using System.IO;
using UnityEngine;

namespace Game
{
    public abstract class Entity : NetworkDeserializable
    {
        [SerializeField] private float m_Speed;

        private Vector3 m_Destination;
		private Vector3 m_Origin;
        private Vector3 m_Direction;
        private bool m_Moving;

        private void FixedUpdate()
        {
            if (!m_Moving)
                return;

            float movementDistance = m_Speed * Time.deltaTime;

            float distanceFromOrigin = Vector3.Distance(transform.position, m_Origin);
            float travelDistance = Vector3.Distance(m_Origin, m_Destination);

            if (distanceFromOrigin < travelDistance)
            {
                float distanceToDestination = Vector3.Distance(transform.position, m_Destination);

                if (movementDistance < distanceToDestination)
                {
                    transform.position += m_Direction * movementDistance;
                }
                else
                {
                    transform.position = m_Destination;
                    m_Moving = false;
                }
            }
        }

        public virtual void MoveTo(Vector3 position)
        {
            if (transform.position == position)
                return;

            m_Origin = transform.position;
            m_Destination = position;

            m_Direction = m_Destination - m_Origin;
            m_Direction = Vector3.Normalize(m_Direction);

            m_Moving = true;
        }

        protected override void Deserialize(BinaryReader reader)
        {
            Vector3 position = Vector3.zero;

            position.x = reader.ReadSingle();
            position.y = reader.ReadSingle();
            position.z = reader.ReadSingle();

            transform.position = position;

            m_Destination.x = reader.ReadSingle();
            m_Destination.y = reader.ReadSingle();
            m_Destination.z = reader.ReadSingle();

            MoveTo(m_Destination);
        }
    }
}