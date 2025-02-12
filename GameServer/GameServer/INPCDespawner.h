#pragma once

namespace GameServer
{
	class INPCDespawner
	{
	public:
		virtual void Despawn() = 0;

		virtual ~INPCDespawner() = default;
	};
}