#pragma once

#include "InternalAura.h"
#include "FloatComparison.h"
#include "IMatchState.h"
#include "IMatchSession.h"

namespace GameServer
{
	class AttackerAura final : public InternalAura
	{
	public:
		AttackerAura(IMatchState* match, IMatchSession* session) :
			InternalAura(match, session, AuraID),
			m_AttackTimer(0.0f) { }

		~AttackerAura() = default;

		InternalAura* InternalCopy(IMatchState* match, IMatchSession* session) override
		{
			AttackerAura* aura = new AttackerAura(match, session);
			aura->m_AttackTimer = m_AttackTimer;

			return aura;
		}

		void OnDamageDone() override
		{
			m_AttackTimer = k_CombatTime;
		}

		bool IsAttacking() override
		{
			return FloatComparison::Greater(m_AttackTimer, 0.0f);
		}

		void Update(float deltaTime) override
		{
			m_AttackTimer -= deltaTime;
		}

		static constexpr size_t AuraID = 2;

	private:
		static constexpr float k_CombatTime = 15.0f;

		float m_AttackTimer;

	};
}