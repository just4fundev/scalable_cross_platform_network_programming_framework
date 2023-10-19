// Copyright Cristian Pagán Díaz. All Rights Reserved.

#include "pch.h"

#include "Network/Include/Public/ConnectionFactory.h"
#include "Network/Include/Private/Connection.h"

Connection::IConnection* Connection::ConnectionFactory::Create(
    boost::asio::ip::tcp::socket* socket, 
    IConnectionHook* connectionHook, 
    size_t aplicationReceiveBuferSize, 
    size_t aplicationSendBufferSize, 
    size_t incrementBufferSizeMultiplier)
{
    return new Connection(socket, 
        connectionHook, 
        aplicationReceiveBuferSize, 
        aplicationSendBufferSize, 
        incrementBufferSizeMultiplier
    );
}