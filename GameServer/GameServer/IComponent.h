// Copyright Cristian Pagán Díaz. All Rights Reserved.

#pragma once

namespace GameServer
{
	class IComponent
	{
	public:
		virtual void OnUpdate(float deltaTime) = 0;

		virtual ~IComponent() = default;
	};
}