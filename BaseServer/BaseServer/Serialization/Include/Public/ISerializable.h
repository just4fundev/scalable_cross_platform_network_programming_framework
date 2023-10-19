// Copyright Cristian Pagán Díaz. All Rights Reserved.

#pragma once

namespace BaseServer
{
	class ISerializable
	{
	public:
		virtual void Serialize(class StreamWriter& streamWriter) const = 0;
		virtual size_t GetSize() const = 0;

		virtual ~ISerializable() = default;
	};
}