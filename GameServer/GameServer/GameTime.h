#pragma once

#include <ctime>
#include <cstdint>

namespace GameServer
{
	class GameTime final
	{
	public:
		static time_t GetClockSeconds();

	private:
		GameTime() = delete;
		GameTime(const GameTime&) = delete;
		GameTime(GameTime&&) = delete;
		~GameTime() = delete;

	};
}
