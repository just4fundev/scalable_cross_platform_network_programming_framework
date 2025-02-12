#pragma once

#include <list>

#include "INPCSpawner.h"
#include "INPCDespawner.h"
#include "NPCSpawnerInfo.h"

namespace GameServer
{
	class NPCSpawner : public INPCSpawner, public INPCDespawner
	{
	public:
		NPCSpawner(size_t size, std::uint32_t time, NPCSpawnerInfo info) : m_Time(time), m_Info(info)
		{
			for (size_t i = 0; i < size; i++)
				m_Timers.push_back(0);
		}

		size_t Update(std::uint32_t timeDifference) override
		{
			size_t count = 0;

			while (!m_Timers.empty() && m_Timers.front() == 0)
			{
				count++;

				m_Timers.pop_front();
			}

			for (std::uint32_t& timer : m_Timers)
				timer -= timeDifference <= timer ? timeDifference : timer;

			return count;
		}

		Player* Spawn(size_t mapID) override
		{
			Vector2 position;
			position.x = Connection::Random::Range(0.0f, 210.0f);
			position.y = -Connection::Random::Range(0.0f, 135.0f);

			return new Player(m_Info.ID, mapID, position, m_Info.MaxHP, m_Info.MaxShield, m_Info.HP, m_Info.Shield, m_Info.Speed, m_Info.MinDamage, m_Info.MaxDamage, m_Info.AttackRadius, m_Info.Faction, m_Info.AttackTime);
		}

		virtual std::string GetAI()
		{
			return m_Info.AI;
		}

		void Despawn() override
		{
			m_Timers.push_back(m_Time);
		}

		virtual ~NPCSpawner() = default;

	private:
		std::uint32_t m_Time;
		std::list<std::uint32_t> m_Timers;
		NPCSpawnerInfo m_Info;

	};
}