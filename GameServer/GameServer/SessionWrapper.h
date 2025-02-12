#pragma once

#include "Session.h"
#include "ISessionExtensionAccessor.h"

namespace GameServer
{
	class SessionWrapper : public ISessionExtensionAccessor
	{
	public:
		SessionWrapper(Session* session, const size_t id) : m_Session(session), m_ID(id) { }

		virtual ~SessionWrapper()
		{
			if (m_Session != nullptr)
				delete m_Session;
		}

		virtual bool Update(std::uint32_t timeDifference) { return m_Session->Update(timeDifference, this); }
		virtual void Send(BaseServer::OutgoingMessage&& message) { m_Session->Send(std::move(message)); }
		virtual void Close() { m_Session->Close(); }

		bool IsConnectionIdle() const override
		{
			return m_Session->IsConnectionIdle();
		}

	protected:
		Session* m_Session;
		const size_t m_ID;
	};
}

#define MESSAGE_HANDLER_DECLARATION(NAME) MESSAGE_HANDLER(GameServer::SessionWrapper, NAME)
#define MESSAGE_HANDLER_DEFINITION(CLASS, NAME, ACTION) bool CLASS::NAME(SessionWrapper* sessionWrapper, StreamReader& data, std::uint8_t*) {CLASS* session = static_cast<CLASS*>(sessionWrapper); ACTION}