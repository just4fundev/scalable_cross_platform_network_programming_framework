#pragma once

namespace GameServer
{
	class ScriptableAreaFactory
	{
	public:
		virtual class ScriptableArea* Create(class IMatchState* match, class Vector2 position, float center) = 0;

		virtual ~ScriptableAreaFactory() = default;
	};
}