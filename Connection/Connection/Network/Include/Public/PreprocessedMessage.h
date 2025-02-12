#pragma once

#include "ByteBuffer.h"

namespace Connection
{
	class PreprocessedMessage final
	{
	public:
		PreprocessedMessage(ByteBuffer&& data);
		PreprocessedMessage(PreprocessedMessage&& other) noexcept;
		~PreprocessedMessage();

		static const size_t MetadataLength = 4;

		ByteBuffer Data;
		std::uint8_t Metadata[MetadataLength];

	};
}