// Copyright Cristian Pagán Díaz. All Rights Reserved.

#pragma once

#include "IMatchState.h"
#include "MatchSession.h"
#include "CollisionEngine.h"

namespace GameServer
{
	class Match final : public IMatchState
	{
	public:
		Match() = default;

		~Match()
		{
			for (MatchSession* session : m_Sessions)
				delete session;
		}

		void AddSession(Session* session, size_t accountID, Player* player)
		{
			size_t id = m_Sessions.size();
			MatchSession* mapSession = new MatchSession(session, id, accountID, this, player);
			
			SendWorldState(mapSession);
			SendNewPlayer(id, mapSession->Player);

			m_Sessions.push_back(mapSession);
		}

		void Update(std::uint32_t timeDifference)
		{
			UpdatePlayers(timeDifference);
			UpdateSessions(timeDifference);
		}

		void SendMovement(size_t id, Vector3& destination) const override
		{
			for (MatchSession* session : m_Sessions)
			{
				if (id != session->ID)
					session->SendMovement(id, destination);
			}
		}

	private:
		void SavePlayer(size_t id, Player* player) { }

		void UpdateSessions(std::uint32_t timeDifference)
		{
			std::vector<MatchSession*>::iterator itr = m_Sessions.begin();
			while (itr != m_Sessions.end())
			{
				MatchSession* session = *itr;

				if (!session->Update(timeDifference))
				{
					SavePlayer(session->AccountID, session->Player);

					size_t id = session->ID;

					delete session;
					itr = m_Sessions.erase(itr);

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
			for (MatchSession* session : m_Sessions)
				session->Player->Update(timeDifference);
		}

		void SendDisconnection(size_t id)
		{
			for (MatchSession* session : m_Sessions)
				session->SendDisconnection(id);
		}

		void SendWorldState(MatchSession* session)
		{
			size_t entitySize = session->Player->GetSize();

			size_t playersCount = m_Sessions.size();

			std::uint16_t size = (std::uint16_t)
			(
				sizeof(BaseServer::Opcode) +
				entitySize +
				sizeof(size_t) +
				(sizeof(size_t) + entitySize) * playersCount
			);

			BaseServer::OutgoingMessage outgoingMessage(size);
			
			outgoingMessage.GetStreamWriter() << static_cast<BaseServer::Opcode>(ServerOpcode::S_WorldState);
			session->Player->Serialize(outgoingMessage.GetStreamWriter());

			outgoingMessage.GetStreamWriter() << static_cast<size_t>(playersCount);

			for (MatchSession* s : m_Sessions)
			{
				outgoingMessage.GetStreamWriter() << static_cast<size_t>(s->ID);
				s->Player->Serialize(outgoingMessage.GetStreamWriter());
			}

			session->Send(std::move(outgoingMessage));
		}

		void SendNewPlayer(size_t id, Player* player)
		{
			for (MatchSession* session : m_Sessions)
				session->SendNewPlayer(id, player);
		}

		std::vector<MatchSession*> m_Sessions;
	};
}