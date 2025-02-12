#pragma once

#include "Message.h"

namespace GameServer
{
	class Player;

	class NewPlayerMessage final : public Message
	{
	public:
		NewPlayerMessage(Session* destination, size_t id, Player* player);
	};
}