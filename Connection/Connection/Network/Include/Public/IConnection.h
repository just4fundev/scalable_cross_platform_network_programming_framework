#pragma once

namespace Connection
{
	class IConnection
	{
	public:
		virtual void Send(class ByteBuffer&& sendingMessage) = 0;
		virtual void Close() = 0;
		virtual bool IsClosed() const = 0;
		virtual bool Receive(class PreprocessedMessage*& receivedMessage) = 0;

		virtual ~IConnection() = default;
	};
}