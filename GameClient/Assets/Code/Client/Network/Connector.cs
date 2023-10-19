// Copyright Cristian Pagán Díaz. All Rights Reserved.

using CppPlugins;
using UnityEngine;

namespace Client
{
    namespace Network
    {
        public class Connector : MonoBehaviour
        {
            public delegate void NewConnectionHandler(Connection newConnection);
            public static event NewConnectionHandler NewConnectionEvent;

            private readonly object m_Mutex = new object();

            private ConnectionManager m_ConnectionManager;

            private string[] m_IPAddresses;
            private ushort m_Port;

            private bool m_ConnectionResult;

            private bool m_Error;
            private int m_Code;
            private ulong m_ConnectionId;

            public static void Instantiate(string name, ConnectionManager connectionManager, string[] ipAddresses, ushort port)
            {
                GameObject newGameObject = new GameObject(name);
                newGameObject.transform.SetParent(connectionManager.transform, false);

                Connector connector = newGameObject.AddComponent<Connector>();
                connector.m_ConnectionManager = connectionManager;
                connector.m_IPAddresses = ipAddresses;
                connector.m_Port = port;
            }

            private void Awake()
            {
                m_ConnectionResult = false;
            }

            private void Start()
            {
                m_ConnectionManager.AsyncConnect(m_IPAddresses, m_Port, (error, code, connectionId) =>
                {
                    lock (m_Mutex) {
                        m_Error = error;
                        m_Code = code;
                        m_ConnectionId = connectionId;

                        m_ConnectionResult = true;
                    }
                });
            }

            private void Update()
            {
                lock (m_Mutex) {
                    if (!m_ConnectionResult)
                        return;

                    if (!m_Error)
                    {
                        Connection newConnection = new Connection(m_ConnectionManager, m_ConnectionId);
                        NewConnectionEvent?.Invoke(newConnection);
                    }
                    else
                    {

                    }

                    Destroy(gameObject);
                }
            }
        }
    }
}