#pragma once

#include "NPCAI.h"
#include "NPC.h"

namespace GameServer
{
	class DefensiveNPCAI final : public NPCAI
	{
	public:
		DefensiveNPCAI(IMatchState* match, IMatchSession* npc) :
			m_Match(match), 
			m_NPC(npc), 
			m_State(&DecidingState),
			m_StoppedTimer(0.0f),
			m_Target(nullptr),
			m_TargetID(0),
			m_Enemy(nullptr),
			m_EnemyID(0),
			m_PasiveFollowingTimer(0.0f),
			m_AggresiveFollowingTimer(0.0f),
			m_EnemyDetectionTimer(0.0f) { }

		void SomeoneDisappear(size_t id) override
		{
			if (m_TargetID == id)
				m_Target = nullptr;

			if (m_EnemyID == id)
				m_Enemy = nullptr;
		}

		void SomeoneAttacks(IMatchSession* attacker) override
		{
			if (DetectsEnemyCondition(this))
				return;

			m_Enemy = attacker;
			m_Match->StartAttack(m_NPC->GetID(), attacker->GetID());
			m_EnemyID = attacker->GetID();
		}

		void AttackCancelled() override
		{
			m_Enemy = nullptr;
		}

		void Update(float deltaTime) override
		{
			DetectTargets(deltaTime);

			m_State(this, deltaTime);
		}

		~DefensiveNPCAI() = default;

	private:
		void DetectTargets(float deltaTime)
		{
			if (DetectsTargetCondition(this))
				return;

			if (FloatComparison::LessEqual(m_EnemyDetectionTimer, 0.0f))
			{
				m_EnemyDetectionTimer = 1.5f;

				Player* npc = m_NPC->GetPlayer();

				std::unordered_map<size_t, IMatchSession*>& sessions = m_Match->GetSessions();
				for (std::pair<const size_t, IMatchSession*>& pair : sessions)
				{
					IMatchSession* session = pair.second;
					Player* player = session->GetPlayer();

					if (npc->Faction != player->Faction && FloatComparison::LessEqual(Vector2::Distance(npc->GetPosition(), player->GetPosition()), npc->AttackRadius))
					{
						if (session->IsPvP())
						{
							m_Target = session;
							m_TargetID = session->GetID();

							return;
						}
					}
				}
			}

			m_EnemyDetectionTimer -= deltaTime;
		}

		static void DecidingState(DefensiveNPCAI* ai, float deltaTime)
		{
			if (DecidesToMoveCondition(ai))
			{
				TransitionToSearchingState(ai);
				return;
			}

			if (DetectsEnemyCondition(ai))
			{
				TransitionToAggresiveFollowingState(ai);
				return;
			}

			if (DetectsTargetCondition(ai))
			{
				TransitionToPasiveFollowingState(ai);
				return;
			}

			ai->m_StoppedTimer -= deltaTime;
		}

		static void SearchingState(DefensiveNPCAI* ai, float deltaTime)
		{
			if (MoveEndedCondition(ai))
			{
				TransitionToDecidingState(ai);
				return;
			}

			if (DetectsEnemyCondition(ai))
			{
				TransitionToAggresiveFollowingState(ai);
				return;
			}

			if (DetectsTargetCondition(ai))
			{
				TransitionToPasiveFollowingState(ai);
				return;
			}
		}

		static void PasiveFollowing(DefensiveNPCAI* ai, float deltaTime)
		{
			if (DetectsEnemyCondition(ai))
			{
				TransitionToAggresiveFollowingState(ai);
				return;
			}

			if (TargetDisappearsCondition(ai))
			{
				TransitionToDecidingState(ai);
				return;
			}

			if (FloatComparison::LessEqual(ai->m_PasiveFollowingTimer, 0.0f))
			{
				TransitionToPasiveFollowingState(ai);
				ai->m_PasiveFollowingTimer = 1.5f;
			}

			if (TargetWithinHalfAttackRangeCondition(ai))
			{
				TransitionToPasiveStoppedState(ai);
				return;
			}

			ai->m_PasiveFollowingTimer -= deltaTime;
		}

