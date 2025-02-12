#pragma once

#include "SessionWrapper.h"

namespace GameServer
{
	class GameSession final : public SessionWrapper
	{
	public:
		GameSession(Session* session, const size_t id, class IGameState* gameState) : SessionWrapper(session, id), m_GameState(gameState) { }
		
		~GameSession() = default;

		void SendAuthentificationChallenge() const
		{
			BaseServer::OutgoingMessage outgoingMessage(sizeof(Connection::Opcode));
			outgoingMessage.GetStreamWriter() << static_cast<Connection::Opcode>(GameServerPublic::ServerOpcode::S_AuthentificationChallenge);
			m_Session->Send(std::move(outgoingMessage));
		}

		bool IsConnectionIdle() const override
		{
			return m_Session->IsConnectionIdle();
		}

		MESSAGE_HANDLER_DECLARATION(HandleAuthentificationChallenge)

	private:
		class IGameState* const m_GameState;
	};
}