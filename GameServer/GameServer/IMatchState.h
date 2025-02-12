#pragma once

#include <unordered_map>

namespace GameServer
{
	class IMatchState
	{
	public:
		virtual void SendMovement(size_t id, class Vector2& destination) const = 0;
		virtual void StartAttack(size_t attacker, size_t attacked) = 0;
		virtual void StopAttack(size_t attacker) = 0;
		virtual void SendSomeoneDies(size_t id) const = 0;
		virtual void SendStatusUpdate(size_t id) const = 0;
		virtual void ActiveJump(class MatchSession* jumper) = 0;
		virtual void SendSomeoneJumps(size_t jumperID) const = 0;
		virtual void Collect(class MatchSession* collector, size_t collectableID) = 0;
		virtual void SendCollectableSpawn(class Collectable* collectable) const = 0;
		virtual void SendCollectableDespawn(size_t collectableID) const = 0;
		virtual std::unordered_map<size_t, class IMatchSession*>& GetSessions() = 0;
		virtual bool IsAttacking(size_t id) = 0;

		virtual ~IMatchState() = default;
	};
}