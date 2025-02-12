#pragma once

#include <ISessionHookFactory.h>

namespace Connector
{
	class SessionHook;

	class SessionHookFactory final : public Connection::ISessionHookFactory
	{
	public:
		SessionHookFactory();
		~SessionHookFactory();

		Connection::ISessionHook* Create() const override;

	};
}