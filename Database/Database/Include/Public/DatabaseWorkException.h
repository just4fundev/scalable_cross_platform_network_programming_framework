#pragma once

#include <exception>

namespace Database
{
	class DatabaseWorkException final : public std::exception
	{
	public:
		DatabaseWorkException() : std::exception("") { }
		~DatabaseWorkException() = default;
	};
}