#pragma once

#include <cstdint>

namespace GameServer
{
	enum AttackResult : std::uint8_t
	{
		AttackResult_Cancelled,
		AttackResult_AttackedDead,
		AttackResult_Attacking
	};

	class Attack final
	{
	public:
		Attack(class IMatchState& matchState, class IMatchSession& attacker, class IMatchSession& attacked, float attackRadius, float attackTime);
		~Attack() = default;

		AttackResult Update(std::uint32_t timeDifference);

		class IMatchSession& Attacked;

		const size_t AttackerID;
		const size_t AttackedID;

	private:
		static const int k_MissPercentage;

		float m_Timer;

		class IMatchState& m_MatchState;
		class IMatchSession& m_Attacker;

		const float m_Time;
		const float m_AttackRadius;

	};
}