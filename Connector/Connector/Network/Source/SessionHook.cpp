#include "pch.h"

#include "Network/Include/Private/SessionHook.h"
#include "Time/Include/Private/Timer.h"

#include <IReceivedQueue.h>
#include <PreprocessedMessage.h>
#include <ByteBuffer.h>
#include <Opcodes.h>

#include <memory>

Connector::SessionHook::SessionHook() = default;
Connector::SessionHook::~SessionHook() = default;

bool Connector::SessionHook::IsValidHeader(Connection::MessageSize messageSize, Connection::Opcode opcode)
{
	return true;
}

void Connector::SessionHook::ProcessReceivedMessage(Connection::ByteBuffer&& message, Connection::IReceivedQueue& receivedQueue)
{
	Connection::PreprocessedMessage preprocessedMessage(std::move(message));
	Connection::Opcode& opcode = *reinterpret_cast<Connection::Opcode*>(preprocessedMessage.Data.GetBytes());

	switch (opcode)
	{
	case GameServerPublic::S_TimeResponse:
		std::uint32_t timeInMilliseconds = GetTimeInMilliseconds();
		size_t oldDataSize = preprocessedMessage.Data.GetSize();
		preprocessedMessage.Data.Resize(oldDataSize + sizeof(std::uint32_t));
		std::memcpy(preprocessedMessage.Data.GetBytes() + oldDataSize, &timeInMilliseconds, sizeof(timeInMilliseconds));
		break;
	}

	receivedQueue.EnqueueReceivedMessage(std::move(preprocessedMessage));
}