#pragma once

#include "ISessionHook.h"

namespace Connection
{
	class ISessionHookFactory
	{
	public:
		virtual ISessionHook* Create() const = 0;

		virtual ~ISessionHookFactory() = default;
	};
}