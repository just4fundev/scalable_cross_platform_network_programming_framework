#include "pch.h"

#include "Network/Include/Public/ConnectionFactory.h"
#include "Network/Include/Private/Connection.h"

std::shared_ptr<Connection::IConnection> Connection::ConnectionFactory::Create(
    boost::asio::ip::tcp::socket* socket, 
    IConnectionHook* connectionHook, 
    ISessionHook* sessionHook,
    size_t aplicationReceiveBuferSize, 
    size_t aplicationSendBufferSize, 
    size_t incrementBufferSizeMultiplier)
{
    return std::make_shared<Connection>(
        socket, 
        connectionHook, 
        sessionHook,
        aplicationReceiveBuferSize, 
        aplicationSendBufferSize, 
        incrementBufferSizeMultiplier
    );
}