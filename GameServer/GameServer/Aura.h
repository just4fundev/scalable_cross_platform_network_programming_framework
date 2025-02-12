#pragma once

#include <cstdint>

namespace GameServer
{
	class Aura abstract
	{
	public:
		Aura(class IMatchState* match, class IMatchSession* session) : m_Match(match), m_Session(session) { }

		virtual void OnCombat() { }
		virtual void OnDamageReceived() { }
		virtual void OnDamageDone() { }

		virtual void Update(float deltaTime) { }

		virtual Aura* Copy(class IMatchState* match, class IMatchSession* session) = 0;

		virtual ~Aura() = default;

	protected:
		class IMatchState* m_Match;
		class IMatchSession* m_Session;

	private:
		Aura(const Aura&) = delete;
		Aura(Aura&&) = delete;

	};
}