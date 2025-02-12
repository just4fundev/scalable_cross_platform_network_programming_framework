#pragma once

#include <cstdint>

namespace GameServer
{
	class NPCSpawnerInfo final
	{
	public:
		NPCSpawnerInfo(
			const size_t id,
			const std::uint32_t maxHP,
			const std::uint32_t maxShield,
			const std::uint32_t hp,
			const std::uint32_t shield,
			const float speed,
			const std::uint32_t minDamage,
			const std::uint32_t maxDamage,
			const float attackRadius,
			const std::string ai,
			const std::uint8_t faction,
			const float attackTime) :
			ID(id),
			MaxHP(maxHP),
			MaxShield(maxShield),
			HP(hp),
			Shield(shield),
			Speed(speed),
			MinDamage(minDamage),
			MaxDamage(maxDamage),
			AttackRadius(attackRadius),
			AI(ai),
			Faction(faction),
			AttackTime(attackTime) { }

		NPCSpawnerInfo() = default;

		const size_t ID;
		const std::uint32_t MaxHP;
		const std::uint32_t MaxShield;
		const std::uint32_t HP;
		const std::uint32_t Shield;
		const float Speed;
		const std::uint32_t MinDamage;
		const std::uint32_t MaxDamage;
		const float AttackRadius;
		const std::string AI;
		const std::uint8_t Faction;
		const float AttackTime;

	};
}