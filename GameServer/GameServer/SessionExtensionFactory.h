#pragma once

#include <memory>

namespace GameServer
{
	class SessionExtension;
	class SessionExtensionSubject;

	class SessionExtensionFactory final
	{
	public:
		SessionExtensionFactory(SessionExtensionSubject* subject);
		~SessionExtensionFactory();

		std::shared_ptr<SessionExtension> Create();

	private:
		SessionExtensionSubject* m_Subject;

	};
}