#pragma once

namespace Connection
{
	class StreamWriter;
}

namespace BaseServer
{
	class ISerializable
	{
	public:
		virtual void Serialize(Connection::StreamWriter& streamWriter) const = 0;
		virtual size_t GetSize() const = 0;

		virtual ~ISerializable() = default;
	};
}