// Copyright Cristian Pagán Díaz. All Rights Reserved.

#pragma once

#include <mutex>

#include <UtilityMacros.h>
#include <IAcceptorObserver.h>

#include "GameSession.h"
#include "Player.h"
#include "Match.h"
#include "IGameState.h"

namespace GameServer
{
	class Game final : 
		public BaseServer::IAcceptorObserver,
		public IGameState
	{
	public:
		Game(BaseServer::MessageHandlerRegistry<SessionWrapper>& messageHandlerRegistry) :
			m_MessageHandlerRegistry(messageHandlerRegistry),
			m_MaxProccessedPacketsPerSessionUpdate(100)
		{
			m_Match.push_back(new Match());
		}

		~Game()
		{
			for (GameSession* session : m_Sessions)
				delete session;

			for (Match* match : m_Match)
				delete match;
		}

		void Update(std::uint32_t timeDifference)
		{
			AddNewSessions();

			UpdateSessions(timeDifference);
			UpdateMatches(timeDifference);
		}

		Player* LoadPlayer(size_t accountID) const override
		{
			return new Player(Vector3(0.0f, 0.0f));
		}

		Match* LoadMatch(size_t accountID) const override
		{
			return m_Match[0];
		}

	private:
		void Notify(std::shared_ptr<Connection::IConnection> connection) override
		{
			std::lock_guard<std::mutex> lock(m_NewConnectionMutex);

			m_NewConnections.push_back(connection);
		}

		void AddNewSessions()
		{
			std::lock_guard<std::mutex> lock(m_NewConnectionMutex);

			for (std::shared_ptr<Connection::IConnection> connection : m_NewConnections)
			{
				Session* newSession = new Session(connection, m_MessageHandlerRegistry, m_MaxProccessedPacketsPerSessionUpdate);
				size_t id = m_Sessions.size();
				GameSession* newGameSession = new GameSession(newSession, id, this);
				newGameSession->SendAuthentificationChallenge();
				m_Sessions.push_back(newGameSession);
			}

			m_NewConnections.clear();
		}

		void UpdateSessions(std::uint32_t timeDifference)
		{
			std::vector<GameSession*>::iterator itr = m_Sessions.begin();
			while (itr != m_Sessions.end())
			{
				GameSession* session = *itr;

				if (!session->Update(timeDifference))
				{
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
			for (Match* match : m_Match)
				match->Update(timeDifference);
		}

		BaseServer::MessageHandlerRegistry<SessionWrapper>& m_MessageHandlerRegistry;

		std::mutex m_NewConnectionMutex;
		std::vector<std::shared_ptr<Connection::IConnection>> m_NewConnections;
		std::vector<GameSession*> m_Sessions;

		const size_t m_MaxProccessedPacketsPerSessionUpdate;

		std::vector<Match*> m_Match;
	};
}