#pragma once

#include "ByteStreamOperator.h"

namespace Connection
{
	class ByteStreamReader : public ByteStreamOperator
	{
	public:
		ByteStreamReader(ByteBuffer& byteBuffer) : ByteStreamOperator(byteBuffer) { }
		virtual ~ByteStreamReader() = default;

	protected:
		void MemoryCopy(void* dataPointer, size_t byteCount, bool move) override
		{
			if (!move)
				std::memcpy(dataPointer, m_ByteBuffer.GetBytes() + m_Position, byteCount);
			else
				std::memmove(dataPointer, m_ByteBuffer.GetBytes() + m_Position, byteCount);
		}
	};
}