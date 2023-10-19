// Copyright Cristian Pagán Díaz. All Rights Reserved.

#pragma once

#include <ByteBuffer.h>

namespace BaseServer
{
	class ByteStreamOperator
	{
	public:
		ByteStreamOperator(Connection::ByteBuffer& byteBuffer) : m_ByteBuffer(byteBuffer), m_Position(0) { }
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
		Connection::ByteBuffer& m_ByteBuffer;
	};
}