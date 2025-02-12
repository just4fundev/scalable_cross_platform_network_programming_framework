#pragma once

#include <unordered_map>

#include <Database.h>
#include <DatabaseProcessor.h>

#include "IMatchState.h"
#include "MatchSession.h"
#include "Attack.h"
#include "Jump.h"
#include "IMapManager.h"
#include "Portal.h"
#include "NPCSpawner.h"
#include "NPC.h"
#include "Collectable.h"
#include "Collection.h"
#include "CollectableSpawner.h"
#include "AccountManager.h"
#include "AccountsDatabase.h"
#include "WorldDatabase.h"
#include "RectArea.h"
#include "CircleArea.h"
#include "NPCAIFactory.h"
#include "ScriptableAreaFactory.h"
#include "AuraFactory.h"
#include "Aura.h"
#include "ScriptableArea.h"
#include "GameTime.h"

namespace GameServer
{
	class Match final : public IMatchState
	{
	public:
		Match(
			AccountManager& accountManager, 
			Database::Database& accountsDB, 
			Database::Database& worldDB, 
			IMapManager* const mapManager, 
			const size_t mapID, 
			std::unordered_map<std::string, NPCAIFactory*>& npcAIFactories,
			std::unordered_map<std::string, ScriptableAreaFactory*>& scriptableAreaFactories,
			std::unordered_map<size_t, AuraFactory*>& auraFactories) :
			m_MapManager(mapManager), 
			m_NextID(0), 
			m_MapID(mapID), 
			m_AccountsDB(accountsDB), 
			m_WorldDB(worldDB), 
			m_AccountManager(accountManager), 
			m_NPCAIFactories(npcAIFactories),
			m_ScriptableAreaFactories(scriptableAreaFactories),
			m_AuraFactories(auraFactories) { }

		~Match()
		{
			m_DatabaseProcessor.ProcessEverithing();

			for (std::pair<Vector2, MatchSession*>& transfer : m_Transfers)
				delete transfer.second;

			for (NPCSpawner* npcSpawner : m_NPCSpawners)
				delete npcSpawner;

			for (CollectableSpawner* collectableSpawner : m_CollectableSpawners)
				delete collectableSpawner;

			for (std::pair<const size_t, Collectable*>& collectable : m_Collectables)
				delete collectable.second;

			for (ScriptableArea* area : m_ScriptableAreas)
				delete area;

			for (std::pair<const size_t, IMatchSession*>& session : m_Sessions)
			{
				session.second->Close();
				delete session.second;
			}
		}

		void Init()
		{
			LoadPortals();
			LoadNPCSpawners();
			LoadCollectableSpawners();
			LoadAreas();
		}

		void AddSession(size_t accountID, Session* session, Player* player)
		{
			size_t id = CalculateNextID();
			IMatchSession* mapSession = new MatchSession(accountID, session, id, this, player, m_AuraFactories);
			
			SendWorldState(mapSession);
			SendNewPlayer(id, mapSession->GetPlayer());

			m_Sessions.insert(std::make_pair(id, mapSession));
		}

		void Transfer(MatchSession* oldMatchSession, const Vector2 position)
		{
			m_Transfers.push_back(std::make_pair(position, oldMatchSession));
		}

		void Update(std::uint32_t timeDifference)
		{
			UpdateTransfers();

			UpdateNPCSpawner(timeDifference);
			UpdateCollectableSpawner(timeDifference);
			UpdateAreas(timeDifference);
			UpdatePlayers(timeDifference);
			UpdateJumps(timeDifference);
			UpdateAttacks(timeDifference);
			UpdateCollections(timeDifference);
			UpdateSessions(timeDifference);

			m_DatabaseProcessor.TryProccess();
		}

		void SendMovement(size_t id, Vector2& destination) const override
		{
			for (const std::pair<const size_t, IMatchSession*>& session : m_Sessions)
			{
				if (id != session.first)
					session.second->SendMovement(id, destination);
			}
		}

