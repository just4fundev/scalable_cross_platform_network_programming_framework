#pragma once

#include "NPCAIFactory.h"
#include "DefensiveNPCAI.h"

namespace GameServer
{
	class DefensiveNPCAIFactory final : public NPCAIFactory
	{
	public:
		virtual class NPCAI* Create(IMatchState* match, IMatchSession* npc)
		{
			return new DefensiveNPCAI(match, npc);
		}

		~DefensiveNPCAIFactory() = default;
	};
}