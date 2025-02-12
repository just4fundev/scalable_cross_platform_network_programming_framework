#include "Collection.h"
#include "MatchSession.h"
#include "Collectable.h"

using namespace GameServer;

Collection::Collection(IMatchState& matchState, MatchSession* collector, Collectable* collectable, Vector2 position) : m_MatchState(matchState), Collector(collector), m_Timer(collectable->Time), CollectableInfo(collectable), CollectableID(collectable->ID), m_Position(position), m_Collecting(false), CollectorID(collector->GetID()) { }

CollectionResult Collection::Update(std::uint32_t timeDifference)
{
	const float deltaTime = timeDifference / 1000.0f;

	Player* collector = Collector->GetPlayer();
	Vector2 collectionPosition = CollectableInfo->Position + Vector2(0.0f, 0.88f);

	if (FloatComparison::Greater(Vector2::Distance(collector->GetPosition(), collectionPosition), 0.25f))
	{
		if (collector->GetDestination() != m_Position)
			return CollectionResult_CancelledBeforeCollecting;
	}
	else
	{
		if (!m_Collecting)
		{
			Collector->SendCollecting(CollectableID);
			m_Collecting = true;
		}

		if (collector->GetDestination() != m_Position)
			return CollectionResult_CancelledWhileCollecting;

		if (FloatComparison::LessEqual(m_Timer, 0.0f))
			return CollectionResult_Collected;
	}

	m_Timer -= deltaTime;

	return CollectionResult_Collecting;
}