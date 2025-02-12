#pragma once

#include <cmath>

#include <Random.h>

#include "IArea.h"

namespace GameServer
{
	class CircleArea sealed : public IArea
	{
	public:
		CircleArea(Vector2 position, float radius) : Position(position), Radius(radius) { }
		~CircleArea() = default;

		Vector2 GetRandomPoint() const override;

		const Vector2 Position;
		const float Radius;
	};
}