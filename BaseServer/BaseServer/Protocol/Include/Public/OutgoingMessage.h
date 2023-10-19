// Copyright Cristian Pagán Díaz. All Rights Reserved.

#pragma once

#include <sstream>

#include <ByteBuffer.h>

#include "StreamWriter.h"
#include "TooLongMessageBodyException.h"

namespace BaseServer
{
	class OutgoingMessage final : public Connection::ByteBuffer
	{
	public:
		OutgoingMessage(std::uint16_t bodySize) : m_StreamWriter(*this)
		{
			std::uint16_t headerSize = sizeof(std::uint16_t);

			std::uint16_t maxBodySize = MAXUINT16 - headerSize;
			if (bodySize > maxBodySize)
			{
				std::ostringstream message;
				message << "The body size of the message (" << bodySize << ") should be less than " << maxBodySize;
				std::string str = message.str();

				throw new TooLongMessageBodyException(str.c_str(), std::move(boost::stacktrace::stacktrace()));
			}

			Resize(headerSize + bodySize);

			m_StreamWriter << bodySize;
		}

		~OutgoingMessage() = default;

		StreamWriter& GetStreamWriter() { return m_StreamWriter; }

	private:
		StreamWriter m_StreamWriter;
	};
}