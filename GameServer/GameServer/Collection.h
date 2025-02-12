#pragma once

#include <cstdint>

#include "Vector2.h"

namespace GameServer
{
	enum CollectionResult : std::uint8_t
	{
		CollectionResult_Collected,
		CollectionResult_Collecting,
		CollectionResult_CancelledBeforeCollecting,
		CollectionResult_CancelledWhileCollecting
	};

	class Collection final
	{
	public:
		Collection(class IMatchState& matchState, class MatchSession* collector, class Collectable* collectable, Vector2 position);
		~Collection() = default;

		CollectionResult Update(std::uint32_t timeDifference);

		class MatchSession * const Collector;
		const size_t CollectorID;

		const size_t CollectableID;
		class Collectable* CollectableInfo;

	private:
		float m_Timer;
		Vector2 m_Position;
		bool m_Collecting;

		class IMatchState& m_MatchState;

	};
}