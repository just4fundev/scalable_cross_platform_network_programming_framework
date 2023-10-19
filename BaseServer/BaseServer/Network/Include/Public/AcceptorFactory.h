// Copyright Cristian Pagán Díaz. All Rights Reserved.

#pragma once

#include <cstdint>

#include <IConnectionHookFactory.h>

namespace BaseServer
{
	class AcceptorFactory
	{
	public:
		static class IAcceptor* Create(
			const class Connection::IConnectionHookFactory* connectionHookFactory,
			const std::uint16_t port,
			const char* bindIp,
			const size_t threadPoolSize,
			const size_t applicationReceiveBufferSize,
			const size_t applicationSendBufferSize,
			const size_t incrementBufferSizeMultiplier);
	};
}