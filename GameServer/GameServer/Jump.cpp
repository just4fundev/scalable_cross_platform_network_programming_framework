#include "Jump.h"
#include "MatchSession.h"

using namespace GameServer;

const float Jump::k_Time = 3.5f;

Jump::Jump(IMatchState& matchState, MatchSession* jumper, const size_t destinationMap, const Vector2 destinationPosition) : m_MatchState(matchState), Jumper(jumper), m_Timer(k_Time), DestinationMap(destinationMap), DestinationPosition(destinationPosition), JumperID(jumper->GetID()) { }

bool Jump::Update(std::uint32_t timeDifference)
{
	const float deltaTime = timeDifference / 1000.0f;

	if (FloatComparison::LessEqual(m_Timer, 0.0f))
	{
		m_MatchState.SendSomeoneJumps(Jumper->GetID());

		return true;
	}

	m_Timer -= deltaTime;

	return false;
}