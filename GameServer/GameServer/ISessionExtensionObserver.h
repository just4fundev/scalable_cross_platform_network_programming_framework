#pragma once

#include <memory>

namespace GameServer
{
	class SessionExtension;

	class ISessionExtensionObserver
	{
	public:
		virtual void NotifySessionExtension(std::shared_ptr<SessionExtension> sessionExtension) = 0;

		virtual ~ISessionExtensionObserver() = default;

	};
}