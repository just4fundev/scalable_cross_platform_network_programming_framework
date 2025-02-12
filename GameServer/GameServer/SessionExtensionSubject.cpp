#include "SessionExtensionSubject.h"
#include "ISessionExtensionObserver.h"

void GameServer::SessionExtensionSubject::Notify(std::shared_ptr<SessionExtension> sessionExtension)
{
	std::lock_guard<std::mutex> lock(m_ObserverSubjectMutex);

	for (ISessionExtensionObserver* observer : m_Observers)
		observer->NotifySessionExtension(sessionExtension);
}

void GameServer::SessionExtensionSubject::SubscribeObserver(ISessionExtensionObserver* observer)
{
	std::lock_guard<std::mutex> lock(m_ObserverSubjectMutex);

	m_Observers.push_back(observer);
}

void GameServer::SessionExtensionSubject::UnsubscribeObserver(ISessionExtensionObserver* observer)
{
	std::lock_guard<std::mutex> lock(m_ObserverSubjectMutex);

	static auto removeDecision = [&observer](ISessionExtensionObserver* item) {
		if (item == observer)
			return true;

		return false;
	};

	m_Observers.erase(std::remove_if(m_Observers.begin(), m_Observers.end(), removeDecision), m_Observers.end());
}