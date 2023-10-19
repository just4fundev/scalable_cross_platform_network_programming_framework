// Copyright Cristian Pagán Díaz. All Rights Reserved.

#pragma once

namespace GameServer
{
	class IMatchState
	{
	public:
		virtual void SendMovement(size_t id, class Vector3& destination) const = 0;

		virtual ~IMatchState() = default;
	};
}