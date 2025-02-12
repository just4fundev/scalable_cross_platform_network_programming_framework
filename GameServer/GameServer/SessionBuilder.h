#pragma once

#include <mutex>
#include <vector>
#include <memory>

#include <IAcceptorObserver.h>
#include <MessageHandlerRegistry.h>

#include "ISessionExtensionObserver.h"

namespace GameServer
{
	class ISessionObserver;
	class Session;
	class SessionWrapper;

	class SessionBuilder final : public BaseServer::IAcceptorObserver, public ISessionExtensionObserver
	{
	public:
		SessionBuilder(BaseServer::MessageHandlerRegistry<SessionWrapper>& messageHandlerRegistry);
		~SessionBuilder();

		void SubscribeObserver(ISessionObserver* observer);
		void UnsubscribeObserver(ISessionObserver* observer);

	private:
		void NotifySessionExtension(std::shared_ptr<SessionExtension> sessionExtension) override;
		void NotifyConnection(std::shared_ptr<Connection::IConnection> connection) override;

		BaseServer::MessageHandlerRegistry<SessionWrapper>& m_MessageHandlerRegistry;
		const size_t m_MaxProccessedPacketsPerSessionUpdate;

		std::mutex m_Mutex;
		std::vector<ISessionObserver*> m_Observers;
		std::shared_ptr<SessionExtension> m_SessionExtension;

	};
}