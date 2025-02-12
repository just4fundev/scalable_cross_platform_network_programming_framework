#pragma once

#include <OutgoingMessage.h>

namespace GameServer
{
	class Session;

	class Message
	{
	public:
		virtual ~Message();

	protected:
		Message(Session* destination, size_t size);

		BaseServer::OutgoingMessage& GetData() { return m_Data; }

	private:
		Message(const Message&) = delete;

		Session* m_Destination;
		BaseServer::OutgoingMessage m_Data;
	};
}