#include "pch.h"

#include "Network/Include/Private/SessionHookFactory.h"
#include "Network/Include/Private/SessionHook.h"

Connector::SessionHookFactory::SessionHookFactory() = default;
Connector::SessionHookFactory::~SessionHookFactory() = default;

Connection::ISessionHook* Connector::SessionHookFactory::Create() const
{
	return new SessionHook();
}