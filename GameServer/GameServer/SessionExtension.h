#pragma once

#include <atomic>

#include "ISessionExtensionAccessor.h"

namespace GameServer
{
	class SessionExtension final : public ISessionExtensionAccessor
	{
	public:
		SessionExtension();
		~SessionExtension();
		
		bool IsConnectionIdle() const override;
		void ResetTimeOut();

	private:
		std::atomic<time_t> m_TimeOut;

	};
}