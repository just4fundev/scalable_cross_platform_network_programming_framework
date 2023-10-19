// Copyright Cristian Pagán Díaz. All Rights Reserved.

#pragma once

#include <IConnection.h>
#include <MessageHandlerRegistry.h>
#include <OutgoingMessage.h>

#include "Opcodes.h"

namespace GameServer
{
	class SessionWrapper;

	class Session final
	{
	public:
		explicit Session(
			std::shared_ptr<Connection::IConnection> connection,
			BaseServer::MessageHandlerRegistry<SessionWrapper>& messageHandlerRegistry,
			const size_t maxProccessedPacketsPerSessionUpdate) :
			m_Connection(connection),
			m_MessageHandlerRegistry(messageHandlerRegistry),
			m_MaxProccessedPacketsPerSessionUpdate(maxProccessedPacketsPerSessionUpdate) { }

		~Session() = default;

		bool Update(std::uint32_t timeDifference, SessionWrapper* sessionWrapper);

		void Send(BaseServer::OutgoingMessage&& message)
		{
			m_Connection->Send(std::move(message));
		}

	private:
		BaseServer::MessageHandlerRegistry<SessionWrapper>& m_MessageHandlerRegistry;
		const size_t m_MaxProccessedPacketsPerSessionUpdate;

		std::shared_ptr<Connection::IConnection> m_Connection;
	};
}