		static void PasiveStopped(DefensiveNPCAI* ai, float deltaTime)
		{
			if (DetectsEnemyCondition(ai))
			{
				TransitionToAggresiveStoppedState(ai);
				return;
			}

			if (TargetDisappearsCondition(ai))
			{
				TransitionToDecidingState(ai);
				return;
			}

			if (!TargetWithinAttackRangeCondition(ai))
			{
				TransitionToPasiveFollowingState(ai);
				return;
			}
		}

		static void AggresiveFollowing(DefensiveNPCAI* ai, float deltaTime)
		{
			if (LowHPCondition(ai))
			{
				TransitionToRunningAwayState(ai);
				return;
			}

			if (EnemyDisappearsCondition(ai))
			{
				TransitionToDecidingState(ai);
				return;
			}

			if (FloatComparison::LessEqual(ai->m_AggresiveFollowingTimer, 0.0f))
			{
				TransitionToAggresiveFollowingState(ai);
				ai->m_AggresiveFollowingTimer = 1.5f;
			}

			if (EnemyWithinHalfAttackRangeCondition(ai))
			{
				TransitionToAggresiveStoppedState(ai);
				return;
			}

			ai->m_AggresiveFollowingTimer -= deltaTime;
		}

		static void AggresiveStopped(DefensiveNPCAI* ai, float deltaTime)
		{
			if (LowHPCondition(ai))
			{
				TransitionToRunningAwayState(ai);
				return;
			}

			if (EnemyDisappearsCondition(ai))
			{
				TransitionToDecidingState(ai);
				return;
			}

			if (!EnemyWithinAttackRangeCondition(ai))
			{
				TransitionToAggresiveFollowingState(ai);
				return;
			}
		}

		static void RunningAwayState(DefensiveNPCAI* ai, float deltaTime) { }

		static void TransitionToDecidingState(DefensiveNPCAI* ai)
		{
			ai->m_StoppedTimer = Connection::Random::Range(0.0f, 3.0f);
			ai->m_State = &DecidingState;
		}

		static void TransitionToSearchingState(DefensiveNPCAI* ai)
		{
			Vector2 dst;
			dst.x = Connection::Random::Range(0.0f, 210.0f);
			dst.y = -Connection::Random::Range(0.0f, 135.0f);

			ai->m_NPC->GetPlayer()->MoveTo(dst);
			ai->m_Match->SendMovement(ai->m_NPC->GetID(), dst);

			ai->m_State = &SearchingState;
		}

		static void TransitionToPasiveFollowingState(DefensiveNPCAI* ai)
		{
			float distance = ai->m_NPC->GetPlayer()->AttackRadius / 2.0f;
			float angle = Connection::Random::Range(0.0, 2.0f * Mathf::Pi);

			Vector2 point(std::cos(angle) * distance, -std::sin(angle) * distance);
			Vector2 dst = ai->m_Target->GetPlayer()->GetPosition() + point;

			ai->m_NPC->GetPlayer()->MoveTo(dst);
			ai->m_Match->SendMovement(ai->m_NPC->GetID(), dst);

			ai->m_PasiveFollowingTimer = 0.0f;

			ai->m_State = &PasiveFollowing;
		}

		static void TransitionToPasiveStoppedState(DefensiveNPCAI* ai)
		{
			ai->m_State = &PasiveStopped;
		}

		static void TransitionToAggresiveFollowingState(DefensiveNPCAI* ai)
		{
			ai->m_Target = nullptr;

			float distance = ai->m_NPC->GetPlayer()->AttackRadius / 2.0f;
			float angle = Connection::Random::Range(0.0, 2.0f * Mathf::Pi);

			Vector2 point(std::cos(angle) * distance, -std::sin(angle) * distance);
			Vector2 dst = ai->m_Enemy->GetPlayer()->GetPosition() + point;

			ai->m_NPC->GetPlayer()->MoveTo(dst);
			ai->m_Match->SendMovement(ai->m_NPC->GetID(), dst);

			ai->m_AggresiveFollowingTimer = 0.0f;

			ai->m_State = &AggresiveFollowing;
		}