		void StartAttack(size_t attackerID, size_t attackedID) override
		{
			if (m_Attacks.count(attackerID) > 0)
				return;

			if (m_Sessions.count(attackedID) == 0)
				return;

			IMatchSession& attacker = *m_Sessions.at(attackerID);
			IMatchSession& attacked = *m_Sessions.at(attackedID);

			if (!attacked.IsPvP())
			{
				attacker.SendAttackResponse(false);
				return;
			}

			m_Attacks.insert(std::make_pair(attackerID, Attack(*this, attacker, attacked, attacker.GetPlayer()->AttackRadius, attacker.GetPlayer()->AttackTime)));
			attacker.SendAttackResponse(true);

			for (std::pair<const size_t, IMatchSession*>& session : m_Sessions)
			{
				if (attackerID != session.first)
					session.second->SendStartAttack(attackerID, attackedID);
			}
		}

		void StopAttack(size_t attackerID) override
		{
			m_Attacks.erase(attackerID);

			for (std::pair<const size_t, IMatchSession*>& session : m_Sessions)
				session.second->SendStopAttack(attackerID);
		}

		void SendSomeoneDies(size_t id) const override
		{
			for (const std::pair<const size_t, IMatchSession*>& session : m_Sessions)
			{
				if (id != session.first)
					session.second->SendSomeoneDies(id);
			}
		}

		void SendStatusUpdate(size_t id) const override
		{
			IMatchSession& updated = *m_Sessions.at(id);

			for (const std::pair<const size_t, IMatchSession*>& session : m_Sessions)
				session.second->SendStatusUpdate(id, updated.GetPlayer()->HP, updated.GetPlayer()->MaxHP, updated.GetPlayer()->Shield, updated.GetPlayer()->MaxShield);
		}

		void ActiveJump(MatchSession* jumper) override
		{
			size_t jumperID = jumper->GetID();

			if (m_Jumps.count(jumperID) > 0)
				return;

			for (Portal& portal : m_Portals)
			{
				if (FloatComparison::LessEqual(Vector2::Distance(jumper->GetPlayer()->GetPosition(), portal.Position), Portal::Radius))
				{
					m_Jumps.insert(std::make_pair(jumperID, Jump(*this, jumper, portal.DestinationMap, portal.DestinationPosition)));
					jumper->SendJumpResult(true);

					return;
				}
			}

			jumper->SendJumpResult(false);
		}

		void SendSomeoneJumps(size_t jumperID) const override
		{
			for (const std::pair<const size_t, IMatchSession*>& session : m_Sessions)
			{
				if (jumperID != session.first)
					session.second->SendSomeoneJumps(jumperID);
			}
		}

		void Collect(MatchSession* collector, size_t collectableId) override
		{
			size_t collectorID = collector->GetID();

			if (m_Collections.count(collectorID) > 0)
				return;

			if (m_Collectables.count(collectableId) == 0)
				return;

			Collectable* collectable = m_Collectables.at(collectableId);
			m_Collections.insert(std::make_pair(collectorID, Collection(*this, collector, collectable, collector->GetPlayer()->GetDestination())));
		}

		void SendCollectableSpawn(Collectable* collectable) const override
		{
			for (const std::pair<const size_t, IMatchSession*>& session : m_Sessions)
				session.second->SendCollectableSpawn(collectable);
		}

		void SendCollectableDespawn(size_t collectableID) const override
		{
			for (const std::pair<const size_t, IMatchSession*>& session : m_Sessions)
				session.second->SendCollectableDespawn(collectableID);
		}

		std::unordered_map<size_t, IMatchSession*>& GetSessions() override
		{
			return m_Sessions;
		}

		bool IsAttacking(size_t id) override
		{
			return m_Attacks.count(id) != 0;
		}

