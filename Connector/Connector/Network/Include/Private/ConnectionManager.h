#pragma once

#include <cstdint>

#include "Connector.h"

namespace Connector
{ 
    typedef void(__stdcall* OnConnectCallback)(bool, int, size_t);
    struct ConnectionManager;
}

typedef Connector::ConnectionManager TypeName;

#ifdef __cplusplus
extern "C" {
#endif
    Connector_API inline TypeName* ConnectionManagerConstructor(
        bool activeTCPNoDelay,
        bool tcpNoDelay,
        size_t applicationReceiveBufferSize,
        size_t applicationSendBufferSize,
        bool activeOperatingSystemReceiveBufferSize,
        std::uint32_t operatingSystemReceiveBufferSize,
        bool activeOperatingSystemSendBufferSize,
        std::uint32_t operatingSystemSendBufferSize,
        size_t incrementBufferSizeMultiplier,
        size_t threadPoolSize
    );

    Connector_API void ConnectionManagerDestructor(TypeName* thisPtr);
    Connector_API void ConnectionManagerStart(TypeName* thisPtr);
    Connector_API void ConnectionManagerStop(TypeName* thisPtr);
    Connector_API void ConnectionManagerAsyncConnect(TypeName* thisPtr, void* ipAddresses, size_t ipAddressesCount, std::uint16_t port, Connector::OnConnectCallback onConnectCallback);
    Connector_API bool ConnectionManagerSendTo(TypeName* thisPtr, size_t connectionId, std::uint8_t* bytes, size_t size);
    Connector_API bool ConnectionManagerReceiveFrom(TypeName* thisPtr, size_t connectionId, std::uint8_t*& bytes, size_t& size);
    Connector_API bool ConnectionManagerCloseConnection(TypeName* thisPtr, size_t id);
    Connector_API bool ConnectionManagerConnectionClosed(TypeName* thisPtr, size_t id);
#ifdef __cplusplus
}
#endif