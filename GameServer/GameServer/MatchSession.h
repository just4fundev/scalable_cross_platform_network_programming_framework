#pragma once

#include "SessionWrapper.h"
#include "Player.h"
#include "IMatchState.h"
#include "IMatchSession.h"
#include "Collectable.h"
#include "Aura.h"
#include "AuraFactory.h"
#include "InternalAura.h"

#include "ShieldRegeneratorAura.h"
#include "AttackerAura.h"

#include "Messages/NewPlayerMessage.h"

namespace GameServer
{
	class MatchSession final : public SessionWrapper, public IMatchSession 
	{
	public:
		MatchSession(
			size_t accountID, 
			Session* session, 
			const size_t id, 
			class IMatchState * const matchState, 
			Player * const player, 
			std::unordered_map<size_t, AuraFactory*>& auraFactories) :
			SessionWrapper(session, id), 
			m_MatchState(matchState),
			m_PlayerInfo(player),
			m_AccountID(accountID),
			m_AuraFactories(auraFactories),
			m_DisconnectionTime()
		{
			CancelDisconnection();

			AddInternalAura(new ShieldRegeneratorAura(m_MatchState, this));
			AddInternalAura(new AttackerAura(m_MatchState, this));
		}

		MatchSession(
			const size_t id, 
			class IMatchState* const matchState, 
			MatchSession& old, 
			Player * const player) :
			SessionWrapper(old.m_Session, id),
			m_MatchState(matchState),
			m_PlayerInfo(player),
			m_AccountID(old.m_AccountID),
			m_AuraFactories(old.m_AuraFactories),
			m_DisconnectionTime(old.m_DisconnectionTime)
		{
			old.m_Session = nullptr;

			for (auto& aura : old.m_Auras)
				m_Auras.insert(std::make_pair(aura.first, aura.second->Copy(m_MatchState, this)));

			for (auto& aura : old.m_InternalAuras)
				m_InternalAuras.insert(std::make_pair(aura.first, aura.second->InternalCopy(m_MatchState, this)));
		}

		~MatchSession()
		{
			for (auto& aura : m_Auras)
				delete aura.second;

			for (auto& aura : m_InternalAuras)
				delete aura.second;

			delete m_PlayerInfo;
		}

		void SendNewPlayer(size_t id, Player* player) const override
		{
			NewPlayerMessage message(m_Session, id, player);
		}

		void SendMovement(size_t id, Vector2& destination) override
		{
			std::uint16_t size = (std::uint16_t)(sizeof(Connection::Opcode) + sizeof(size_t) + sizeof(float) * 2);

			BaseServer::OutgoingMessage outgoingMessage(size);
			outgoingMessage.GetStreamWriter()
				<< static_cast<Connection::Opcode>(GameServerPublic::ServerOpcode::S_Movement)
				<< static_cast<size_t>(id)
				<< static_cast<float>(destination.x)
				<< static_cast<float>(destination.y);

			m_Session->Send(std::move(outgoingMessage));
		}

		void SendDisconnection(size_t id) override
		{
			BaseServer::OutgoingMessage outgoingMessage(sizeof(Connection::Opcode) + sizeof(size_t));
			outgoingMessage.GetStreamWriter()
				<< static_cast<Connection::Opcode>(GameServerPublic::ServerOpcode::S_Disconnection)
				<< static_cast<size_t>(id);

			m_Session->Send(std::move(outgoingMessage));
		}

		void SendDamageDone(size_t id, std::uint32_t damage) override
		{
			BaseServer::OutgoingMessage outgoingMessage(sizeof(Connection::Opcode) + sizeof(size_t) + sizeof(std::uint32_t));
			outgoingMessage.GetStreamWriter()
				<< static_cast<Connection::Opcode>(GameServerPublic::ServerOpcode::S_DamageDone)
				<< static_cast<size_t>(id)
				<< static_cast<std::uint32_t>(damage);

			m_Session->Send(std::move(outgoingMessage));

			for (auto& aura : m_Auras)
			{
				aura.second->OnCombat();
				aura.second->OnDamageDone();
			}

			for (auto& aura : m_InternalAuras)
			{
				aura.second->OnCombat();
				aura.second->OnDamageDone();
			}
		}

