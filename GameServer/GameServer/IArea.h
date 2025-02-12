#pragma once

#include "Vector2.h"

namespace GameServer
{
	class IArea
	{
	public:
		virtual Vector2 GetRandomPoint() const = 0;

		virtual ~IArea() = default;
	};
}