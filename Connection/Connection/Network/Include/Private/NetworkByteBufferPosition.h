// Copyright Cristian Pagán Díaz. All Rights Reserved.

#pragma once

namespace Connection
{
    class NetworkByteBufferPosition final
    {
    public:
        NetworkByteBufferPosition() : m_Value(0) { }

        NetworkByteBufferPosition(NetworkByteBufferPosition&& other) noexcept : m_Value(other.m_Value)
        {
            other.m_Value = 0;
        }

        size_t GetValue() const { return m_Value; }

        void Reset() { m_Value = 0; }

        void operator += (size_t value) { m_Value += value; }
        void operator -= (size_t value) { m_Value -= value; }
    private:
        size_t m_Value;
    };
}