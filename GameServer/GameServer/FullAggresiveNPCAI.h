#pragma once

#include "NPCAI.h"
#include "NPC.h"

namespace GameServer
{
	class FullAggresiveNPCAI final : public NPCAI
	{
	public:
		FullAggresiveNPCAI(IMatchState* match, IMatchSession* npc) : 
			m_Match(match), 
			m_NPC(npc), 
			m_State(&DecidingState),
			m_StoppedTimer(0.0f),
			m_Enemy(nullptr),
			m_EnemyID(0),
			m_AggresiveFollowingTimer(0.0f),
			m_EnemyDetectionTimer(0.0f) { }

		void SomeoneDisappear(size_t id) override
		{
			if (m_EnemyID == id)
				m_Enemy = nullptr;
		}

		void AttackCancelled() override
		{
			m_Enemy = nullptr;
		}

		void Update(float deltaTime) override
		{
			DetectEnemies(deltaTime);

			m_State(this, deltaTime);
		}

		~FullAggresiveNPCAI() = default;

	private:
		void DetectEnemies(float deltaTime)
		{
			if (DetectsEnemyCondition(this))
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
						m_Match->StartAttack(m_NPC->GetID(), session->GetID());
						if (m_Match->IsAttacking(m_NPC->GetID()))
						{
							m_Enemy = session;
							m_EnemyID = session->GetID();

							return;
						}
					}
				}
			}

			m_EnemyDetectionTimer -= deltaTime;
		}

		static void DecidingState(FullAggresiveNPCAI* ai, float deltaTime)
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

			ai->m_StoppedTimer -= deltaTime;
		}

		static void SearchingState(FullAggresiveNPCAI* ai, float deltaTime)
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
		}

		static void AggresiveFollowing(FullAggresiveNPCAI* ai, float deltaTime)
		{
			if (LowHPCondition(ai))
			{
				TransitionToRunningAwayState(ai);
				return;
			}

			if (TargetDisappearsCondition(ai))
			{
				TransitionToDecidingState(ai);
				return;
			}

			if (FloatComparison::LessEqual(ai->m_AggresiveFollowingTimer, 0.0f))
			{
				TransitionToAggresiveFollowingState(ai);
				ai->m_AggresiveFollowingTimer = 1.5f;
			}

			if (TargetWithinHalfAttackRangeCondition(ai))
			{
				TransitionToAggresiveStoppedState(ai);
				return;
			}

			ai->m_AggresiveFollowingTimer -= deltaTime;
		}

		static void AggresiveStopped(FullAggresiveNPCAI* ai, float deltaTime)
		{
			if (LowHPCondition(ai))
			{
				TransitionToRunningAwayState(ai);
				return;
			}

			if (TargetDisappearsCondition(ai))
			{
				TransitionToDecidingState(ai);
				return;
			}

			if (!TargetWithinAttackRangeCondition(ai))
			{
				TransitionToAggresiveFollowingState(ai);
				return;
			}
		}

		static void RunningAwayState(FullAggresiveNPCAI* ai, float deltaTime) { }

		static void TransitionToDecidingState(FullAggresiveNPCAI* ai)
		{
			ai->m_StoppedTimer = Connection::Random::Range(0.0f, 3.0f);
			ai->m_State = &DecidingState;
		}

		static void TransitionToSearchingState(FullAggresiveNPCAI* ai)
		{
			Vector2 dst;
			dst.x = Connection::Random::Range(0.0f, 210.0f);
			dst.y = -Connection::Random::Range(0.0f, 135.0f);

			ai->m_NPC->GetPlayer()->MoveTo(dst);
			ai->m_Match->SendMovement(ai->m_NPC->GetID(), dst);

			ai->m_State = &SearchingState;
		}

		static void TransitionToAggresiveFollowingState(FullAggresiveNPCAI* ai)
		{
			float distance = ai->m_NPC->GetPlayer()->AttackRadius / 2.0f;
			float angle = Connection::Random::Range(0.0, 2.0f * Mathf::Pi);

			Vector2 point(std::cos(angle) * distance, -std::sin(angle) * distance);
			Vector2 dst = ai->m_Enemy->GetPlayer()->GetPosition() + point;

			ai->m_NPC->GetPlayer()->MoveTo(dst);
			ai->m_Match->SendMovement(ai->m_NPC->GetID(), dst);

			ai->m_AggresiveFollowingTimer = 0.0f;

			ai->m_State = &AggresiveFollowing;
		}

		static void TransitionToAggresiveStoppedState(FullAggresiveNPCAI* ai)
		{
			ai->m_State = &AggresiveStopped;
		}

		static void TransitionToRunningAwayState(FullAggresiveNPCAI* ai)
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

		static bool TargetDisappearsCondition(FullAggresiveNPCAI* ai)
		{
			return ai->m_Enemy == nullptr;
		}

		static bool DetectsEnemyCondition(FullAggresiveNPCAI* ai)
		{
			return ai->m_Enemy != nullptr;
		}

		static bool TargetWithinHalfAttackRangeCondition(FullAggresiveNPCAI* ai)
		{
			return FloatComparison::LessEqual(Vector2::Distance(ai->m_NPC->GetPlayer()->GetPosition(), ai->m_Enemy->GetPlayer()->GetPosition()), ai->m_NPC->GetPlayer()->AttackRadius / 2.0f);
		}

		static bool TargetWithinAttackRangeCondition(FullAggresiveNPCAI* ai)
		{
			return FloatComparison::LessEqual(Vector2::Distance(ai->m_NPC->GetPlayer()->GetPosition(), ai->m_Enemy->GetPlayer()->GetPosition()), ai->m_NPC->GetPlayer()->AttackRadius);
		}

		static bool DecidesToMoveCondition(FullAggresiveNPCAI* ai)
		{
			return FloatComparison::LessEqual(ai->m_StoppedTimer, 0.0f);
		}

		static bool MoveEndedCondition(FullAggresiveNPCAI* ai)
		{
			return !ai->m_NPC->GetPlayer()->IsMoving();
		}

		static bool LowHPCondition(FullAggresiveNPCAI* ai)
		{
			return FloatComparison::Less(static_cast<float>(ai->m_NPC->GetPlayer()->HP), ai->m_NPC->GetPlayer()->MaxHP * 0.2f);
		}

		IMatchState* m_Match;
		IMatchSession* m_NPC;

		void(*m_State)(FullAggresiveNPCAI*, float);

		float m_StoppedTimer;
		IMatchSession* m_Enemy;
		size_t m_EnemyID;
		float m_AggresiveFollowingTimer;

		float m_EnemyDetectionTimer;

	};
}