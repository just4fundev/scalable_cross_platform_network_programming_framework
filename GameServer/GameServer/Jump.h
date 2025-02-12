#pragma once

#include <cstdint>

#include "Vector2.h"

namespace GameServer
{
	class Jump final
	{
	public:
		Jump(class IMatchState& matchState, class MatchSession* jumper, const size_t destinationMap, const Vector2 destinationPosition);
		~Jump() = default;

		bool Update(std::uint32_t timeDifference);

		const size_t JumperID;
		class MatchSession * const Jumper;

		const size_t DestinationMap;
		const Vector2 DestinationPosition;

	private:
		static const float k_Time;

		float m_Timer;

		class IMatchState& m_MatchState;

	};
}