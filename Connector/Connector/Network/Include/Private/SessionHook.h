#pragma once

#include <ISessionHook.h>

namespace Connection
{
	class ByteBuffer;
	class IReceivedQueue;
}

namespace Connector
{
	class SessionHook final : public Connection::ISessionHook
	{
	public:
		SessionHook();
		~SessionHook();

		bool IsValidHeader(Connection::MessageSize messageSize, Connection::Opcode opcode) override;
		void ProcessReceivedMessage(Connection::ByteBuffer&& message, Connection::IReceivedQueue& receivedQueue) override;

	};
}