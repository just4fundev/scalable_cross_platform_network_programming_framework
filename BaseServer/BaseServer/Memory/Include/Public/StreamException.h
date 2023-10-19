// Copyright Cristian Pagán Díaz. All Rights Reserved.

#pragma once

#include <Exception.h>

namespace BaseServer
{
	class StreamException final : public Connection::Exception
	{
	public:
		StreamException(
			size_t byteBufferSize,
			const char* streamType,
			size_t streamPositon,
			size_t dataSize,
			boost::stacktrace::stacktrace&& stackTrace,
			const char* description) :
			m_ByteBufferSize(byteBufferSize),
			m_StreamType(streamType),
			m_StreamPositon(streamPositon),
			m_DataSize(dataSize),
			Exception(description, std::move(stackTrace)) { }

		~StreamException() = default;

		size_t GetByteBufferSize() const { return m_ByteBufferSize; }
		const char* GetStreamType() const { return m_StreamType; }
		size_t GetStreamPositon() const { return m_StreamPositon; }
		size_t GetDataSize() const { return m_DataSize; }

	private:
		size_t m_ByteBufferSize;
		const char* m_StreamType;
		size_t m_StreamPositon;
		size_t m_DataSize;
	};
}