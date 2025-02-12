#pragma once

#include "ByteStreamWriter.h"
#include "StreamException.h"

namespace Connection
{
    class StreamWriter final : public ByteStreamWriter
    {
    public:
        StreamWriter(ByteBuffer& byteBuffer) : ByteStreamWriter(byteBuffer) { }
        ~StreamWriter() = default;

        StreamWriter& operator << (std::uint8_t value)
        {
            PrimitiveOperate(value);

            return *this;
        }

        StreamWriter& operator << (std::int8_t value)
        {
            PrimitiveOperate(value);

            return *this;
        }

        StreamWriter& operator << (std::uint16_t value)
        {
            PrimitiveOperate(value);

            return *this;
        }

        StreamWriter& operator << (std::int16_t value)
        {
            PrimitiveOperate(value);

            return *this;
        }

        StreamWriter& operator << (std::uint32_t value)
        {
            PrimitiveOperate(value);

            return *this;
        }

        StreamWriter& operator << (std::int32_t value)
        {
            PrimitiveOperate(value);

            return *this;
        }

        StreamWriter& operator << (std::uint64_t value)
        {
            PrimitiveOperate(value);

            return *this;
        }

        StreamWriter& operator << (std::int64_t value)
        {
            PrimitiveOperate(value);

            return *this;
        }

        StreamWriter& operator << (float value)
        {
            PrimitiveOperate(value);

            return *this;
        }

        StreamWriter& operator << (double value)
        {
            PrimitiveOperate(value);

            return *this;
        }

    private:
        template<typename T> void PrimitiveOperate(T value)
        {
            if (!Operate(&value, sizeof(value)))
            {
                throw StreamException(
                    m_ByteBuffer.GetSize(),
                    "StreamWriter",
                    m_Position,
                    sizeof(value),
                    std::move(boost::stacktrace::stacktrace()),
                    "StreamWriter out of bounds exception");
            }
        }
    };
}