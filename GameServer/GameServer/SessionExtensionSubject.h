#pragma once

#include <mutex>
#include <vector>
#include <memory>

namespace GameServer
{
	class ISessionExtensionObserver;
	class SessionExtension;

	class SessionExtensionSubject final
	{
	public:
		void Notify(std::shared_ptr<SessionExtension> sessionExtension);

		void SubscribeObserver(ISessionExtensionObserver* observer);
		void UnsubscribeObserver(ISessionExtensionObserver* observer);

	private:
		std::mutex m_ObserverSubjectMutex;
		std::vector<ISessionExtensionObserver*> m_Observers;

	};
}