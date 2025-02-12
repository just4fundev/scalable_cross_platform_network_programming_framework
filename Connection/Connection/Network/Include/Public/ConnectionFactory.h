#pragma once

#include <boost/asio.hpp>

namespace Connection
{
	class ConnectionFactory
	{
	public:
		static std::shared_ptr<class IConnection> Create(
			boost::asio::ip::tcp::socket* socket,
			class IConnectionHook* connectionHook,
			class ISessionHook* sessionHook,
			size_t aplicationReceiveBuferSize,
			size_t aplicationSendBufferSize,
			size_t incrementBufferSizeMultiplier);
	};
}