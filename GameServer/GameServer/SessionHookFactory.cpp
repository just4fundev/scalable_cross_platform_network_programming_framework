#include "SessionHookFactory.h"
#include "SessionHook.h"
#include "SessionExtensionFactory.h"

GameServer::SessionHookFactory::SessionHookFactory(SessionExtensionFactory* sessionExtensionFactory) : m_SessionExtensionFactory(sessionExtensionFactory) { }
GameServer::SessionHookFactory::~SessionHookFactory() = default;

Connection::ISessionHook* GameServer::SessionHookFactory::Create() const
{
	std::shared_ptr<SessionExtension> sessionExtension = m_SessionExtensionFactory->Create();
	return new SessionHook(sessionExtension);
}