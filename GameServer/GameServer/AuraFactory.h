#pragma once

namespace GameServer
{
	class AuraFactory
	{
	public:
		virtual class Aura* Create(class IMatchState* match, class IMatchSession* session) = 0;

		virtual ~AuraFactory() = default;
	};
}