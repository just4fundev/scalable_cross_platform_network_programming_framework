// Copyright Cristian Pagán Díaz. All Rights Reserved.

#pragma once

#include "ByteStreamOperator.h"

namespace BaseServer
{
	class ByteStreamWriter : public ByteStreamOperator
	{
	public:
		ByteStreamWriter(Connection::ByteBuffer& byteBuffer) : ByteStreamOperator(byteBuffer) { }
		virtual ~ByteStreamWriter() = default;

		Connection::ByteBuffer& GetByteBuffer() { return m_ByteBuffer; }

	protected:
		void MemoryCopy(void* dataPointer, size_t byteCount, bool move) override
		{
			if (!move)
				std::memcpy(m_ByteBuffer.GetBytes() + m_Position, dataPointer, byteCount);
			else
				std::memmove(m_ByteBuffer.GetBytes() + m_Position, dataPointer, byteCount);
		}
	};
}