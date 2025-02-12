#pragma once

#include "IConnectionHook.h"

namespace Connection
{
	class IConnectionHookFactory
	{
	public:
		virtual IConnectionHook* Create() const = 0;

		virtual ~IConnectionHookFactory() = default;
	};
}