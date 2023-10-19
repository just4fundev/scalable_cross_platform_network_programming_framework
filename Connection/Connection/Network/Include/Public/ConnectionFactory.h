// Copyright Cristian Pagán Díaz. All Rights Reserved.

#pragma once

#include <boost/asio.hpp>

namespace Connection
{
	class ConnectionFactory
	{
	public:
		static class IConnection* Create(
			boost::asio::ip::tcp::socket* socket,
			class IConnectionHook* connectionHook,
			size_t aplicationReceiveBuferSize,
			size_t aplicationSendBufferSize,
			size_t incrementBufferSizeMultiplier);
	};
}