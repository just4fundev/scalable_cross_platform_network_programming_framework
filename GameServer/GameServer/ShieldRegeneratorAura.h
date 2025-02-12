#pragma once

#include "InternalAura.h"
#include "FloatComparison.h"
#include "IMatchState.h"
#include "IMatchSession.h"
#include "Player.h"

namespace GameServer
{
	class ShieldRegeneratorAura final : public InternalAura
	{
	public:
		ShieldRegeneratorAura(IMatchState* match, IMatchSession* session) : 
			InternalAura(match, session, AuraID),
			m_CombatTimer(k_CombatTime),
			m_ShieldRegenerationTimer(k_ShieldRegenerationTimer) { }

		~ShieldRegeneratorAura() = default;

		InternalAura* InternalCopy(IMatchState* match, IMatchSession* session) override
		{
			ShieldRegeneratorAura* aura = new ShieldRegeneratorAura(match, session);
			aura->m_CombatTimer = m_CombatTimer;
			aura->m_ShieldRegenerationTimer = m_ShieldRegenerationTimer;

			return aura;
		}

		void OnDamageReceived() override
		{
			m_CombatTimer = k_CombatTime;
		}

		void Update(float deltaTime) override
		{
			Player* player = m_Session->GetPlayer();

			if (!FloatComparison::LessEqual(m_CombatTimer, 0.0f))
			{
				m_CombatTimer -= deltaTime;

				return;
			}

			if (player->Shield >= player->MaxShield)
				return;

			if (!FloatComparison::LessEqual(m_ShieldRegenerationTimer, 0.0f))
			{
				m_ShieldRegenerationTimer -= deltaTime;

				return;
			}

			std::uint32_t shield = player->Shield + static_cast<std::uint32_t>(player->MaxShield * 0.04f);
			player->Shield = shield < player->MaxShield ? shield : player->MaxShield;

			m_Match->SendStatusUpdate(m_Session->GetID());

			m_ShieldRegenerationTimer = k_ShieldRegenerationTimer;
		}

		static constexpr size_t AuraID = 0;

	private:
		static constexpr float k_CombatTime = 15.0f;
		static constexpr float k_ShieldRegenerationTimer = 1.0f;

		float m_CombatTimer;
		float m_ShieldRegenerationTimer;

	};
}