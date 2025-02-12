#pragma once

#include "ScriptableAreaFactory.h"
#include "DemilitarizedArea.h"

namespace GameServer
{
	class DemilitarizedAreaFactory final : public ScriptableAreaFactory
	{
	public:
		virtual class ScriptableArea* Create(IMatchState* match, Vector2 position, float center)
		{
			return new DemilitarizedArea(match, position, center);
		}

		~DemilitarizedAreaFactory() = default;
	};
}