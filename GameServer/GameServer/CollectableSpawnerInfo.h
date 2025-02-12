#pragma once

namespace GameServer
{
	class CollectableSpawnerInfo final
	{
	public:
		CollectableSpawnerInfo(const size_t id, const float time) : ID(id), Time(time) { }
		CollectableSpawnerInfo() = default;

		const size_t ID;
		const float Time;

	};
}