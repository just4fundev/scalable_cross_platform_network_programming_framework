// Copyright Cristian Pagán Díaz. All Rights Reserved.

#pragma once

#include "Session.h"

namespace GameServer
{
	class SessionWrapper
	{
	public:
		SessionWrapper(Session* session, const size_t id) : m_Session(session), ID(id) { }

		virtual ~SessionWrapper()
		{
			if (m_Session != nullptr)
				delete m_Session;
		}

		bool Update(std::uint32_t timeDifference) { return m_Session->Update(timeDifference, this); }

		void Send(BaseServer::OutgoingMessage&& message)
		{
			m_Session->Send(std::move(message));
		}

		const size_t ID;

	protected:
		Session* m_Session;
	};
}

#define MESSAGE_HANDLER_DECLARATION(NAME) MESSAGE_HANDLER(GameServer::SessionWrapper, NAME)
#define MESSAGE_HANDLER_DEFINITION(CLASS, NAME, ACTION) bool CLASS::NAME(SessionWrapper * sessionWrapper, StreamReader & streamReader) {CLASS* session = static_cast<CLASS*>(sessionWrapper); ACTION}