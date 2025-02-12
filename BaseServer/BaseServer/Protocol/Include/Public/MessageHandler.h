#pragma once

#include <vector>
#include <cstdint>

namespace Connection
{
	class StreamReader;
}

#define MESSAGE_HANDLER_PARAMETER_LIST(TYPE) (TYPE*, Connection::StreamReader&, std::uint8_t*)
#define MESSAGE_HANDLER(TYPE, NAME) static bool NAME MESSAGE_HANDLER_PARAMETER_LIST(TYPE);

namespace BaseServer
{
	template<typename T> using MessageHandler = bool(*)MESSAGE_HANDLER_PARAMETER_LIST(T);
}