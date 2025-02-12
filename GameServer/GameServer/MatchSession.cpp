#include "MatchSession.h"
#include "GameTime.h"

#include <StreamReader.h>

#include <memory>

using namespace Connection;
using namespace GameServer;

MESSAGE_HANDLER_DEFINITION(MatchSession, HandleMovement,
	Vector2 destination;
	data >> destination.x >> destination.y;

	session->GetPlayer()->MoveTo(destination);
	session->m_MatchState->SendMovement(session->GetID(), destination);

	return true;
)

MESSAGE_HANDLER_DEFINITION(MatchSession, HandleStartAttack,
	size_t id;
	data >> id;

	session->m_MatchState->StartAttack(session->GetID(), id);

	return true;
)

MESSAGE_HANDLER_DEFINITION(MatchSession, HandleStopAttack,
	session->m_MatchState->StopAttack(session->GetID());

	return true;
)

MESSAGE_HANDLER_DEFINITION(MatchSession, HandleJump,
	session->m_MatchState->ActiveJump(session);

	return true;
)

MESSAGE_HANDLER_DEFINITION(MatchSession, HandleCollect,
	size_t id;
	data >> id;

	session->m_MatchState->Collect(session, id);

	return true;
)

MESSAGE_HANDLER_DEFINITION(MatchSession, HandleStartDisconnection,
	if (session->IsDisconnecting())
		return true;

	session->SendDisconnectionStarted();
	session->SetStartDisconnectTime(GameTime::GetClockSeconds());

	return true;
)

MESSAGE_HANDLER_DEFINITION(MatchSession, HandleCancelDisconnection,
	if (!session->IsDisconnecting())
		return true;

	session->CancelDisconnection();
	session->SendDisconnectionCancelled();

	return true;
)

bool MatchSession::HandleTimeRequest(SessionWrapper * sessionWrapper, StreamReader & data, std::uint8_t* metadata) {
	MatchSession* session = static_cast<MatchSession*>(sessionWrapper);

	std::uint32_t counter;
	data >> counter;

	std::uint32_t timeInMilliseconds;
	std::memcpy(&timeInMilliseconds, metadata, sizeof(timeInMilliseconds));

	session->SendTimeResponse(counter, timeInMilliseconds);
	
	return true;
}