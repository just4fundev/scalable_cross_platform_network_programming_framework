// Copyright Cristian Pagán Díaz. All Rights Reserved.

#pragma once

#include "IShape.h"
#include "Ray.h"
#include "Utilities.h"

namespace GameServer
{
	class Rect final : public IShape
	{
	public:
		Rect(const Vector2& min, const Vector2& dimension) : Min(min), Max(min + dimension) { }

		bool Intersection(const Ray& rt, class Vector2& hit) const override
		{
			float tMinX = (Min.x - rt.Origin.x) / rt.Direction.x;
			float tMaxX = (Max.x - rt.Origin.x) / rt.Direction.x;

			if (FloatComparison::Greater(tMinX, tMaxX))
				Swap(tMinX, tMaxX);

			float tMinY = (Min.y - rt.Origin.y) / rt.Direction.y;
			float tMaxY = (Max.y - rt.Origin.y) / rt.Direction.y;

			if (FloatComparison::Greater(tMinY, tMaxY))
				Swap(tMinY, tMaxY);

			if (FloatComparison::Greater(tMinX, tMaxY) || FloatComparison::Greater(tMinY, tMaxX))
				return false;

			float tClosest = FloatComparison::Greater(tMinX, tMinY) ? tMinX : tMinY;

			hit = rt.Origin + rt.Direction * tClosest;
			return rt.Inside(hit);
		}

		const Vector2 Min;
		const Vector2 Max;
	};
}