#pragma once

#include <list>

#include "CollectableSpawnerInfo.h"
#include "ICollectableDespawner.h"
#include "ICollectableSpawner.h"
#include "IArea.h"

namespace GameServer
{
	class CollectableSpawner : public ICollectableSpawner, public ICollectableDespawner
	{
	public:
		CollectableSpawner(IArea* area, size_t size, std::uint32_t time, CollectableSpawnerInfo info) : m_Time(time), m_Area(area), m_Info(info)
		{
			for (size_t i = 0; i < size; i++)
				m_Timers.push_back(0);
		}

		virtual ~CollectableSpawner()
		{
			delete m_Area;
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

		Collectable* Spawn(size_t id) override
		{
			Vector2 position = m_Area->GetRandomPoint();
			position.y = -position.y;

			return new Collectable(*this, m_Info.ID, id, position, m_Info.Time);
		}

		void Despawn() override
		{
			m_Timers.push_back(m_Time);
		}

	protected:
		IArea* m_Area;

	private:
		std::uint32_t m_Time;
		std::list<std::uint32_t> m_Timers;
		CollectableSpawnerInfo m_Info;

	};
}