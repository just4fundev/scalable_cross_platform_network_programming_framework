// Copyright Cristian Pagán Díaz. All Rights Reserved.

#pragma once

#include "SessionWrapper.h"

namespace GameServer
{
	class GameSession final : public SessionWrapper
	{
	public:
		GameSession(Session* session, const size_t id, const class IGameState* const gameState) : SessionWrapper(session, id), m_GameState(gameState) { }
		
		~GameSession() = default;

		void SendAuthentificationChallenge() const
		{
			BaseServer::OutgoingMessage outgoingMessage(sizeof(BaseServer::Opcode));
			outgoingMessage.GetStreamWriter() << static_cast<BaseServer::Opcode>(ServerOpcode::S_AuthentificationChallenge);
			m_Session->Send(std::move(outgoingMessage));
		}

		MESSAGE_HANDLER_DECLARATION(HandleAuthentificationChallenge)

	private:
		const class IGameState* const m_GameState;
	};
}