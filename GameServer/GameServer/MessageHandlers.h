// Copyright Cristian Pagán Díaz. All Rights Reserved.

#pragma once

#include <MessageHandler.h>

#include "Opcodes.h"
#include "GameSession.h"
#include "MatchSession.h"

namespace GameServer
{
    static const BaseServer::MessageHandler<SessionWrapper>* const GetMessageHandlers()
    {
        static BaseServer::MessageHandler<SessionWrapper> messageHandlers[ClientOpcodeSize];

        messageHandlers[ClientOpcode::C_AuthentificationChallenge] = GameSession::HandleAuthentificationChallenge;
        messageHandlers[ClientOpcode::C_Movement] = MatchSession::HandleMovement;

        return messageHandlers;
    }
}