	private:
		void UpdateSessions(std::uint32_t timeDifference)
		{
			std::unordered_map<size_t, IMatchSession*>::iterator itr = m_Sessions.begin();
			while (itr != m_Sessions.end())
			{
				IMatchSession* session = itr->second;

				session->Update(timeDifference);
				if (session->IsDisconnecting())
				{
					if (!IsValidDisconnection(session))
					{
						session->CancelDisconnection();
						session->SendDisconnectionCancelled();
					}
				}

				if (session->IsConnectionIdle())
				{
					SavePlayer(session->GetAccountID(), session->GetPlayer());

					size_t accountID = itr->second->GetAccountID();
					size_t id = itr->first;

					session->Close();
					delete session;
					itr = m_Sessions.erase(itr);

					m_AccountManager.SignOut(accountID);
					SendDisconnection(id);
				}
				else if (session->ShouldDisconnect(GameTime::GetClockSeconds()))
				{
					SavePlayer(session->GetAccountID(), session->GetPlayer());

					size_t accountID = itr->second->GetAccountID();
					size_t id = itr->first;

					session->Close();
					delete session;
					itr = m_Sessions.erase(itr);

					m_AccountManager.SignOut(accountID);
					SendDisconnection(id);
				}
				else
				{
					++itr;
				}
			}
		}

		void UpdatePlayers(std::uint32_t timeDifference)
		{
			for (std::pair<const size_t, IMatchSession*>& session : m_Sessions)
				session.second->GetPlayer()->Update(timeDifference);
		}

		void UpdateAttacks(std::uint32_t timeDifference)
		{
			std::unordered_map<size_t, Attack>::iterator iterator = m_Attacks.begin();
			while (iterator != m_Attacks.end())
			{
				Attack& attack = iterator->second;
				
				if (m_Sessions.count(attack.AttackerID) == 0)
				{
					iterator = m_Attacks.erase(iterator);
				}
				else if (m_Sessions.count(attack.AttackedID) == 0)
				{
					iterator = m_Attacks.erase(iterator);
				}
				else
				{
					AttackResult result = iterator->second.Update(timeDifference);

					if (result == AttackResult_AttackedDead)
					{
						IMatchSession* attacked = m_Sessions.at(attack.AttackedID);

						if (m_Attacks.count(attacked->GetID()) > 0)
							m_Attacks.erase(attacked->GetID());

						iterator = m_Attacks.erase(iterator);

						m_Sessions.erase(attacked->GetID());
						delete attacked;
					}
					else if (result == AttackResult_Cancelled)
					{
						size_t attackerID = attack.AttackerID;

						iterator = m_Attacks.erase(iterator);

						m_Sessions.at(attackerID)->SendCancelAttack();

						for (std::pair<const size_t, IMatchSession*>& session : m_Sessions)
						{
							if (session.first != attackerID)
								session.second->SendStopAttack(attackerID);
						}
					}
					else
					{
						iterator++;
					}
				}
			}
		}

		void UpdateJumps(std::uint32_t timeDifference)
		{
			std::unordered_map<size_t, Jump>::iterator iterator = m_Jumps.begin();
			while (iterator != m_Jumps.end())
			{
				Jump& jump = iterator->second;

				if (m_Sessions.count(jump.JumperID) == 0)
				{
					iterator = m_Jumps.erase(iterator);
				}
				else if (iterator->second.Update(timeDifference))
				{
					MatchSession* jumper = jump.Jumper;

					const size_t destinationMap = jump.DestinationMap;
					const Vector2 destinationPositon = jump.DestinationPosition;

					iterator = m_Jumps.erase(iterator);
					m_Sessions.erase(jumper->GetID());

					m_MapManager->Transfer(destinationMap, destinationPositon, jumper);
				}
				else
				{
					iterator++;
				}
			}
		}

		void UpdateNPCSpawner(std::uint32_t timeDifference)
		{
			for (NPCSpawner* npcSpawner : m_NPCSpawners)
			{
				size_t count = npcSpawner->Update(timeDifference);
				
				for (size_t i = 0; i < count; i++)
				{
					size_t id = CalculateNextID();

					Player* player = npcSpawner->Spawn(m_MapID);
					std::string ai = npcSpawner->GetAI();
					NPCAIFactory* npcAIFactory = m_NPCAIFactories.at(ai);
					IMatchSession* npc = new NPC(*npcSpawner, id, this, player, npcAIFactory, m_AuraFactories);

					SendNewPlayer(id, npc->GetPlayer());

					m_Sessions.insert(std::make_pair(id, npc));
				}
			}
		}

