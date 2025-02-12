#pragma once

#include <mutex>
#include <unordered_map>

#include <UtilityMacros.h>
#include <Database.h>
#include <DatabaseProcessor.h>

#include "ISessionObserver.h"
#include "GameSession.h"
#include "PlayerExtension.h"
#include "Match.h"
#include "IGameState.h"
#include "AccountManager.h"
#include "AccountsDatabase.h"
#include "FullAggresiveNPCAIFactory.h"
#include "DefensiveNPCAIFactory.h"
#include "DemilitarizedAreaFactory.h"

namespace GameServer
{
	class Game final : 
		public ISessionObserver,
		public IGameState,
		public IMapManager
	{
	public:
		Game(Database::Database& accountsDB, Database::Database& worldDB) : m_NextID(0), m_AccountsDB(accountsDB), m_WorldDB(worldDB)
		{
			m_NPCAIFactories.insert(std::make_pair("FullAggresive", new FullAggresiveNPCAIFactory()));
			m_NPCAIFactories.insert(std::make_pair("Defensive", new DefensiveNPCAIFactory()));

			m_ScriptableAreaFactories.insert(std::make_pair("DemilitarizedArea", new DemilitarizedAreaFactory()));
		}

		~Game()
		{
			m_DatabaseProcessor.ProcessEverithing();

			for (std::pair<const size_t, GameSession*>& session : m_Sessions)
				delete session.second;

			for (std::pair<const size_t, Match*>& match : m_Match)
				delete match.second;

			for (std::pair<const std::string, NPCAIFactory*>& npcAIFactory : m_NPCAIFactories)
				delete npcAIFactory.second;

			for (auto& scriptableAreaFactory : m_ScriptableAreaFactories)
				delete scriptableAreaFactory.second;

			for (auto& auraFactory : m_AuraFactories)
				delete auraFactory.second;

			for (Session* session : m_NewSessions)
			{
				session->Close();
				delete session;
			}
		}

		void Init()
		{
			LoadMaps();
		}

		void Update(std::uint32_t timeDifference)
		{
			AddNewSessions();

			UpdateSessions(timeDifference);
			UpdateMatches(timeDifference);

			m_DatabaseProcessor.TryProccess();
		}

		void StartLoadPlayer(size_t accountID, Session* session) override
		{
			if (!SignIn(accountID))
			{
				session->Close();
				delete session;
				return;
			}

			std::vector<boost::mysql::field> params = {
				boost::mysql::field(accountID)
			};

			m_DatabaseProcessor.AddCallback(m_AccountsDB.AsynchronousExecuteStatement(AccountsADBStmts::ADB_SelectPlayer, params, 
				[this, accountID, session] (boost::mysql::results& result) {
					EndLoadPlayer(result, accountID, session);
				}
			));
		}

		void EndLoadPlayer(boost::mysql::results& result, size_t accountID, Session* session)
		{
			Player* player = LoadPlayer(result);

			if (player == nullptr)
			{
				session->Close();
				delete session;
				return;
			}

			Match* match = LoadMatch(player->MapID);
			match->AddSession(accountID, session, player);
		}

		bool SignIn(size_t accountID)
		{
			if (m_AccountManager.IsSignedIn(accountID))
				return false;

			m_AccountManager.SignIn(accountID);

			return true;
		}

		Match* LoadMatch(size_t id)
		{
			return m_Match[id];
		}

		void Transfer(const size_t map, const Vector2 position, MatchSession* oldMatchSession) override
		{
			Match* match = LoadMatch(map);
			match->Transfer(oldMatchSession, position);
		}

	private:
		void NotifySession(Session* session) override
		{
			std::lock_guard<std::mutex> lock(m_NewSessionMutex);

			m_NewSessions.push_back(session);
		}

		void AddNewSessions()
		{
			std::lock_guard<std::mutex> lock(m_NewSessionMutex);

			for (Session* session : m_NewSessions)
			{
				size_t id = CalculateNextID();
				GameSession* newGameSession = new GameSession(session, id, this);
				newGameSession->SendAuthentificationChallenge();
				m_Sessions.insert(std::make_pair(id, newGameSession));
			}

			m_NewSessions.clear();
		}

		void UpdateSessions(std::uint32_t timeDifference)
		{
			std::unordered_map<size_t, GameSession*>::iterator itr = m_Sessions.begin();
			while (itr != m_Sessions.end())
			{
				GameSession* session = itr->second;

				if (!session->Update(timeDifference))
				{
					delete session;
					itr = m_Sessions.erase(itr);
				}
				else if (session->IsConnectionIdle())
				{
					session->Close();
					delete session;
					itr = m_Sessions.erase(itr);
				}
				else
				{
					++itr;
				}
			}
		}

		void UpdateMatches(std::uint32_t timeDifference)
		{
			for (std::pair<const size_t, Match*>& match : m_Match)
				match.second->Update(timeDifference);
		}

		size_t CalculateNextID()
		{
			size_t id = m_NextID;
			m_NextID++;
			return id;
		}

		static Player* LoadPlayer(boost::mysql::results& result)
		{
			if (!result.has_value())
				return nullptr;

			boost::mysql::row_view rowView = result.rows().at(0);

			size_t config = rowView.at(0).as_uint64();
			size_t map = rowView.at(1).as_uint64();
			float positionX = rowView.at(2).as_float();
			float positionY = rowView.at(3).as_float();
			std::uint32_t maxHP = static_cast<std::uint32_t>(rowView.at(4).as_uint64());
			std::uint32_t maxShield = static_cast<std::uint32_t>(rowView.at(5).as_uint64());
			std::uint32_t hp = static_cast<std::uint32_t>(rowView.at(6).as_uint64());
			std::uint32_t shield = static_cast<std::uint32_t>(rowView.at(7).as_uint64());
			float speed = rowView.at(8).as_float();
			std::uint32_t minDamage = static_cast<std::uint32_t>(rowView.at(9).as_uint64());
			std::uint32_t maxDamage = static_cast<std::uint32_t>(rowView.at(10).as_uint64());
			std::string clan = std::string(rowView.at(11).get_string());
			std::string name = std::string(rowView.at(12).get_string());
			std::uint8_t rank = static_cast<std::uint8_t>(rowView.at(13).as_uint64());
			std::uint8_t faction = static_cast<std::uint8_t>(rowView.at(14).as_uint64());
			float attackRadius = rowView.at(15).as_float();
			float attackTime = rowView.at(16).as_float();

			return new Player(config, map, Vector2(positionX, positionY), maxHP, maxShield, hp, shield, speed, minDamage, maxDamage, attackRadius, faction, attackTime, clan, name, rank);
		}

		void LoadMaps()
		{
			std::vector<boost::mysql::field> params = { };

			boost::mysql::results result;
			m_WorldDB.SynchronousExecuteStatement(WorldSDBStmts::SDB_SelectMaps, params, result);

			if (!result.has_value())
				return;

			boost::mysql::rows_view rows = result.rows();
			
			for (size_t i = 0; i < rows.size(); i++)
			{
				boost::mysql::row_view row = rows.at(i);
				size_t mapID = row.at(0).as_uint64();

				Match* match = new Match(m_AccountManager, m_AccountsDB, m_WorldDB, this, mapID, m_NPCAIFactories, m_ScriptableAreaFactories, m_AuraFactories);
				match->Init();

				m_Match.insert(std::make_pair(mapID, match));
			}
		}

		std::mutex m_NewSessionMutex;
		std::vector<Session*> m_NewSessions;
		std::unordered_map<size_t, GameSession*> m_Sessions;

		std::unordered_map<size_t, Match*> m_Match;

		size_t m_NextID;

		Database::Database& m_AccountsDB;
		Database::Database& m_WorldDB;

		Database::DatabaseProcessor m_DatabaseProcessor;

		AccountManager m_AccountManager;

		std::unordered_map<std::string, NPCAIFactory*> m_NPCAIFactories;
		std::unordered_map<std::string, ScriptableAreaFactory*> m_ScriptableAreaFactories;

		std::unordered_map<size_t, AuraFactory*> m_AuraFactories;

	};
}