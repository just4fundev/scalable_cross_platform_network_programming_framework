#include "SessionHook.h"
#include "SessionExtension.h"
#include "Timer.h"

#include <IReceivedQueue.h>
#include <PreprocessedMessage.h>
#include <ByteBuffer.h>
#include <Opcodes.h>

#include <memory>

GameServer::SessionHook::SessionHook(std::shared_ptr<SessionExtension> sessionExtension) : m_SessionExtension(sessionExtension) { }
GameServer::SessionHook::~SessionHook() = default;

bool GameServer::SessionHook::IsValidHeader(Connection::MessageSize messageSize, Connection::Opcode opcode)
{
	return messageSize <= 14 && opcode < GameServerPublic::ClientOpcode::ClientOpcodeSize;
}

void GameServer::SessionHook::ProcessReceivedMessage(Connection::ByteBuffer&& message, Connection::IReceivedQueue& receivedQueue)
{
	bool enqueue = true;

	Connection::PreprocessedMessage preprocessedMessage(std::move(message));
	Connection::Opcode& opcode = *reinterpret_cast<Connection::Opcode*>(preprocessedMessage.Data.GetBytes());

	switch (opcode)
	{
	case GameServerPublic::C_KeepAlive:
		enqueue = false;
		break;

	case GameServerPublic::C_TimeRequest:
		std::uint32_t timeInMilliseconds = GetTimeInMilliseconds();
		std::memcpy(preprocessedMessage.Metadata, &timeInMilliseconds, sizeof(timeInMilliseconds));
		break;
	}

	if (enqueue)
		receivedQueue.EnqueueReceivedMessage(std::move(preprocessedMessage));

	m_SessionExtension->ResetTimeOut();
}