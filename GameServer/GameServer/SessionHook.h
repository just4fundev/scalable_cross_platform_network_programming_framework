#pragma once

#include <ISessionHook.h>

#include <memory>

namespace Connection
{
	class ByteBuffer;
	class IReceivedQueue;
}

namespace GameServer
{
	class SessionExtension;

	class SessionHook final : public Connection::ISessionHook
	{
	public:
		SessionHook(std::shared_ptr<SessionExtension> sessionExtension);
		~SessionHook();

		bool IsValidHeader(Connection::MessageSize messageSize, Connection::Opcode opcode) override;
		void ProcessReceivedMessage(Connection::ByteBuffer&& message, Connection::IReceivedQueue& receivedQueue) override;

	private:
		std::shared_ptr<SessionExtension> m_SessionExtension;

	};
}