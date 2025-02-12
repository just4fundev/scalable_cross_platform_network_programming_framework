#pragma once

#include "InternalAura.h"
#include "FloatComparison.h"
#include "IMatchState.h"
#include "IMatchSession.h"

namespace GameServer
{
	class DemilitarizedProtectionAura final : public InternalAura
	{
	public:
		DemilitarizedProtectionAura(IMatchState* match, IMatchSession* session) :
			InternalAura(match, session, AuraID) { }

		~DemilitarizedProtectionAura() = default;

		InternalAura* InternalCopy(IMatchState* match, IMatchSession* session) override
		{
			DemilitarizedProtectionAura* aura = new DemilitarizedProtectionAura(match, session);

			return aura;
		}

		bool IsPvP() override
		{
			return m_Session->IsAttacking();
		}

		static constexpr size_t AuraID = 1;

	};
}