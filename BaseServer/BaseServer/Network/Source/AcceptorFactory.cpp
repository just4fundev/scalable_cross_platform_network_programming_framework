#include "pch.h"

#include "Network/Include/Public/AcceptorFactory.h"
#include "Network/Include/Private/Acceptor.h"

BaseServer::IAcceptor* BaseServer::AcceptorFactory::Create(
	const class Connection::IConnectionHookFactory* connectionHookFactory,
	const class Connection::ISessionHookFactory* sessionHookFactory,
	const std::uint16_t port,
	const char* bindIp,
	const size_t threadPoolSize,
	const size_t applicationReceiveBufferSize,
	const size_t applicationSendBufferSize,
	const size_t incrementBufferSizeMultiplier)
{
	return new Acceptor(
		connectionHookFactory,
		sessionHookFactory,
		port,
		bindIp,
		threadPoolSize,
		applicationReceiveBufferSize,
		applicationSendBufferSize,
		incrementBufferSizeMultiplier
	);
}