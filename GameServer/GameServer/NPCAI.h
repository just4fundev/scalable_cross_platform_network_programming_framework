#pragma once

namespace GameServer
{
	class NPCAI
	{
	public:
		virtual void SomeoneAttacks(class IMatchSession* attacker) { }
		virtual void SomeoneDisappear(size_t id) { }
		virtual void AttackCancelled() { }

		virtual void Update(float deltaTime) = 0;

		virtual ~NPCAI() = default;

	};
}