		void SendDamageReceived(std::uint32_t damage) override
		{
			BaseServer::OutgoingMessage outgoingMessage(sizeof(Connection::Opcode) + sizeof(std::uint32_t));
			outgoingMessage.GetStreamWriter()
				<< static_cast<Connection::Opcode>(GameServerPublic::ServerOpcode::S_DamageReceived)
				<< static_cast<std::uint32_t>(damage);

			m_Session->Send(std::move(outgoingMessage));

			for (auto& aura : m_Auras)
			{
				aura.second->OnCombat();
				aura.second->OnDamageReceived();
			}

			for (auto& aura : m_InternalAuras)
			{
				aura.second->OnCombat();
				aura.second->OnDamageReceived();
			}
		}

		void SendOutOfRange() override
		{
			BaseServer::OutgoingMessage outgoingMessage(sizeof(Connection::Opcode));
			outgoingMessage.GetStreamWriter()
				<< static_cast<Connection::Opcode>(GameServerPublic::ServerOpcode::S_OutOfRange);

			m_Session->Send(std::move(outgoingMessage));
		}

		void SendYouDie() override
		{
			m_MatchState->SendSomeoneDies(m_ID);

			BaseServer::OutgoingMessage outgoingMessage(sizeof(Connection::Opcode));
			outgoingMessage.GetStreamWriter()
				<< static_cast<Connection::Opcode>(GameServerPublic::ServerOpcode::S_YouDie);

			m_Session->Send(std::move(outgoingMessage));
		}

		void SendSomeoneDies(size_t id) override
		{
			BaseServer::OutgoingMessage outgoingMessage(sizeof(Connection::Opcode) + sizeof(size_t));
			outgoingMessage.GetStreamWriter()
				<< static_cast<Connection::Opcode>(GameServerPublic::ServerOpcode::S_SomeoneDies)
				<< static_cast<size_t>(id);

			m_Session->Send(std::move(outgoingMessage));
		}

		void SendStatusUpdate(size_t id, std::uint32_t hp, std::uint32_t maxHP, std::uint32_t shield, std::uint32_t maxShield) override
		{
			BaseServer::OutgoingMessage outgoingMessage(sizeof(Connection::Opcode) + sizeof(size_t) + sizeof(std::uint32_t) * 4);
			outgoingMessage.GetStreamWriter()
				<< static_cast<Connection::Opcode>(GameServerPublic::ServerOpcode::S_StatusUpdate)
				<< static_cast<size_t>(id)
				<< static_cast<std::uint32_t>(hp)
				<< static_cast<std::uint32_t>(maxHP)
				<< static_cast<std::uint32_t>(shield)
				<< static_cast<std::uint32_t>(maxShield);

			m_Session->Send(std::move(outgoingMessage));
		}

		void SendStartAttack(size_t attackerID, size_t attackedID) override
		{
			BaseServer::OutgoingMessage outgoingMessage(sizeof(Connection::Opcode) + sizeof(size_t) * 2);
			outgoingMessage.GetStreamWriter()
				<< static_cast<Connection::Opcode>(GameServerPublic::ServerOpcode::S_StartAttack)
				<< static_cast<size_t>(attackerID)
				<< static_cast<size_t>(attackedID);

			m_Session->Send(std::move(outgoingMessage));
		}

		void SendStopAttack(size_t id) override
		{
			BaseServer::OutgoingMessage outgoingMessage(sizeof(Connection::Opcode) + sizeof(size_t));
			outgoingMessage.GetStreamWriter()
				<< static_cast<Connection::Opcode>(GameServerPublic::ServerOpcode::S_StopAttack)
				<< static_cast<size_t>(id);

			m_Session->Send(std::move(outgoingMessage));
		}

		void SendCancelAttack() override
		{
			BaseServer::OutgoingMessage outgoingMessage(sizeof(Connection::Opcode));
			outgoingMessage.GetStreamWriter()
				<< static_cast<Connection::Opcode>(GameServerPublic::ServerOpcode::S_CancelAttack);

			m_Session->Send(std::move(outgoingMessage));
		}

