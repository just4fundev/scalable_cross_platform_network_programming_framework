// Copyright Cristian Pagán Díaz. All Rights Reserved.

#pragma once

#include <boost/asio.hpp>

namespace Connection
{
	class IConnectionHook
	{
	public:
		virtual bool OnStart(boost::asio::ip::tcp::socket* socket) = 0;

		virtual void EnqueueReceivedMessage(class ByteBuffer&& sendingMessage) = 0;
		virtual bool DequeueReceivedMessage(class ByteBuffer*& receivedMessage) = 0;

		virtual void EnqueueSendingMessage(class ByteBuffer&& sendingMessage) = 0;
		virtual bool DequeueSendingMessage(class ByteBuffer*& sendingMessage) = 0;

		virtual ~IConnectionHook() = default;
	};
}