#pragma once

#include "ByteBuffer.h"

namespace Connection
{
	class ByteStreamOperator
	{
	public:
		ByteStreamOperator(ByteBuffer& byteBuffer) : m_ByteBuffer(byteBuffer), m_Position(0) { }
		virtual ~ByteStreamOperator() = default;

		bool Operate(void* dataPointer, size_t byteCount, bool move = false)
		{
			if (m_Position + byteCount > m_ByteBuffer.GetSize())
				return false;

			MemoryCopy(dataPointer, byteCount, move);

			m_Position += byteCount;

			return true;
		}

	protected:
		virtual void MemoryCopy(void* dataPointer, size_t byteCount, bool move) = 0;

		size_t m_Position;
		ByteBuffer& m_ByteBuffer;
	};
}