// Copyright Cristian Pagán Díaz. All Rights Reserved.

using CppPlugins;

namespace Client
{
    namespace Network
    {
        public class Connection
        {
            private ConnectionManager m_ConnectionManager;
            private ulong m_ConnectionId;

            public Connection(ConnectionManager connectionManager, ulong connectionId)
            {
                m_ConnectionManager = connectionManager;
                m_ConnectionId = connectionId;
            }

            public bool Send(OutgoingMessage outgoingMessage)
            {
                return m_ConnectionManager.SendTo(m_ConnectionId, outgoingMessage);
            }

            public IncomingMessage Receive()
            {
                return m_ConnectionManager.ReceiveFrom(m_ConnectionId);
            }

            public bool Close()
            {
                return m_ConnectionManager.CloseConnection(m_ConnectionId);
            }

            public bool IsClosed()
            {
                return m_ConnectionManager.ConnectionClosed(m_ConnectionId);
            }
        }
    }
}