#pragma once

namespace GameServer
{
	class IMapManager
	{
	public:
		virtual void Transfer(size_t map, const class Vector2 position, class MatchSession* oldMatchSession) = 0;

		virtual ~IMapManager() = default;
	};
}