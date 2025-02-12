#pragma once

namespace GameServer
{
	class IGameState
	{
	public:
		virtual void StartLoadPlayer(size_t accountID, class Session* session) = 0;

		virtual ~IGameState() = default;
	};
}