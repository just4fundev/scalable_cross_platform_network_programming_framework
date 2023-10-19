// Copyright Cristian Pagán Díaz. All Rights Reserved.

#pragma once

namespace GameServer
{
	class IGameState
	{
	public:
		virtual class Player* LoadPlayer(size_t accountID) const = 0;
		virtual class Match* LoadMatch(size_t accountID) const = 0;

		virtual ~IGameState() = default;
	};
}