// Copyright Cristian Pagán Díaz. All Rights Reserved.

#pragma once

#include "Vector2.h"

namespace GameServer
{
	class Ray final
	{
	public:
		Ray(const Vector2& origin, const Vector2& direction, float distance) : Origin(origin), Direction(Vector2::Normalized(direction)), Distance(distance) { }
		~Ray() = default;

		bool Inside(const Vector2& intersection) const
		{
			return FloatComparison::LessEqual(Vector2::Distance(Origin, intersection), Distance);
		}

		const Vector2 Origin;
		const Vector2 Direction;
		const float Distance;
	};
}