		void SendAttackResponse(bool value) override
		{
			BaseServer::OutgoingMessage outgoingMessage(sizeof(Connection::Opcode) + sizeof(std::uint8_t));
			outgoingMessage.GetStreamWriter()
				<< static_cast<Connection::Opcode>(GameServerPublic::ServerOpcode::S_AttackResponse)
				<< static_cast<std::uint8_t>(value ? 1 : 0);

			m_Session->Send(std::move(outgoingMessage));
		}

		void SendSomeoneJumps(size_t id) override
		{
			BaseServer::OutgoingMessage outgoingMessage(sizeof(Connection::Opcode) + sizeof(size_t));
			outgoingMessage.GetStreamWriter()
				<< static_cast<Connection::Opcode>(GameServerPublic::ServerOpcode::S_SomeoneJumps)
				<< static_cast<size_t>(id);

			m_Session->Send(std::move(outgoingMessage));
		}

		void SendJumpResult(bool result) override
		{
			BaseServer::OutgoingMessage outgoingMessage(sizeof(Connection::Opcode) + sizeof(std::uint8_t));
			outgoingMessage.GetStreamWriter()
				<< static_cast<Connection::Opcode>(GameServerPublic::ServerOpcode::S_JumpResult)
				<< static_cast<std::uint8_t>(result ? 1 : 0);

			m_Session->Send(std::move(outgoingMessage));
		}

		void SendCollecting(size_t id) const override
		{
			BaseServer::OutgoingMessage outgoingMessage(sizeof(Connection::Opcode) + sizeof(size_t));
			outgoingMessage.GetStreamWriter() 
				<< static_cast<Connection::Opcode>(GameServerPublic::ServerOpcode::S_Collecting)
				<< static_cast<size_t>(id);

			m_Session->Send(std::move(outgoingMessage));
		}

		void SendCollectionResult(bool result) const override
		{
			BaseServer::OutgoingMessage outgoingMessage(sizeof(Connection::Opcode) + sizeof(std::uint8_t));
			outgoingMessage.GetStreamWriter()
				<< static_cast<Connection::Opcode>(GameServerPublic::ServerOpcode::S_CollectionResult)
				<< static_cast<std::uint8_t>(result ? 1 : 0);

			m_Session->Send(std::move(outgoingMessage));
		}

		void SendCollectableSpawn(Collectable* collectable) const override
		{
			std::uint16_t size = (std::uint16_t)(sizeof(Connection::Opcode) + collectable->GetSize());

			BaseServer::OutgoingMessage outgoingMessage(size);
			outgoingMessage.GetStreamWriter()
				<< static_cast<Connection::Opcode>(GameServerPublic::ServerOpcode::S_CollectableSpawn);

			collectable->Serialize(outgoingMessage.GetStreamWriter());

			m_Session->Send(std::move(outgoingMessage));
		}

		void SendCollectableDespawn(size_t id) const override
		{
			BaseServer::OutgoingMessage outgoingMessage(sizeof(Connection::Opcode) + sizeof(size_t));
			outgoingMessage.GetStreamWriter()
				<< static_cast<Connection::Opcode>(GameServerPublic::ServerOpcode::S_CollectableDespawn)
				<< static_cast<size_t>(id);

			m_Session->Send(std::move(outgoingMessage));
		}

		void SendDisconnectionCancelled() const override
		{
			BaseServer::OutgoingMessage outgoingMessage(sizeof(Connection::Opcode));
			outgoingMessage.GetStreamWriter() << static_cast<Connection::Opcode>(GameServerPublic::ServerOpcode::S_DisconnectionCancelled);

			m_Session->Send(std::move(outgoingMessage));
		}

		bool Update(std::uint32_t timeDifference) override
		{
			const float deltaTime = timeDifference / 1000.0f;

			bool result = SessionWrapper::Update(timeDifference);

			if (result)
			{
				for (auto& aura : m_Auras)
					aura.second->Update(deltaTime);

				for (auto& aura : m_InternalAuras)
					aura.second->Update(deltaTime);
			}

			return result;
		}

