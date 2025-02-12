#pragma once

namespace GameServer
{
	class ICollectableDespawner
	{
	public:
		virtual void Despawn() = 0;

		virtual ~ICollectableDespawner() = default;
	};
}