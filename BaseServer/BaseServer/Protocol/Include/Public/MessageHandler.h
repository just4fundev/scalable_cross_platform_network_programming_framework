// Copyright Cristian Pagán Díaz. All Rights Reserved.

#pragma once

#include <vector>

namespace BaseServer
{
	class StreamReader;
}

#define MESSAGE_HANDLER_PARAMETER_LIST(TYPE) (TYPE*, BaseServer::StreamReader&)
#define MESSAGE_HANDLER(TYPE, NAME) static bool NAME MESSAGE_HANDLER_PARAMETER_LIST(TYPE);

namespace BaseServer
{
	template<typename T> using MessageHandler = bool(*)MESSAGE_HANDLER_PARAMETER_LIST(T);
}