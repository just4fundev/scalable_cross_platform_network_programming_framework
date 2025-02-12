#pragma once

#include <cstdint>

namespace GameServer
{
	class INPCSpawner
	{
	public:
		virtual size_t Update(std::uint32_t timeDifference) = 0;
		virtual class Player* Spawn(size_t mapID) = 0;
		virtual std::string GetAI() = 0;

		virtual ~INPCSpawner() = default;
	};
}