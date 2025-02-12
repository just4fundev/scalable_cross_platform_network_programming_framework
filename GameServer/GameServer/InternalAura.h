#pragma once

#include "Aura.h"

namespace GameServer
{
	class InternalAura abstract : public Aura
	{
	public:
		InternalAura(class IMatchState* match, class IMatchSession* session, const size_t id) : Aura(match, session), ID(id) { }

		virtual bool IsPvP() { return true; }
		virtual bool IsAttacking() { return false; }

		virtual InternalAura* InternalCopy(class IMatchState* match, class IMatchSession* session) = 0;

		virtual ~InternalAura() = default;

		Aura* Copy(IMatchState* match, IMatchSession* session) override
		{
			return InternalCopy(match, session);
		}

		const size_t ID;

	private:
		InternalAura(const InternalAura&) = delete;
		InternalAura(InternalAura&&) = delete;

	};
}