#include "SessionExtension.h"

#include "GameTime.h"

time_t TimeOut = 60;

GameServer::SessionExtension::SessionExtension() : m_TimeOut()
{
	ResetTimeOut();
}

GameServer::SessionExtension::~SessionExtension() = default;

bool GameServer::SessionExtension::IsConnectionIdle() const
{
	return m_TimeOut < GameTime::GetClockSeconds();
}

void GameServer::SessionExtension::ResetTimeOut()
{
	m_TimeOut = GameTime::GetClockSeconds() + TimeOut;
}