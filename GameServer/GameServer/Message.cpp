#include "Message.h"
#include "Session.h"

GameServer::Message::~Message()
{
	m_Destination->Send(std::move(m_Data));
}

GameServer::Message::Message(Session* destination, size_t size) : m_Destination(destination), m_Data(static_cast<std::uint16_t>(size)) { }