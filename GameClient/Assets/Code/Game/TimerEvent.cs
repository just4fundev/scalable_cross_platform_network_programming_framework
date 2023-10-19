// Copyright Cristian Pagán Díaz. All Rights Reserved.

using UnityEngine;

namespace Game
{
    [RequireComponent(typeof(PlayerController))]
    public abstract class TimerEvent : MonoBehaviour
    {
        [SerializeField] protected float m_Time;
        [SerializeField] private bool m_Late;

        protected PlayerController m_PlayerController;

        private float m_Timer;

        private void Awake()
        {
            m_PlayerController = GetComponent<PlayerController>();

            ResetTimer();
        }

        private void Update()
        {
            if (m_Late)
                return;

            UpdateTimer();
        }

        private void LateUpdate()
        {
            if (!m_Late)
                return;

            UpdateTimer();
        }

        private void UpdateTimer()
        {
            m_Timer -= Time.deltaTime;

            if (m_Timer <= 0.0f)
            {
                Event();
                ResetTimer();
            }
        }

        private void ResetTimer()
        {
            m_Timer = m_Time;
        }

        protected abstract void Event();
    }
}