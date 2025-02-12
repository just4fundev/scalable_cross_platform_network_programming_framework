#pragma once

#include "NPCAIFactory.h"
#include "FullAggresiveNPCAI.h"

namespace GameServer
{
	class FullAggresiveNPCAIFactory final : public NPCAIFactory
	{
	public:
		virtual class NPCAI* Create(IMatchState* match, IMatchSession* npc)
		{
			return new FullAggresiveNPCAI(match, npc);
		}

		~FullAggresiveNPCAIFactory() = default;
	};
}