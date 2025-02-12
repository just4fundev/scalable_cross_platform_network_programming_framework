#pragma once

#include <Opcode.h>

namespace GameServerPublic
{
	enum ServerOpcode : Connection::Opcode
	{
		S_AuthentificationChallenge,
		S_NewPlayer,
		S_WorldState,
		S_Movement,
		S_Disconnection,
		S_YouDie,
		S_SomeoneDies,
		S_DamageDone,
		S_DamageReceived,
		S_OutOfRange,
		S_StatusUpdate,
		S_StartAttack,
		S_StopAttack,
		S_CancelAttack,
		S_AttackResponse,
		S_SomeoneJumps,
		S_JumpResult,
		S_Collecting,
		S_CollectionResult,
		S_CollectableSpawn,
		S_CollectableDespawn,
		S_DisconnectionCancelled,
		S_TimeResponse,
		S_DisconnectionStarted
	};

	enum ClientOpcode : Connection::Opcode
	{
		C_AuthentificationChallenge,
		C_Movement,
		C_StartAttack,
		C_StopAttack,
		C_Jump,
		C_Collect,
		C_KeepAlive,
		C_StartDisconnection,
		C_TimeRequest,
		C_CancelDisconnection,
		ClientOpcodeSize
	};
}