#pragma once

#include <cstring>
#include <Random.h>

#include "Entity.h"
#include "PlayerExtension.h"

namespace GameServer
{
	class Player final : public Entity
	{
	public:
		explicit Player(
			size_t config,
			size_t mapID,
			Vector2 position, 
			std::uint32_t maxHP, 
			std::uint32_t maxShield, 
			std::uint32_t hp, 
			std::uint32_t shield, 
			float speed, 
			std::uint32_t minDamage, 
			std::uint32_t maxDamage,
			float attackRadius,
			std::uint8_t faction,
			float attackTime) :
			Entity(position, speed * k_SpeedFactor),
			Absortion(0.8f), 
			MaxHP(maxHP), 
			MaxShield(maxShield), 
			HP(hp), 
			Shield(shield), 
			Config(config), 
			MinDamage(minDamage), 
			MaxDamage(maxDamage),
			AttackRadius(attackRadius),
			Faction(faction),
			AttackTime(attackTime),
			m_PlayerExtension(nullptr),
			MapID(mapID) { }

		explicit Player(
			size_t config,
			size_t mapID,
			Vector2 position,
			std::uint32_t maxHP,
			std::uint32_t maxShield,
			std::uint32_t hp,
			std::uint32_t shield,
			float speed,
			std::uint32_t minDamage,
			std::uint32_t maxDamage,
			float attackRadius,
			std::uint8_t faction,
			float attackTime,
			std::string& clan,
			std::string& name,
			std::uint8_t rank) :
			Entity(position, speed * k_SpeedFactor),
			Absortion(0.8f),
			MaxHP(maxHP),
			MaxShield(maxShield),
			HP(hp),
			Shield(shield),
			Config(config),
			MinDamage(minDamage),
			MaxDamage(maxDamage),
			AttackRadius(attackRadius),
			Faction(faction),
			AttackTime(attackTime),
			m_PlayerExtension(new PlayerExtension(clan, name, rank)),
			MapID(mapID) { }

		Player(size_t mapID, Vector2 position, const Player& other) : 
			Entity(position, other.Speed), 
			Absortion(other.Absortion), 
			MaxHP(other.MaxHP), 
			MaxShield(other.MaxShield), 
			HP(other.HP), 
			Shield(other.Shield), 
			Config(other.Config), 
			MinDamage(other.MinDamage), 
			MaxDamage(other.MaxDamage),
			AttackRadius(other.AttackRadius),
			Faction(other.Faction),
			AttackTime(other.AttackTime),
			m_PlayerExtension(new PlayerExtension(*other.m_PlayerExtension)),
			MapID(mapID) { }
		
		~Player()
		{
			if (m_PlayerExtension != nullptr)
				delete m_PlayerExtension;
		}

		const std::uint32_t CalculateDamage() const
		{
			int min = static_cast<int>(MinDamage);
			int max = static_cast<int>(MaxDamage) + 1;

			int damage = Connection::Random::Range(min, max);

			return static_cast<std::uint32_t>(damage);
		}

		virtual size_t GetSize() const override
		{
			return Entity::GetSize() + sizeof(size_t) + sizeof(std::uint8_t) * 2 + sizeof(float) * 2 + sizeof(std::uint32_t) * 4 + (m_PlayerExtension != nullptr ? m_PlayerExtension->GetSize() : 0);
		}

		virtual void Serialize(Connection::StreamWriter& streamWriter) const override
		{
			streamWriter 
				<< static_cast<std::uint8_t>(m_PlayerExtension != nullptr ? 1 : 0)
				<< static_cast<size_t>(Config);

			streamWriter << static_cast<float>(AttackTime);

			Entity::Serialize(streamWriter);

			streamWriter << static_cast<float>(Speed);

			if (m_PlayerExtension != nullptr)
			{
				m_PlayerExtension->Serialize(streamWriter);
				streamWriter << static_cast<std::uint8_t>(Faction);
			}

			streamWriter 
				<< static_cast<std::uint32_t>(HP)
				<< static_cast<std::uint32_t>(MaxHP)
				<< static_cast<std::uint32_t>(Shield)
				<< static_cast<std::uint32_t>(MaxShield);
		}

		const size_t MapID;

		std::uint32_t MaxHP;
		std::uint32_t MaxShield;

		std::uint32_t HP;
		std::uint32_t Shield;

		const std::uint32_t MinDamage;
		const std::uint32_t MaxDamage;

		const float Absortion;
		const float AttackRadius;
		const float AttackTime;

		const size_t Config;

		const std::uint8_t Faction;

	private:
		static constexpr float k_SpeedFactor = 0.00815f;

		PlayerExtension* m_PlayerExtension;

	};
}