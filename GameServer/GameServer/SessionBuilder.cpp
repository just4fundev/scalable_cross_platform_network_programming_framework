#include "SessionBuilder.h"
#include "ISessionObserver.h"
#include "Session.h"
#include "SessionExtensionAccessor.h"

GameServer::SessionBuilder::SessionBuilder(BaseServer::MessageHandlerRegistry<SessionWrapper>& messageHandlerRegistry) :
	m_MessageHandlerRegistry(messageHandlerRegistry),
	m_MaxProccessedPacketsPerSessionUpdate(100),
	m_SessionExtension(nullptr) { }

GameServer::SessionBuilder::~SessionBuilder() = default;

void GameServer::SessionBuilder::SubscribeObserver(ISessionObserver* observer)
{
	std::lock_guard<std::mutex> lock(m_Mutex);

	m_Observers.push_back(observer);
}

void GameServer::SessionBuilder::UnsubscribeObserver(ISessionObserver* observer)
{
	std::lock_guard<std::mutex> lock(m_Mutex);

	static auto removeDecision = [&observer](ISessionObserver* item) {
		if (item == observer)
			return true;

		return false;
	};

	m_Observers.erase(std::remove_if(m_Observers.begin(), m_Observers.end(), removeDecision), m_Observers.end());
}

void GameServer::SessionBuilder::NotifySessionExtension(std::shared_ptr<SessionExtension> sessionExtension)
{
	std::lock_guard<std::mutex> lock(m_Mutex);

	m_SessionExtension = sessionExtension;
}

void GameServer::SessionBuilder::NotifyConnection(std::shared_ptr<Connection::IConnection> connection)
{
	std::lock_guard<std::mutex> lock(m_Mutex);

	std::shared_ptr<SessionExtension> sessionExtension = m_SessionExtension;
	m_SessionExtension = nullptr;

	SessionExtensionAccessor* sessionExtensionAccessor = new SessionExtensionAccessor(sessionExtension);
	Session* session = new Session(connection, sessionExtensionAccessor, m_MessageHandlerRegistry, m_MaxProccessedPacketsPerSessionUpdate);
	
	for (ISessionObserver* observer : m_Observers)
		observer->NotifySession(session);
}