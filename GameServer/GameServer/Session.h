#pragma once

#include <IConnection.h>
#include <MessageHandlerRegistry.h>
#include <OutgoingMessage.h>
#include <Opcodes.h>

#include "ISessionExtensionAccessor.h"
#include "SessionExtensionAccessor.h"

namespace GameServer
{
	class SessionWrapper;

	class Session final : public ISessionExtensionAccessor
	{
	public:
		explicit Session(
			std::shared_ptr<Connection::IConnection> connection,
			SessionExtensionAccessor* sessionExtensionAccessor,
			BaseServer::MessageHandlerRegistry<SessionWrapper>& messageHandlerRegistry,
			const size_t maxProccessedPacketsPerSessionUpdate) :
			m_Connection(connection),
			m_SessionExtensionAccessor(sessionExtensionAccessor),
			m_MessageHandlerRegistry(messageHandlerRegistry),
			m_MaxProccessedPacketsPerSessionUpdate(maxProccessedPacketsPerSessionUpdate) { }

		~Session()
		{
			delete m_SessionExtensionAccessor;
		}

		bool Update(std::uint32_t timeDifference, SessionWrapper* sessionWrapper);

		void Send(BaseServer::OutgoingMessage&& message)
		{
			m_Connection->Send(std::move(message));
		}

		void Close()
		{
			m_Connection->Close();
		}

		bool IsConnectionIdle() const override
		{
			return m_SessionExtensionAccessor->IsConnectionIdle();
		}

	private:
		BaseServer::MessageHandlerRegistry<SessionWrapper>& m_MessageHandlerRegistry;
		const size_t m_MaxProccessedPacketsPerSessionUpdate;

		std::shared_ptr<Connection::IConnection> m_Connection;
		SessionExtensionAccessor* m_SessionExtensionAccessor;

	};
}