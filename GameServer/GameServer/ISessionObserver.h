#pragma once

namespace GameServer
{
	class Session;

	class ISessionObserver
	{
	public:
		virtual void NotifySession(Session* session) = 0;

		virtual ~ISessionObserver() = default;
	};
}