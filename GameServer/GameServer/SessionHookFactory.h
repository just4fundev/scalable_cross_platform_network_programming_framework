#pragma once

#include <ISessionHookFactory.h>

namespace GameServer
{
	class SessionHook;
	class SessionExtension;
	class SessionExtensionFactory;

	class SessionHookFactory final : public Connection::ISessionHookFactory
	{
	public:
		SessionHookFactory(SessionExtensionFactory* sessionExtensionFactory);
		~SessionHookFactory();

		Connection::ISessionHook* Create() const override;

	private:
		SessionExtensionFactory* m_SessionExtensionFactory;

	};
}