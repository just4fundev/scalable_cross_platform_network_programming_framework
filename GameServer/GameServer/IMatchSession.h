#pragma once

#include <cstdint>

namespace BaseServer { class OutgoingMessage; }

namespace GameServer
{
	class IMatchSession
	{
	public:
		virtual void SendNewPlayer(size_t id, class Player* player) const = 0;
		virtual void SendMovement(size_t id, class Vector2& destination) = 0;
		virtual void SendDisconnection(size_t id) = 0;
		virtual void SendDamageDone(size_t id, std::uint32_t damage) = 0;
		virtual void SendDamageReceived(std::uint32_t damage) = 0;
		virtual void SendOutOfRange() = 0;
		virtual void SendYouDie() = 0;
		virtual void SendSomeoneDies(size_t id) = 0;
		virtual void SendStatusUpdate(size_t id, std::uint32_t hp, std::uint32_t maxHP, std::uint32_t shield, std::uint32_t maxShield) = 0;
		virtual void SendStartAttack(size_t attackerID, size_t attackedID) = 0;
		virtual void SendStopAttack(size_t id) = 0;
		virtual void SendCancelAttack() = 0;
		virtual void SendAttackResponse(bool value) = 0;
		virtual void SendSomeoneJumps(size_t id) = 0;
		virtual void SendJumpResult(bool result) = 0;
		virtual void SendCollecting(size_t id) const = 0;
		virtual void SendCollectionResult(bool result) const = 0;
		virtual void SendCollectableSpawn(class Collectable* collectable) const = 0;
		virtual void SendCollectableDespawn(size_t id) const = 0;
		virtual void SendDisconnectionCancelled() const = 0;
		virtual class Player* const GetPlayer() const = 0;
		virtual const size_t GetID() const = 0;
		virtual const size_t GetAccountID() const = 0;
		virtual bool IsPvP() const = 0;
		virtual bool IsAttacking() const = 0;
		virtual void AddAura(size_t id) = 0;
		virtual void RemoveAura(size_t id) = 0;
		virtual void AddInternalAura(class InternalAura* aura) = 0;
		virtual void RemoveInternalAura(size_t id) = 0;
		virtual bool ShouldDisconnect(time_t time) const = 0;
		virtual bool IsDisconnecting() = 0;
		virtual void CancelDisconnection() = 0;

		virtual bool Update(std::uint32_t timeDifference) { return false; }
		virtual void Send(class BaseServer::OutgoingMessage&& message) { }
		virtual void Close() { }
		virtual bool IsConnectionIdle() const { return false; }

		virtual ~IMatchSession() = default;
	};
}