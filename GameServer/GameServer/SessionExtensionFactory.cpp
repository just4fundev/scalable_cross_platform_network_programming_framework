#include "SessionExtensionFactory.h"
#include "SessionExtension.h"
#include "SessionExtensionSubject.h"

GameServer::SessionExtensionFactory::SessionExtensionFactory(SessionExtensionSubject* subject) : m_Subject(subject) { }
GameServer::SessionExtensionFactory::~SessionExtensionFactory() = default;

std::shared_ptr<GameServer::SessionExtension> GameServer::SessionExtensionFactory::Create()
{
	auto sessionExtension = std::make_shared<SessionExtension>();
	m_Subject->Notify(sessionExtension);

	return sessionExtension;
}