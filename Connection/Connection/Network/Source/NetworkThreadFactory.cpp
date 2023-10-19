// Copyright Cristian Pagán Díaz. All Rights Reserved.

#include "pch.h"

#include "Network/Include/Public/NetworkThreadFactory.h"
#include "Network/Include/Private/NetworkThread.h"

Connection::INetworkThread* Connection::NetworkThreadFactory::Create()
{
    return new NetworkThread();
}