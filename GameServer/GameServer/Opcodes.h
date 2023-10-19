// Copyright Cristian Pagán Díaz. All Rights Reserved.

#pragma once

#include <Opcode.h>

namespace GameServer
{
	enum ServerOpcode : BaseServer::Opcode
	{
		S_AuthentificationChallenge,
		S_NewPlayer,
		S_WorldState,
		S_Movement,
		S_Disconnection,
	};

	enum ClientOpcode : BaseServer::Opcode
	{
		C_AuthentificationChallenge,
		C_Movement,
		ClientOpcodeSize
	};
}