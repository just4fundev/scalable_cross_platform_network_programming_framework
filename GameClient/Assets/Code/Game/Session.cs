// Copyright Cristian Pagán Díaz. All Rights Reserved.

using Client.Network;
using CppPlugins;
using Client.Protocol;

namespace Game
{
    public abstract class Session
    {
        protected Connection m_Connection;

        protected byte[] m_OpcodeBuffer;
            
        public Session(Connection connection)
        {
            m_Connection = connection;

            m_OpcodeBuffer = new byte[OpcodeExtension.OpcodeSize];
        }

        public bool Update()
        {
            if (m_Connection.IsClosed())
                return false;

            IncomingMessage incomingMessage = m_Connection.Receive();
            while (incomingMessage != null)
            {
                incomingMessage.BinaryReader.Read(m_OpcodeBuffer, 0, OpcodeExtension.OpcodeSize);
                ServerOpcode opcode = (ServerOpcode)OpcodeExtension.ExtractOpcodeValueFromBytes(m_OpcodeBuffer);

                ProcessIncomingMessage(opcode, incomingMessage);

                incomingMessage.Dispose();

                incomingMessage = m_Connection.Receive();
            }

            return true;
        }

        protected abstract void ProcessIncomingMessage(ServerOpcode opcode, IncomingMessage incomingMessage);
    }
}