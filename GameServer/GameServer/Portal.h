#pragma once

#include "Vector2.h"

namespace GameServer
{
	class Portal final
	{
	public:
		Portal(
			const Vector2 position, 
			const size_t destinationMap, 
			const Vector2 destinationPosition) : 
			Position(position),
			DestinationMap(destinationMap),
			DestinationPosition(destinationPosition) { }

		~Portal() = default;

		static constexpr float Radius = 10.0f;

		const Vector2 Position;

		const size_t DestinationMap;
		const Vector2 DestinationPosition;
	};
}