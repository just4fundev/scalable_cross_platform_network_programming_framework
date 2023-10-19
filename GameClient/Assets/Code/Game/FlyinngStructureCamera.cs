// Copyright Cristian Pagán Díaz. All Rights Reserved.

using System.Collections.Generic;
using UnityEngine;

namespace Game
{
    public sealed class FlyinngStructureCamera : MonoBehaviour
    {
        [SerializeField] private Transform m_TargetTransform;
        [SerializeField] private LayerMask m_LayerMask;
        [SerializeField] private float m_Radius;

        private RaycastHit[] m_RaycastHits;
        private HashSet<FlyingStructure> m_Activating;
        private HashSet<FlyingStructure> m_Inactivating;
        private List<FlyingStructure> m_Removing;

        private Vector3 m_RayDirection;
        private float m_Distance;

        private bool m_Started;

        private void Awake()
        {
            m_Activating = new HashSet<FlyingStructure>();
            m_Inactivating = new HashSet<FlyingStructure>();

            m_Removing = new List<FlyingStructure>();
        }

        private void Start()
        {
            m_RayDirection = m_TargetTransform.position - transform.position;

            m_Distance = m_RayDirection.magnitude;
            m_RayDirection /= m_Distance;

            m_Started = true;
        }

        private void FixedUpdate()
        {
            m_RaycastHits = Physics.CapsuleCastAll(
                transform.position - m_RayDirection * m_Distance,
                m_TargetTransform.position,
                m_Radius,
                m_RayDirection,
                m_Distance * 2.0f,
                m_LayerMask.value
            );
        }

        private void Update()
        {
            m_Inactivating.Clear();
            m_Removing.Clear();

            foreach (RaycastHit raycastHit in m_RaycastHits)
                m_Inactivating.Add(raycastHit.collider.GetComponent<FlyingStructure>());

            foreach (FlyingStructure item in m_Activating)
            {
                if (!m_Inactivating.Contains(item))
                {
                    item.Appear();
                    m_Removing.Add(item);
                }
            }

            foreach (FlyingStructure item in m_Removing)
                m_Activating.Remove(item);

            foreach (FlyingStructure item in m_Inactivating)
            {
                if (!m_Activating.Contains(item))
                {
                    item.Disappear();
                    m_Activating.Add(item);
                }
            }
        }

        private void OnDrawGizmosSelected()
        {
            if (!m_Started)
                return;

            GizmosExtension.SaveColor();
            Gizmos.color = Color.gray;

            GizmosExtension.DrawWireCapsule(
                transform.position - m_RayDirection * m_Distance,
                m_TargetTransform.position,
                m_Radius
            );

            GizmosExtension.RestoreColor();
        }
    }
}