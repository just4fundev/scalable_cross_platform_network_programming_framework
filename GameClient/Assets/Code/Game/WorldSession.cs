// Copyright Cristian Pagán Díaz. All Rights Reserved.

using Client.Network;
using Client.Protocol;
using CppPlugins;
using NetworkSpawning;
using System;
using System.Collections.Generic;
using UnityEngine;

namespace Game
{
    public sealed class WorldSession : Session
    {
        private MessageHandlerRegistry<WorldSession> m_MessageHandlerRegistry;
        private NetworkSpawner m_NetworkSpawner;

        private Dictionary<ulong, Player> m_Players;

        public WorldSession(Connection connection) : base(connection)
        {
            MessageHandler<WorldSession>[] messageHandlers = new MessageHandler<WorldSession>[ServerOpcode.ServerOpcodeSize.ToValue()];
            messageHandlers[ServerOpcode.AuthentificationChallenge.ToValue()] = HandleAuthentificationChallenge;
            messageHandlers[ServerOpcode.NewPlayer.ToValue()] = HandleNewPlayer;
            messageHandlers[ServerOpcode.WorldState.ToValue()] = HandleWorldState;
            messageHandlers[ServerOpcode.Movement.ToValue()] = HandleMovement;
            messageHandlers[ServerOpcode.Disconnection.ToValue()] = HandleDisconnection;
            m_MessageHandlerRegistry = new MessageHandlerRegistry<WorldSession>(messageHandlers);

            Dictionary<Type, NetworkResource> networkResources = new Dictionary<Type, NetworkResource>();
            networkResources.Add(typeof(PlayerController), new NetworkResource("PlayerController", "PlayerController"));
            networkResources.Add(typeof(Player), new NetworkResource("Player", "Player"));
            m_NetworkSpawner = new NetworkSpawner(networkResources);

            m_Players = new Dictionary<ulong, Player>();
        }

        private void SendAuthentificationChallenge()
        {
            OutgoingMessage outgoingMessage = new OutgoingMessage(OpcodeExtension.OpcodeSize + sizeof(ulong));
            outgoingMessage.BinaryWriter.Write(ClientOpcode.AuthentificationChallenge.ToValue());
            outgoingMessage.BinaryWriter.Write(0ul);
            m_Connection.Send(outgoingMessage);
        }

        public void SendMovement(Vector3 destination)
        {
            OutgoingMessage outgoingMessage = new OutgoingMessage(OpcodeExtension.OpcodeSize + sizeof(float) * 3);
            outgoingMessage.BinaryWriter.Write(ClientOpcode.Movement.ToValue());
            outgoingMessage.BinaryWriter.Write(destination.x);
            outgoingMessage.BinaryWriter.Write(destination.y);
            outgoingMessage.BinaryWriter.Write(destination.z);

            m_Connection.Send(outgoingMessage);
        }

        public void SendKeepAlive()
        {
                
        }

        protected override void ProcessIncomingMessage(ServerOpcode opcode, IncomingMessage incomingMessage)
        {
            MessageHandler<WorldSession> messageHandler = m_MessageHandlerRegistry.GetMessageHandler(opcode.ToValue());
            messageHandler(this, incomingMessage);
        }

        private static void HandleAuthentificationChallenge(WorldSession session, IncomingMessage incomingMessage)
        {
            session.SendAuthentificationChallenge();
        }

        private static void HandleNewPlayer(WorldSession session, IncomingMessage incomingMessage)
        {
            ulong id = incomingMessage.BinaryReader.ReadUInt64();
            Player newPlayer = session.m_NetworkSpawner.Spawn<Player>(incomingMessage.BinaryReader, null);
            session.m_Players.Add(id, newPlayer);
        }

        private static void HandleWorldState(WorldSession session, IncomingMessage incomingMessage)
        {
            session.m_NetworkSpawner.Spawn<PlayerController>(incomingMessage.BinaryReader, session);

            ulong playersCount = incomingMessage.BinaryReader.ReadUInt64();

            for (ulong i = 0; i < playersCount; i++) 
            {
                ulong id = incomingMessage.BinaryReader.ReadUInt64();
                Player player = session.m_NetworkSpawner.Spawn<Player>(incomingMessage.BinaryReader, null);
                session.m_Players.Add(id, player);
            }
        }

        private static void HandleMovement(WorldSession session, IncomingMessage incomingMessage)
        {
            ulong id = incomingMessage.BinaryReader.ReadUInt64();
            Entity player = session.m_Players[id];

            Vector3 position = Vector3.zero;
            position.x = incomingMessage.BinaryReader.ReadSingle();
            position.y = incomingMessage.BinaryReader.ReadSingle();
            position.z = incomingMessage.BinaryReader.ReadSingle();

            player.MoveTo(position);
        }

        private static void HandleDisconnection(WorldSession session, IncomingMessage incomingMessage)
        {
            ulong id = incomingMessage.BinaryReader.ReadUInt64();
            Player player = session.m_Players[id];
            session.m_Players.Remove(id);
            UnityEngine.Object.Destroy(player.gameObject);
        }
    }
}