		void UpdateCollectableSpawner(std::uint32_t timeDifference)
		{
			for (CollectableSpawner* collectableSpawner : m_CollectableSpawners)
			{
				size_t count = collectableSpawner->Update(timeDifference);

				for (size_t i = 0; i < count; i++)
				{
					size_t id = CalculateNextID();

					Collectable* collectable = collectableSpawner->Spawn(id);
					m_Collectables.insert(std::make_pair(id, collectable));

					SendCollectableSpawn(collectable);
				}
			}
		}

		void UpdateCollections(std::uint32_t timeDifference)
		{
			std::unordered_map<size_t, Collection>::iterator iterator = m_Collections.begin();
			while (iterator != m_Collections.end())
			{
				Collection& collection = iterator->second;

				if (m_Sessions.count(collection.CollectorID) == 0)
				{
					iterator = m_Collections.erase(iterator);
				}
				else if (m_Collectables.count(collection.CollectableID) == 0)
				{
					collection.Collector->SendCollectionResult(false);
					iterator = m_Collections.erase(iterator);
				}
				else
				{
					CollectionResult result = iterator->second.Update(timeDifference);

					if (result == CollectionResult_Collected)
					{
						collection.Collector->SendCollectionResult(true);

						Collectable* collectable = collection.CollectableInfo;
						SendCollectableDespawn(collectable->ID);

						iterator = m_Collections.erase(iterator);

						m_Collectables.erase(collectable->ID);
						collectable->Despawn();
						delete collectable;
					}
					else if (result == CollectionResult_CancelledBeforeCollecting)
					{
						iterator = m_Collections.erase(iterator);
					}
					else if (result == CollectionResult_CancelledWhileCollecting)
					{
						collection.Collector->SendCollectionResult(false);
						iterator = m_Collections.erase(iterator);
					}
					else
					{
						iterator++;
					}
				}
			}
		}

		void UpdateAreas(std::uint32_t timeDifference)
		{
			for (ScriptableArea* area : m_ScriptableAreas)
				area->Update();
		}

		void UpdateTransfers()
		{
			for (std::pair<Vector2, MatchSession*>& oldMapSession : m_Transfers)
			{
				size_t id = CalculateNextID();

				Player* player = new Player(m_MapID, oldMapSession.first, *oldMapSession.second->GetPlayer());
				MatchSession* newMapSession = new MatchSession(id, this, *oldMapSession.second, player);

				SendWorldState(newMapSession);
				SendNewPlayer(id, newMapSession->GetPlayer());

				m_Sessions.insert(std::make_pair(id, newMapSession));

				delete oldMapSession.second;
			}

			m_Transfers.clear();
		}

		void SendDisconnection(size_t id)
		{
			for (std::pair<const size_t, IMatchSession*>& session : m_Sessions)
				session.second->SendDisconnection(id);
		}

		void SendWorldState(IMatchSession* session)
		{
			std::uint16_t size = static_cast<std::uint16_t>(sizeof(Connection::Opcode) + session->GetPlayer()->GetSize() + sizeof(size_t) * 6 + sizeof(float) * 2);
			
			for (auto s : m_Sessions)
				size += static_cast<std::uint16_t>(sizeof(size_t) + s.second->GetPlayer()->GetSize());

			for (auto& attack : m_Attacks)
				size += static_cast<std::uint16_t>(sizeof(size_t) * 2);
			
			for (auto& portals : m_Portals)
				size += static_cast<std::uint16_t>(sizeof(float) * 2 + sizeof(size_t));

			for (auto& collectables : m_Collectables)
				size += static_cast<std::uint16_t>(collectables.second->GetSize());

			BaseServer::OutgoingMessage outgoingMessage(size);
			
			outgoingMessage.GetStreamWriter() << static_cast<Connection::Opcode>(GameServerPublic::ServerOpcode::S_WorldState);
			outgoingMessage.GetStreamWriter() << static_cast<size_t>(session->GetID());
			session->GetPlayer()->Serialize(outgoingMessage.GetStreamWriter());

			outgoingMessage.GetStreamWriter() 
				<< static_cast<size_t>(m_MapID) 
				<< static_cast<float>(210.0f) 
				<< static_cast<float>(135.0f) 
				<< static_cast<size_t>(m_Sessions.size());

			for (std::pair<const size_t, IMatchSession*>& s : m_Sessions)
			{
				outgoingMessage.GetStreamWriter() << static_cast<size_t>(s.first);
				s.second->GetPlayer()->Serialize(outgoingMessage.GetStreamWriter());
			}

			outgoingMessage.GetStreamWriter() << static_cast<size_t>(m_Attacks.size());
			for (auto& attack : m_Attacks)
				outgoingMessage.GetStreamWriter() << static_cast<size_t>(attack.second.AttackerID) << static_cast<size_t>(attack.second.AttackedID);

			outgoingMessage.GetStreamWriter() << static_cast<size_t>(m_Portals.size());
			for (auto& portal : m_Portals)
				outgoingMessage.GetStreamWriter() << static_cast<float>(portal.Position.x) << static_cast<float>(portal.Position.y) << static_cast<size_t>(0);

			outgoingMessage.GetStreamWriter() << static_cast<size_t>(m_Collectables.size());
			for (auto& collectables : m_Collectables)
				collectables.second->Serialize(outgoingMessage.GetStreamWriter());

			session->Send(std::move(outgoingMessage));
		}

