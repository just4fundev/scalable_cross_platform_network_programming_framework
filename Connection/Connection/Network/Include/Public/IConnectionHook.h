#pragma once

#include <boost/asio.hpp>

#include "IReceivedQueue.h"

namespace Connection
{
	class IConnectionHook : public IReceivedQueue
	{
	public:
		virtual bool OnStart(boost::asio::ip::tcp::socket* socket) = 0;

		virtual bool DequeueReceivedMessage(class PreprocessedMessage*& receivedMessage) = 0;

		virtual void EnqueueSendingMessage(class ByteBuffer&& sendingMessage) = 0;
		virtual bool DequeueSendingMessage(class ByteBuffer*& sendingMessage) = 0;

		virtual ~IConnectionHook() = default;
	};
}