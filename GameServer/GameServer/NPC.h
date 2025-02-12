#pragma once

#include "Player.h"
#include "IMatchState.h"
#include "IMatchSession.h"
#include "INPCDespawner.h"
#include "Mathf.h"
#include "NPCAI.h"
#include "NPCAIFactory.h"
#include "Aura.h"
#include "AuraFactory.h"
#include "InternalAura.h"

#include "ShieldRegeneratorAura.h"
#include "AttackerAura.h"

#include <Random.h>

namespace GameServer
{
	class NPC final : public IMatchSession
	{
	public:
		NPC(
			INPCDespawner& npcDespawner, 
			const size_t id, 
			class IMatchState* const matchState, 
			Player* const player, 
			NPCAIFactory* npcAIFactory,
			std::unordered_map<size_t, AuraFactory*>& auraFactories) :
			m_NpcDespawner(npcDespawner),
			m_ID(id),
			m_MatchState(matchState),
			m_PlayerInfo(player),
			m_AI(npcAIFactory->Create(matchState, this)),
			m_AuraFactories(auraFactories)
		{
			AddInternalAura(new ShieldRegeneratorAura(m_MatchState, this));
			AddInternalAura(new AttackerAura(m_MatchState, this));
		}

		~NPC()
		{
			delete m_AI;

			for (auto& aura : m_Auras)
				delete aura.second;

			for (auto& aura : m_InternalAuras)
				delete aura.second;

			delete m_PlayerInfo;
		}

		void SendNewPlayer(size_t id, Player* player) const override { }
		void SendMovement(size_t id, Vector2& destination) override { }
		void SendOutOfRange() override { }
		void SendStatusUpdate(size_t id, std::uint32_t hp, std::uint32_t maxHP, std::uint32_t shield, std::uint32_t maxShield) override { }
		void SendStopAttack(size_t id) override { }
		void SendAttackResponse(bool value) override { }
		void SendJumpResult(bool result) override { }
		void SendCollecting(size_t id) const override { }
		void SendCollectionResult(bool result) const override { }
		void SendCollectableSpawn(class Collectable* collectable) const override { }
		void SendCollectableDespawn(size_t id) const override { }
		void SendDisconnectionCancelled() const override { }

		void SendStartAttack(size_t attackerID, size_t attackedID) override
		{
			if (m_ID == attackedID)
				m_AI->SomeoneAttacks(m_MatchState->GetSessions().at(attackerID));
		}

		void SendDisconnection(size_t id) override
		{
			m_AI->SomeoneDisappear(id);
		}

		void SendSomeoneJumps(size_t id) override
		{
			m_AI->SomeoneDisappear(id);
		}

		void SendSomeoneDies(size_t id) override
		{
			m_AI->SomeoneDisappear(id);
		}

		void SendCancelAttack() override
		{
			m_AI->AttackCancelled();
		}

		void SendDamageDone(size_t id, std::uint32_t damage) override
		{
			for (auto& aura : m_Auras)
			{
				aura.second->OnCombat();
				aura.second->OnDamageDone();
			}

			for (auto& aura : m_InternalAuras)
			{
				aura.second->OnCombat();
				aura.second->OnDamageDone();
			}
		}

		void SendDamageReceived(std::uint32_t damage) override
		{
			for (auto& aura : m_Auras)
			{
				aura.second->OnCombat();
				aura.second->OnDamageReceived();
			}

			for (auto& aura : m_InternalAuras)
			{
				aura.second->OnCombat();
				aura.second->OnDamageReceived();
			}
		}

		void SendYouDie() override
		{
			m_NpcDespawner.Despawn();
			m_MatchState->SendSomeoneDies(m_ID);
		}

		bool Update(std::uint32_t timeDifference) override
		{
			const float deltaTime = timeDifference / 1000.0f;

			m_AI->Update(deltaTime);

			for (auto& aura : m_Auras)
				aura.second->Update(deltaTime);

			for (auto& aura : m_InternalAuras)
				aura.second->Update(deltaTime);

			return true;
		}

		Player* const GetPlayer() const override { return m_PlayerInfo; }
		const size_t GetID() const override { return m_ID; }
		const size_t GetAccountID() const override { return 0; }

		bool IsPvP() const override
		{
			for (auto& aura : m_InternalAuras)
			{
				if (!aura.second->IsPvP())
					return false;
			}

			return true;
		}

		bool IsAttacking() const override
		{
			for (auto& aura : m_InternalAuras)
			{
				if (aura.second->IsAttacking())
					return true;
			}

			return false;
		}

		void AddAura(size_t id) override
		{
			Aura* aura = m_AuraFactories[id]->Create(m_MatchState, this);
			m_Auras.insert(std::make_pair(id, aura));
		}

		void RemoveAura(size_t id) override
		{
			Aura* aura = m_Auras[id];

			m_Auras.erase(id);
			delete aura;
		}

		void AddInternalAura(InternalAura* aura) override
		{
			m_InternalAuras.insert(std::make_pair(aura->ID, aura));
		}

		void RemoveInternalAura(size_t id) override
		{
			Aura* aura = m_InternalAuras[id];
			m_InternalAuras.erase(id);
			delete aura;
		}

		bool ShouldDisconnect(time_t time) const override
		{
			return false;
		}

		bool IsDisconnecting() override
		{
			return false;
		}

		void CancelDisconnection() override { }

	private:
		INPCDespawner& m_NpcDespawner;
		class IMatchState* const m_MatchState;
		Player* const m_PlayerInfo;
		const size_t m_ID;
		NPCAI* m_AI;

		std::unordered_map<size_t, AuraFactory*>& m_AuraFactories;

		std::unordered_map<size_t, Aura*> m_Auras;
		std::unordered_map<size_t, InternalAura*> m_InternalAuras;

	};
}