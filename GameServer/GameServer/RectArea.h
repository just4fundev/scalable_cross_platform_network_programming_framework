#pragma once

#pragma once

#include <Random.h>

#include "IArea.h"

namespace GameServer
{
	class RectArea sealed : public IArea
	{
	public:
		RectArea(Vector2 center, float width, float height) : Center(center), Width(width), Height(height){ }
		~RectArea() = default;

		Vector2 GetRandomPoint() const override
		{
			const float halfWidth = Width / 2.0f;
			const float halfHeight = Height / 2.0f;

			const float x = Connection::Random::Range(-halfWidth, halfWidth);
			const float y = Connection::Random::Range(-halfHeight, halfHeight);

			return Vector2(x, y) + Center;
		}

		const Vector2 Center;
		const float Width;
		const float Height;
	};
}