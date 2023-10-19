#pragma once

#include "Entity.h"

namespace GameServer
{
	class Player final : public Entity
	{
	public:
		explicit Player(Vector3 position) : Entity(position) { }
		~Player() = default;
	};
}