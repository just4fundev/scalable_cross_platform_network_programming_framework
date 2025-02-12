#pragma once

#include "IMatchState.h"
#include "IMatchSession.h"
#include "Player.h"
#include "FloatComparison.h"

namespace GameServer
{
	class ScriptableArea abstract
	{
	public:
		ScriptableArea(IMatchState* match, Vector2 position, float radius) : m_Match(match), m_Position(position), m_Radius(radius) { }

		void Update()
		{
			auto& mapSessions = m_Match->GetSessions();

			std::unordered_map<size_t, IMatchSession*>::iterator iterator = m_SessionsOnArea.begin();
			while (iterator != m_SessionsOnArea.end())
			{
				size_t id = iterator->first;
				if (mapSessions.count(id) == 0)
					iterator = m_SessionsOnArea.erase(iterator);
				else
					iterator++;
			}

			for (auto& pair : mapSessions)
			{
				IMatchSession* session = pair.second;

				Player* player = session->GetPlayer();
				float distance = Vector2::Distance(m_Position, player->GetPosition());

				if (FloatComparison::LessEqual(distance, m_Radius))
				{
					if (m_SessionsOnArea.count(session->GetID()) == 0)
					{
						m_SessionsOnArea.insert(std::make_pair(session->GetID(), session));
						OnAreaEnter(session);
					}
					else
					{
						OnAreaStay(session);
					}
				}
				else
				{
					if (m_SessionsOnArea.count(session->GetID()) > 0)
					{
						m_SessionsOnArea.erase(session->GetID());
						OnAreaExit(session);
					}
				}
			}
		}

		virtual ~ScriptableArea() = default;
		
	protected:
		virtual void OnAreaEnter(IMatchSession* session) { }
		virtual void OnAreaStay(IMatchSession* session) { }
		virtual void OnAreaExit(IMatchSession* session) { }

		IMatchState* m_Match;

	private:
		std::unordered_map<size_t, IMatchSession*> m_SessionsOnArea;

		Vector2 m_Position;
		float m_Radius;

	};
}