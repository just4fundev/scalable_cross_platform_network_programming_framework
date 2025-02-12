#pragma once

#include "ScriptableArea.h"
#include "DemilitarizedProtectionAura.h"

namespace GameServer
{
	class DemilitarizedArea final : public ScriptableArea
	{
	public:
		DemilitarizedArea(IMatchState* match, Vector2 position, float radius) : ScriptableArea(match, position, radius) { }
		~DemilitarizedArea() = default;

		virtual void OnAreaEnter(IMatchSession* session)
		{
			if (session->GetPlayer()->Faction < 3)
				session->AddInternalAura(new DemilitarizedProtectionAura(m_Match, session));
		}

		virtual void OnAreaExit(IMatchSession* session)
		{
			session->RemoveInternalAura(DemilitarizedProtectionAura::AuraID);
		}

	};
}