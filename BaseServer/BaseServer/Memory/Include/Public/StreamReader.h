// Copyright Cristian Pagán Díaz. All Rights Reserved.

#pragma once

#include "ByteStreamReader.h"
#include "StreamException.h"

namespace BaseServer
{
    class StreamReader final : public ByteStreamReader
    {
    public:
        StreamReader(Connection::ByteBuffer& byteBuffer) : ByteStreamReader(byteBuffer) { }
        StreamReader() = default;

        StreamReader& operator >> (std::uint8_t& value)
        {
            PrimitiveOperate(value);

            return *this;
        }

        StreamReader& operator >> (std::int8_t& value)
        {
            PrimitiveOperate(value);

            return *this;
        }

        StreamReader& operator >> (std::uint16_t& value)
        {
            PrimitiveOperate(value);

            return *this;
        }

        StreamReader& operator >> (std::int16_t& value)
        {
            PrimitiveOperate(value);

            return *this;
        }

        StreamReader& operator >> (std::uint32_t& value)
        {
            PrimitiveOperate(value);

            return *this;
        }

        StreamReader& operator >> (std::int32_t& value)
        {
            PrimitiveOperate(value);

            return *this;
        }

        StreamReader& operator >> (std::uint64_t& value)
        {
            PrimitiveOperate(value);

            return *this;
        }

        StreamReader& operator >> (std::int64_t& value)
        {
            PrimitiveOperate(value);

            return *this;
        }

        StreamReader& operator >> (float& value)
        {
            PrimitiveOperate(value);

            return *this;
        }

        StreamReader& operator >> (double& value)
        {
            PrimitiveOperate(value);

            return *this;
        }

    private:
        template<typename T> void PrimitiveOperate(T& value)
        {
            if (!Operate(&value, sizeof(value)))
            {
                throw StreamException(
                    m_ByteBuffer.GetSize(),
                    "StreamReader",
                    m_Position,
                    sizeof(value),
                    std::move(boost::stacktrace::stacktrace()),
                    "StreamReader out of bounds exception");
            }
        }
    };
}