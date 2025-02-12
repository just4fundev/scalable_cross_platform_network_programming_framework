#pragma once

namespace GameServer
{
	class NPCAIFactory
	{
	public:
		virtual class NPCAI* Create(class IMatchState* match, class IMatchSession* npc) = 0;

		virtual ~NPCAIFactory() = default;
	};
}