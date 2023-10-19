// Copyright Cristian Pagán Díaz. All Rights Reserved.

#pragma once

#include <utility>

#include "NetworkByteBufferPosition.h"
#include "Memory/Include/Public/ByteBuffer.h"

namespace Connection
{
    class NetworkByteBuffer final
    {
    public:
        NetworkByteBuffer() : m_ByteBuffer() { }

        NetworkByteBuffer(NetworkByteBuffer&& other) noexcept :
            m_ByteBuffer(std::move(other.GetByteBuffer())),
            m_ReadPosition(std::move(other.m_ReadPosition)),
            m_WritePosition(std::move(other.m_WritePosition)) { }

        size_t CalculateRemainingSpace() const { return m_ByteBuffer.GetSize() - m_WritePosition.GetValue(); }
        size_t CalculateActiveSize() const { return m_WritePosition.GetValue() - m_ReadPosition.GetValue(); }

        ByteBuffer& GetByteBuffer() { return m_ByteBuffer; }

        NetworkByteBufferPosition& GetReadPosition() { return m_ReadPosition; }
        NetworkByteBufferPosition& GetWritePosition() { return m_WritePosition; }
    private:
        ByteBuffer m_ByteBuffer;

        NetworkByteBufferPosition m_ReadPosition;
        NetworkByteBufferPosition m_WritePosition;
    };
}