// Copyright Cristian Pagán Díaz. All Rights Reserved.

#pragma once

namespace GameServer
{
	class IShape
	{
	public:
		virtual bool Intersection(const class Ray& ray, class Vector2& hit) const = 0;

		virtual ~IShape() = default;
	};
}