// Copyright Cristian Pagán Díaz. All Rights Reserved.

#include "GameSession.h"
#include "Player.h"
#include "Match.h"
#include "IGameState.h"

#include <StreamReader.h>

using namespace BaseServer;
using namespace GameServer;

MESSAGE_HANDLER_DEFINITION(GameSession, HandleAuthentificationChallenge, 
	size_t accountID;
	streamReader >> accountID;

	Player* player = session->m_GameState->LoadPlayer(accountID);
	Match* match = session->m_GameState->LoadMatch(accountID);

	Session* s = session->m_Session;
	session->m_Session = nullptr;

	match->AddSession(s, accountID, player);

	return false;
)