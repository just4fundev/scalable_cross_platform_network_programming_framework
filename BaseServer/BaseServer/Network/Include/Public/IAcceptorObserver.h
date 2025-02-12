#pragma once

#include <memory>

#include <IConnection.h>

namespace BaseServer
{
	class IAcceptorObserver
	{
	public:
		virtual void NotifyConnection(std::shared_ptr<Connection::IConnection> newConnection) = 0;

		virtual ~IAcceptorObserver() = default;
	};
}