#pragma once

#include <ISerializable.h>

#include "ICollectableDespawner.h"
#include "Vector2.h"

namespace GameServer
{
	class Collectable final : public BaseServer::ISerializable
	{
	public:
		Collectable(ICollectableDespawner& collectableDespawner, size_t config, size_t id, Vector2 position, float time) : ID(id), Position(position), m_Config(config), m_CollectableDespawner(collectableDespawner), Time(time) { }
		~Collectable() = default;

		void Despawn()
		{
			m_CollectableDespawner.Despawn();
		}

		size_t GetSize() const override { return sizeof(size_t) * 2 + sizeof(float) * 2; }

		void Serialize(Connection::StreamWriter& streamWriter) const override
		{
			streamWriter
				<< static_cast<size_t>(ID)
				<< static_cast<size_t>(m_Config)
				<< static_cast<float>(Position.x)
				<< static_cast<float>(Position.y);
		}
	
		const size_t ID;
		const Vector2 Position;
		const float Time;

	private:
		const size_t m_Config;
		ICollectableDespawner& m_CollectableDespawner;
	};
}