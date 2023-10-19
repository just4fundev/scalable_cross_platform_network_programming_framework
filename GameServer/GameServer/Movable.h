// Copyright Cristian Pagán Díaz. All Rights Reserved.

#pragma once

#include "IComponent.h"

namespace GameServer
{
	class Movable final : public IComponent
	{
	public:
		Movable() = default;
		~Movable() = default;

		void OnUpdate(float deltaTime) override
		{

		}
	};
}