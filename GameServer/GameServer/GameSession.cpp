#include "GameSession.h"
#include "Player.h"
#include "Match.h"
#include "IGameState.h"

#include <StreamReader.h>

using namespace Connection;
using namespace GameServer;

MESSAGE_HANDLER_DEFINITION(GameSession, HandleAuthentificationChallenge, 
	size_t accountID;
	data >> accountID;

	Session* s = session->m_Session;
	session->m_Session = nullptr;

	session->m_GameState->StartLoadPlayer(accountID, s);

	return false;
)