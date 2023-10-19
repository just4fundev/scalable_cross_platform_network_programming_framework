// Copyright Cristian Pagán Díaz. All Rights Reserved.

#pragma once

#include <boost/asio.hpp>

#include <cstdint>
#include <memory>

namespace Connection
{
	class INetworkThread
	{
    public:
        virtual void Stop() = 0;
        virtual bool Start() = 0;
        virtual bool Started() const = 0;
        virtual void Wait() = 0;
        virtual void AddConnection(std::shared_ptr<class IConnection> connection) = 0;
        virtual std::uint32_t GetConnectionsCount() const = 0;
        virtual boost::asio::ip::tcp::socket&& GetAcceptSocket() = 0;

        virtual ~INetworkThread() = default;
	};
}