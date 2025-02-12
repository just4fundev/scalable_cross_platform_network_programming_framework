#include "NewPlayerMessage.h"
#include "Player.h"

#include <Opcodes.h>

GameServer::NewPlayerMessage::NewPlayerMessage(Session* destination, size_t id, Player* player) :
	Message(destination, sizeof(Connection::Opcode) + sizeof(size_t) + player->GetSize())
{
	GetData().GetStreamWriter()
		<< static_cast<Connection::Opcode>(GameServerPublic::ServerOpcode::S_NewPlayer)
		<< static_cast<size_t>(id);

	player->Serialize(GetData().GetStreamWriter());
}