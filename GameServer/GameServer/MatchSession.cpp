// Copyright Cristian Pagán Díaz. All Rights Reserved.

#include "MatchSession.h"

#include <StreamReader.h>

using namespace BaseServer;
using namespace GameServer;

MESSAGE_HANDLER_DEFINITION(MatchSession, HandleMovement,
	Vector3 destination;
	streamReader >> destination.x >> destination.y >> destination.z;

	session->Player->MoveTo(destination);
	session->m_MatchState->SendMovement(session->ID, destination);

	return true;
)