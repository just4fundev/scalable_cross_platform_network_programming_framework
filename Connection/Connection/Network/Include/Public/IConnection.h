// Copyright Cristian Pagán Díaz. All Rights Reserved.

#pragma once

namespace Connection
{
	class IConnection
	{
	public:
		virtual void Start() = 0;
		virtual void Send(class ByteBuffer&& sendingMessage) = 0;
		virtual void Close() = 0;
		virtual bool IsClosed() const = 0;
		virtual bool Receive(class ByteBuffer*& receivedMessage) = 0;

		virtual ~IConnection() = default;
	};
}