		void SendNewPlayer(size_t id, Player* player)
		{
			for (std::pair<const size_t, IMatchSession*>& session : m_Sessions)
				session.second->SendNewPlayer(id, player);
		}

		size_t CalculateNextID()
		{
			size_t id = m_NextID;
			m_NextID++;
			return id;
		}

		void SavePlayer(size_t accountID, Player* player)
		{
			Vector2 position = player->GetPosition();

			std::vector<boost::mysql::field> params = {
				boost::mysql::field(player->MapID),
				boost::mysql::field(position.x),
				boost::mysql::field(position.y),
				boost::mysql::field(player->HP),
				boost::mysql::field(player->Shield),
				boost::mysql::field(accountID)
			};

			m_DatabaseProcessor.AddCallback(m_AccountsDB.AsynchronousExecuteStatement(AccountsADBStmts::ADB_UpdatePlayer, params, EmptyProcessor));
		}

		void LoadPortals()
		{
			std::vector<boost::mysql::field> params = {
				boost::mysql::field(m_MapID)
			};

			boost::mysql::results result;
			m_WorldDB.SynchronousExecuteStatement(WorldSDBStmts::SDB_SelectMapPortals, params, result);

			if (!result.has_value())
				return;

			boost::mysql::rows_view rows = result.rows();
			
			for (size_t i = 0; i < rows.size(); i++)
			{
				boost::mysql::row_view row = rows.at(i);

				float srcPositionX = row.at(0).as_float();
				float srcPositionY = row.at(1).as_float();
				size_t dstMapID = row.at(2).as_uint64();
				float dstPositionX = row.at(3).as_float();
				float dstPositionY = row.at(4).as_float();

				m_Portals.push_back(Portal(Vector2(srcPositionX, srcPositionY), dstMapID, Vector2(dstPositionX, dstPositionY)));
			}
		}

		void LoadNPCSpawners()
		{
			std::vector<boost::mysql::field> params = {
				boost::mysql::field(m_MapID)
			};

			boost::mysql::results result;
			m_WorldDB.SynchronousExecuteStatement(WorldSDBStmts::SDB_SelectMapNPC, params, result);

			if (!result.has_value())
				return;

			boost::mysql::rows_view rows = result.rows();

			for (size_t i = 0; i < rows.size(); i++)
			{
				boost::mysql::row_view row = rows.at(i);

				const size_t count = row.at(0).as_uint64();
				const std::uint32_t milliseconds = static_cast<std::uint32_t>(row.at(1).as_uint64());
				const size_t id = row.at(2).as_uint64();
				const std::uint32_t maxHP = static_cast<std::uint32_t>(row.at(3).as_uint64());
				const std::uint32_t maxShield = static_cast<std::uint32_t>(row.at(4).as_uint64());
				const std::uint32_t hp = static_cast<std::uint32_t>(row.at(5).as_uint64());
				const std::uint32_t shield = static_cast<std::uint32_t>(row.at(6).as_uint64());
				const float speed = row.at(7).as_float();
				const std::uint32_t minDamage = static_cast<std::uint32_t>(row.at(8).as_uint64());
				const std::uint32_t maxDamage = static_cast<std::uint32_t>(row.at(9).as_uint64());
				const float attackRadius = row.at(10).as_float();
				const std::string ai = std::string(row.at(11).as_string());
				const std::uint8_t faction = static_cast<std::uint8_t>(row.at(12).as_uint64());
				const float attackTime = row.at(13).as_float();

				m_NPCSpawners.push_back(new NPCSpawner(count, milliseconds, NPCSpawnerInfo(id, maxHP, maxShield, hp, shield, speed, minDamage, maxDamage, attackRadius, ai, faction, attackTime)));
			}
		}

