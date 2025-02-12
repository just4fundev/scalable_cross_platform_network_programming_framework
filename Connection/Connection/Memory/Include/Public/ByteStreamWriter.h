#pragma once

#include "ByteStreamOperator.h"

namespace Connection
{
	class ByteStreamWriter : public ByteStreamOperator
	{
	public:
		ByteStreamWriter(ByteBuffer& byteBuffer) : ByteStreamOperator(byteBuffer) { }
		virtual ~ByteStreamWriter() = default;

		ByteBuffer& GetByteBuffer() { return m_ByteBuffer; }

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