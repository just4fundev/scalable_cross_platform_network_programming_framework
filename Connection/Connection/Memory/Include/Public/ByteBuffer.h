// Copyright Cristian Pagán Díaz. All Rights Reserved.

#pragma once

#include <cstdint>
#include <memory>

namespace Connection
{
	class ByteBuffer
	{
	public:
		ByteBuffer() : m_Bytes(nullptr), m_Size(0) { }

		ByteBuffer(ByteBuffer&& other) noexcept : m_Bytes(other.m_Bytes), m_Size(other.m_Size)
		{
			other.m_Bytes = nullptr;
			other.m_Size = 0;
		}

		virtual ~ByteBuffer()
		{
			std::free(m_Bytes);
		}

		bool Resize(size_t size)
		{
			m_Size = size;
			m_Bytes = static_cast<uint8_t*>(std::realloc(m_Bytes, m_Size));

			return m_Bytes != nullptr;
		}

		size_t GetSize() const { return m_Size; }
		std::uint8_t* GetBytes() const { return m_Bytes; }
	protected:
		size_t m_Size;
		std::uint8_t* m_Bytes;
	};
}