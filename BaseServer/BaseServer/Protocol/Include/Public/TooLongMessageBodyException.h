// Copyright Cristian Pagán Díaz. All Rights Reserved.

#pragma once

#include <Exception.h>

namespace BaseServer
{
	class TooLongMessageBodyException final : public Connection::Exception
	{
	public:
		TooLongMessageBodyException(const char* description, boost::stacktrace::stacktrace&& stackTrace) : Connection::Exception(description, std::move(stackTrace)) { }

		~TooLongMessageBodyException() = default;
	};
}