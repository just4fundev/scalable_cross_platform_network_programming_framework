// Copyright Cristian Pagán Díaz. All Rights Reserved.

#pragma once

#include "SessionWrapper.h"
#include "Player.h"
#include "IMatchState.h"

namespace GameServer
{
	class MatchSession final : public SessionWrapper
	{
	public:
		MatchSession(Session* session, const size_t id, const size_t accountID, const class IMatchState * const matchState, Player * const player) : 
			SessionWrapper(session, id), 
			AccountID(accountID),
			m_MatchState(matchState),
			Player(player) { }

		~MatchSession()
		{
			delete Player;
		}

		void SendNewPlayer(size_t id, Player* player) const
		{
			std::uint16_t size = (std::uint16_t)(sizeof(BaseServer::Opcode) + sizeof(size_t) + player->GetSize());

			BaseServer::OutgoingMessage outgoingMessage(size);
			outgoingMessage.GetStreamWriter()
				<< static_cast<BaseServer::Opcode>(ServerOpcode::S_NewPlayer)
				<< static_cast<size_t>(id);

			player->Serialize(outgoingMessage.GetStreamWriter());
			m_Session->Send(std::move(outgoingMessage));
		}

		void SendMovement(size_t id, Vector3& destination) const
		{
			std::uint16_t size = (std::uint16_t)(sizeof(BaseServer::Opcode) + sizeof(size_t) + sizeof(float) * 3);

			BaseServer::OutgoingMessage outgoingMessage(size);
			outgoingMessage.GetStreamWriter()
				<< static_cast<BaseServer::Opcode>(ServerOpcode::S_Movement)
				<< static_cast<size_t>(id)
				<< static_cast<float>(destination.x)
				<< static_cast<float>(destination.y)
				<< static_cast<float>(destination.z);

			m_Session->Send(std::move(outgoingMessage));
		}

		void SendDisconnection(size_t id)
		{
			BaseServer::OutgoingMessage outgoingMessage(sizeof(BaseServer::Opcode) + sizeof(size_t));
			outgoingMessage.GetStreamWriter()
				<< static_cast<BaseServer::Opcode>(ServerOpcode::S_Disconnection)
				<< static_cast<size_t>(id);

			m_Session->Send(std::move(outgoingMessage));
		}

		void SendDisconnection(size_t id) const
		{
			BaseServer::OutgoingMessage outgoingMessage(sizeof(BaseServer::Opcode) + sizeof(size_t));
			outgoingMessage.GetStreamWriter()
				<< static_cast<BaseServer::Opcode>(ServerOpcode::S_Disconnection)
				<< static_cast<size_t>(id);

			m_Session->Send(std::move(outgoingMessage));
		}

		const size_t AccountID;
		Player* const Player;

		MESSAGE_HANDLER_DECLARATION(HandleMovement)

	private:
		const class IMatchState * const m_MatchState;
	};
}