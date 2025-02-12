#pragma once

#include <cstdint>

namespace GameServer
{
	class ICollectableSpawner
	{
	public:
		virtual size_t Update(std::uint32_t timeDifference) = 0;
		virtual class Collectable* Spawn(size_t id) = 0;

		virtual ~ICollectableSpawner() = default;
	};
}