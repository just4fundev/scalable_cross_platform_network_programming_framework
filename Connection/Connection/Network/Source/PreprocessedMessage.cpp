#include "PreprocessedMessage.h"
#include "ByteBuffer.h"

#include <memory>

Connection::PreprocessedMessage::PreprocessedMessage(PreprocessedMessage&& other) noexcept : Data(std::move(other.Data))
{
	std::memcpy(Metadata, other.Metadata, MetadataLength);
}

Connection::PreprocessedMessage::PreprocessedMessage(ByteBuffer&& data) : Data(std::move(data)), Metadata() { }
Connection::PreprocessedMessage::~PreprocessedMessage() = default;