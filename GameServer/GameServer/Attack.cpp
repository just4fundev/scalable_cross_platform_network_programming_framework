#include "Attack.h"
#include "IMatchState.h"
#include "IMatchSession.h"
#include "FloatComparison.h"
#include "Vector2.h"
#include "Player.h"

#include <Random.h>

using namespace GameServer;

const int Attack::k_MissPercentage = 20;

Attack::Attack(IMatchState& matchState, IMatchSession& attacker, IMatchSession& attacked, float attackRadius, float attackTime) : m_MatchState(matchState), m_Attacker(attacker), Attacked(attacked), m_Timer(attackTime), AttackerID(attacker.GetID()), AttackedID(attacked.GetID()), m_AttackRadius(attackRadius), m_Time(attackTime) { }

AttackResult GameServer::Attack::Update(std::uint32_t timeDifference)
{
	const float deltaTime = timeDifference / 1000.0f;

	if (FloatComparison::LessEqual(m_Timer, 0.0f))
	{
		if (FloatComparison::LessEqual(Vector2::Distance(m_Attacker.GetPlayer()->GetPosition(), Attacked.GetPlayer()->GetPosition()), m_AttackRadius))
		{
			if (!Attacked.IsPvP())
				return AttackResult_Cancelled;

			std::uint32_t damage;

			if (Connection::Random::Range(0, 100) < k_MissPercentage)
				damage = 0;
			else
				damage = m_Attacker.GetPlayer()->CalculateDamage();

			std::int64_t shield = Attacked.GetPlayer()->Shield;

			if (shield > 0)
			{
				std::int64_t shieldDamage = static_cast<std::int64_t>(damage * Attacked.GetPlayer()->Absortion);
				shield -= shieldDamage;
			}

			std::int64_t hpDamage;
			if (shield > 0)
			{
				hpDamage = static_cast<std::int64_t>(damage * (1.0f - Attacked.GetPlayer()->Absortion));
			}
			else if (shield < 0)
			{
				hpDamage = static_cast<std::int64_t>(damage * (1.0f - Attacked.GetPlayer()->Absortion)) + std::abs(shield);
				shield = 0;
			}
			else
			{
				hpDamage = static_cast<std::int64_t>(damage);
			}

			std::int64_t hp = Attacked.GetPlayer()->HP;
			hp -= hpDamage;

			std::uint32_t damageNotification;
			if (hp >= 0)
			{
				damageNotification = damage;
			}
			else
			{
				damageNotification = static_cast<std::uint32_t>(std::abs(hp));
				hp = 0;
			}

			Attacked.GetPlayer()->Shield = static_cast<std::uint32_t>(shield);
			Attacked.GetPlayer()->HP = static_cast<std::uint32_t>(hp);

			m_Attacker.SendDamageDone(Attacked.GetID(), damageNotification);

			if (Attacked.GetPlayer()->HP == 0)
			{
				Attacked.SendYouDie();

				return AttackResult_AttackedDead;
			}

			m_MatchState.SendStatusUpdate(Attacked.GetID());
			Attacked.SendDamageReceived(damage);
		}
		else
		{
			m_Attacker.SendOutOfRange();
		}

		m_Timer = m_Time;
	}

	m_Timer -= deltaTime;

	return AttackResult_Attacking;
}