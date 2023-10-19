// Copyright Cristian Pagán Díaz. All Rights Reserved.

#pragma once

#include "IShape.h"
#include "Ray.h"

namespace GameServer
{
	class Circle final : public IShape
	{
	public:
		Circle(const Vector2& center, float radius) : Center(center), Radius(radius) { }

		bool Intersection(const class Ray& ray, class Vector2& hit) const override
		{
			const Vector2 u = Center - ray.Origin;
			const Vector2 p = Vector2::Project(u, ray.Direction);

			const float distance = Vector2::Distance(u, p);

			if (FloatComparison::Greater(distance, Radius))
				return false;

			if (FloatComparison::Less(distance, Radius))
			{
				const float m = std::sqrt(Radius * Radius + distance * distance);

				hit = ray.Origin + p - ray.Direction * m;
				return ray.Inside(hit);
			}

			hit = ray.Origin + p;
			return ray.Inside(hit);
		}

		const Vector2 Center;
		const float Radius;
	};
}