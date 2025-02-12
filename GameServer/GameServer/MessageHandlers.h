#pragma once

#include <MessageHandler.h>
#include <Opcodes.h>

#include "GameSession.h"
#include "MatchSession.h"

namespace GameServer
{
    const BaseServer::MessageHandler<SessionWrapper>* const GetMessageHandlers()
    {
        class HiddenHandlers final
        {
        public:
            static bool HandleNULL(GameServer::SessionWrapper*, Connection::StreamReader&, uint8_t*) { return true; }
        
        private:
            HiddenHandlers() = delete;
            HiddenHandlers(const HiddenHandlers&) = delete;
            HiddenHandlers(HiddenHandlers&&) = delete;
            ~HiddenHandlers() = delete;
        };
        
        static BaseServer::MessageHandler<SessionWrapper> messageHandlers[GameServerPublic::ClientOpcodeSize];

        messageHandlers[GameServerPublic::ClientOpcode::C_AuthentificationChallenge] = GameSession::HandleAuthentificationChallenge;
        messageHandlers[GameServerPublic::ClientOpcode::C_Movement] = MatchSession::HandleMovement;
        messageHandlers[GameServerPublic::ClientOpcode::C_StartAttack] = MatchSession::HandleStartAttack;
        messageHandlers[GameServerPublic::ClientOpcode::C_StopAttack] = MatchSession::HandleStopAttack;
        messageHandlers[GameServerPublic::ClientOpcode::C_Jump] = MatchSession::HandleJump;
        messageHandlers[GameServerPublic::ClientOpcode::C_Collect] = MatchSession::HandleCollect;
        messageHandlers[GameServerPublic::ClientOpcode::C_KeepAlive] = HiddenHandlers::HandleNULL;
        messageHandlers[GameServerPublic::ClientOpcode::C_StartDisconnection] = MatchSession::HandleStartDisconnection;
        messageHandlers[GameServerPublic::ClientOpcode::C_TimeRequest] = MatchSession::HandleTimeRequest;
        messageHandlers[GameServerPublic::ClientOpcode::C_CancelDisconnection] = MatchSession::HandleCancelDisconnection;

        return messageHandlers;
    }
}