		static void TransitionToAggresiveStoppedState(DefensiveNPCAI* ai)
		{
			ai->m_Target = nullptr;

			ai->m_State = &AggresiveStopped;
		}

		static void TransitionToRunningAwayState(DefensiveNPCAI* ai)
		{
			Vector2 topLeft(0.0f, 0.0f);
			Vector2 bottomRight(210.0f, -135.0f);

			float topLeftDistance = Vector2::Distance(ai->m_NPC->GetPlayer()->GetPosition(), topLeft);
			float bottomRightDistance = Vector2::Distance(ai->m_NPC->GetPlayer()->GetPosition(), bottomRight);

			Vector2 dst;
			if (FloatComparison::LessEqual(topLeftDistance, bottomRightDistance))
				dst = topLeft;
			else
				dst = bottomRight;

			ai->m_NPC->GetPlayer()->MoveTo(dst);
			ai->m_Match->SendMovement(ai->m_NPC->GetID(), dst);

			ai->m_State = &RunningAwayState;
		}

		static bool TargetDisappearsCondition(DefensiveNPCAI* ai)
		{
			return ai->m_Target == nullptr;
		}

		static bool DetectsTargetCondition(DefensiveNPCAI* ai)
		{
			return ai->m_Target != nullptr;
		}

		static bool TargetWithinHalfAttackRangeCondition(DefensiveNPCAI* ai)
		{
			return FloatComparison::LessEqual(Vector2::Distance(ai->m_NPC->GetPlayer()->GetPosition(), ai->m_Target->GetPlayer()->GetPosition()), ai->m_NPC->GetPlayer()->AttackRadius / 2.0f);
		}

		static bool TargetWithinAttackRangeCondition(DefensiveNPCAI* ai)
		{
			return FloatComparison::LessEqual(Vector2::Distance(ai->m_NPC->GetPlayer()->GetPosition(), ai->m_Target->GetPlayer()->GetPosition()), ai->m_NPC->GetPlayer()->AttackRadius);
		}

		static bool EnemyDisappearsCondition(DefensiveNPCAI* ai)
		{
			return ai->m_Enemy == nullptr;
		}

		static bool DetectsEnemyCondition(DefensiveNPCAI* ai)
		{
			return ai->m_Enemy != nullptr;
		}

		static bool EnemyWithinHalfAttackRangeCondition(DefensiveNPCAI* ai)
		{
			return FloatComparison::LessEqual(Vector2::Distance(ai->m_NPC->GetPlayer()->GetPosition(), ai->m_Enemy->GetPlayer()->GetPosition()), ai->m_NPC->GetPlayer()->AttackRadius / 2.0f);
		}

		static bool EnemyWithinAttackRangeCondition(DefensiveNPCAI* ai)
		{
			return FloatComparison::LessEqual(Vector2::Distance(ai->m_NPC->GetPlayer()->GetPosition(), ai->m_Enemy->GetPlayer()->GetPosition()), ai->m_NPC->GetPlayer()->AttackRadius);
		}

		static bool DecidesToMoveCondition(DefensiveNPCAI* ai)
		{
			return FloatComparison::LessEqual(ai->m_StoppedTimer, 0.0f);
		}

		static bool MoveEndedCondition(DefensiveNPCAI* ai)
		{
			return !ai->m_NPC->GetPlayer()->IsMoving();
		}

		static bool LowHPCondition(DefensiveNPCAI* ai)
		{
			return FloatComparison::Less(static_cast<float>(ai->m_NPC->GetPlayer()->HP), ai->m_NPC->GetPlayer()->MaxHP * 0.2f);
		}

		IMatchState* m_Match;
		IMatchSession* m_NPC;

		void(*m_State)(DefensiveNPCAI*, float);

		float m_StoppedTimer;
		IMatchSession* m_Target;
		size_t m_TargetID;
		IMatchSession* m_Enemy;
		size_t m_EnemyID;
		float m_PasiveFollowingTimer;
		float m_AggresiveFollowingTimer;

		float m_EnemyDetectionTimer;

	};
}