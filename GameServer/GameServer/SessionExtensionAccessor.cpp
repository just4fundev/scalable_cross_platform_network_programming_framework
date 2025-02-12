#include "SessionExtensionAccessor.h"
#include "SessionExtension.h"

GameServer::SessionExtensionAccessor::SessionExtensionAccessor(std::shared_ptr<SessionExtension> sessionExtension) : m_SessionExtension(sessionExtension) { }
GameServer::SessionExtensionAccessor::~SessionExtensionAccessor() = default;

bool GameServer::SessionExtensionAccessor::IsConnectionIdle() const
{
	return m_SessionExtension->IsConnectionIdle();
}