		void Send(BaseServer::OutgoingMessage&& message) override { SessionWrapper::Send(std::move(message)); }

		void Close() override
		{
			SessionWrapper::Close();
		}

		bool IsConnectionIdle() const override
		{
			return SessionWrapper::IsConnectionIdle();
		}

		Player* const GetPlayer() const override { return m_PlayerInfo; }
		const size_t GetID() const override { return m_ID; }
		const size_t GetAccountID() const override { return m_AccountID; }

		bool IsPvP() const override
		{
			for (auto& aura : m_InternalAuras)
			{
				if (!aura.second->IsPvP())
					return false;
			}

			return true;
		}

		bool IsAttacking() const override
		{
			for (auto& aura : m_InternalAuras)
			{
				if (aura.second->IsAttacking())
					return true;
			}

			return false;
		}

		void AddAura(size_t id) override
		{
			Aura* aura = m_AuraFactories[id]->Create(m_MatchState, this);
			m_Auras.insert(std::make_pair(id, aura));
		}

		void RemoveAura(size_t id) override
		{
			Aura* aura = m_Auras[id];
			
			m_Auras.erase(id);
			delete aura;
		}

		void AddInternalAura(InternalAura* aura) override
		{
			m_InternalAuras.insert(std::make_pair(aura->ID, aura));
		}

		void RemoveInternalAura(size_t id) override
		{
			Aura* aura = m_InternalAuras[id];
			m_InternalAuras.erase(id);
			delete aura;
		}

		bool ShouldDisconnect(time_t time) const override
		{
			static const time_t disconnectionTime = 20;
			return (m_DisconnectionTime > 0 && time >= m_DisconnectionTime + disconnectionTime);
		}

		bool IsDisconnecting() override
		{
			return m_DisconnectionTime != 0;
		}

		void CancelDisconnection() override
		{
			m_DisconnectionTime = 0;
		}

		MESSAGE_HANDLER_DECLARATION(HandleMovement)
		MESSAGE_HANDLER_DECLARATION(HandleStartAttack)
		MESSAGE_HANDLER_DECLARATION(HandleStopAttack)
		MESSAGE_HANDLER_DECLARATION(HandleJump)
		MESSAGE_HANDLER_DECLARATION(HandleCollect)
		MESSAGE_HANDLER_DECLARATION(HandleStartDisconnection)
		MESSAGE_HANDLER_DECLARATION(HandleCancelDisconnection)
		MESSAGE_HANDLER_DECLARATION(HandleTimeRequest)

	private:
		void SendDisconnectionStarted() const
		{
			BaseServer::OutgoingMessage outgoingMessage(sizeof(Connection::Opcode));
			outgoingMessage.GetStreamWriter() << static_cast<Connection::Opcode>(GameServerPublic::ServerOpcode::S_DisconnectionStarted);

			m_Session->Send(std::move(outgoingMessage));
		}

		void SetStartDisconnectTime(time_t time)
		{
			m_DisconnectionTime = time;
		}

		void SendTimeResponse(std::uint32_t counter, std::uint32_t timeInMilliseconds) const
		{
			BaseServer::OutgoingMessage outgoingMessage(sizeof(Connection::Opcode) + sizeof(std::uint32_t) * 2);
			outgoingMessage.GetStreamWriter()
				<< static_cast<Connection::Opcode>(GameServerPublic::ServerOpcode::S_TimeResponse)
				<< static_cast<std::uint32_t>(counter)
				<< static_cast<std::uint32_t>(timeInMilliseconds);

			m_Session->Send(std::move(outgoingMessage));
		}

		class IMatchState * const m_MatchState;
		Player* const m_PlayerInfo;

		const size_t m_AccountID;

		std::unordered_map<size_t, AuraFactory*>& m_AuraFactories;

		std::unordered_map<size_t, Aura*> m_Auras;
		std::unordered_map<size_t, InternalAura*> m_InternalAuras;

		time_t m_DisconnectionTime;

	};
}