		void LoadCollectableSpawners()
		{
			std::vector<boost::mysql::field> params = {
				boost::mysql::field(m_MapID)
			};

			boost::mysql::results result;
			m_WorldDB.SynchronousExecuteStatement(WorldSDBStmts::SDB_SelectMapCollectables, params, result);

			if (!result.has_value())
				return;

			boost::mysql::rows_view rows = result.rows();

			for (size_t i = 0; i < rows.size(); i++)
			{
				boost::mysql::row_view row = rows.at(i);

				const size_t count = row.at(0).as_uint64();
				const std::uint32_t milliseconds = static_cast<std::uint32_t>(row.at(1).as_uint64());
				const float argument1 = row.at(2).as_float();
				const float argument2 = row.at(3).as_float();

				IArea* area;
				if (row.at(4).is_null())
				{
					const float width = argument1;
					const float height = argument2;

					area = new RectArea(Vector2(width / 2.0f, height / 2.0f), width, height);
				}
				else
				{
					const float positionX = argument1;
					const float positionY = argument2;

					const float radius = row.at(4).as_float();

					area = new CircleArea(Vector2(positionX, positionY), radius);
				}

				const size_t id = row.at(5).as_uint64();
				const float seconds = row.at(6).as_float();

				m_CollectableSpawners.push_back(new CollectableSpawner(area, count, milliseconds, CollectableSpawnerInfo(id, seconds)));
			}
		}

		void LoadAreas()
		{
			std::vector<boost::mysql::field> params = {
				boost::mysql::field(m_MapID)
			};

			boost::mysql::results result;
			m_WorldDB.SynchronousExecuteStatement(WorldSDBStmts::SDB_SelectMapAreas, params, result);

			if (!result.has_value())
				return;

			boost::mysql::rows_view rows = result.rows();

			for (size_t i = 0; i < rows.size(); i++)
			{
				boost::mysql::row_view row = rows.at(i);

				const float positionX = row.at(0).as_float();
				const float positionY = row.at(1).as_float();
				const float radius = row.at(2).as_float();
				const std::string script = std::string(row.at(3).as_string());

				m_ScriptableAreas.push_back(m_ScriptableAreaFactories[script]->Create(this, Vector2(positionX, positionY), radius));
			}
		}

		bool IsValidDisconnection(IMatchSession* session)
		{
			return true;
		}

		static void EmptyProcessor(boost::mysql::results& result) { }

		std::unordered_map<size_t, IMatchSession*> m_Sessions;

		std::unordered_map<size_t, Collectable*> m_Collectables;

		std::unordered_map<size_t, Attack> m_Attacks;
		std::unordered_map<size_t, Jump> m_Jumps;
		std::unordered_map<size_t, Collection> m_Collections;

		std::vector<Portal> m_Portals;
		std::vector<NPCSpawner*> m_NPCSpawners;
		std::vector<CollectableSpawner*> m_CollectableSpawners;
		std::vector<ScriptableArea*> m_ScriptableAreas;

		std::vector<std::pair<Vector2, MatchSession*>> m_Transfers;
		const size_t m_MapID;

		size_t m_NextID;

		IMapManager* const m_MapManager;
		Database::Database& m_AccountsDB;
		Database::Database& m_WorldDB;
		Database::DatabaseProcessor m_DatabaseProcessor;
		AccountManager& m_AccountManager;

		std::unordered_map<std::string, NPCAIFactory*>& m_NPCAIFactories;
		std::unordered_map<std::string, ScriptableAreaFactory*>& m_ScriptableAreaFactories;
		std::unordered_map<size_t, AuraFactory*>& m_AuraFactories;
	};
}