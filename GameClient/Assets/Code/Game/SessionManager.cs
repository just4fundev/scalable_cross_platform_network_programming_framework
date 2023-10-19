// Copyright Cristian Pagán Díaz. All Rights Reserved.

using System.Collections.Generic;
using UnityEngine;
using Client.Network;

namespace Game
{
    public class SessionManager : MonoBehaviour
    {
        private List<Session> m_Sessions;

        private void Awake()
        {
            DontDestroyOnLoad(gameObject);

            m_Sessions = new List<Session>();
        }

        private void Start()
        {
            Connector.NewConnectionEvent += NewConnectionHandler;
        }

        private void LateUpdate()
        {
            foreach (var session in m_Sessions)
            {
                if (session.Update())
                {
                    
                }
            }
        }

        private void NewConnectionHandler(Connection newConnection)
        {
            m_Sessions.Add(new WorldSession(newConnection));
        }
    }
}