#pragma once

#include <memory>

#include "ISessionExtensionAccessor.h"

namespace GameServer
{
	class SessionExtension;

	class SessionExtensionAccessor final : public ISessionExtensionAccessor
	{
	public:
		SessionExtensionAccessor(std::shared_ptr<SessionExtension> sessionExtension);
		~SessionExtensionAccessor();

		bool IsConnectionIdle() const override;

	private:
		std::shared_ptr<SessionExtension> m_SessionExtension;

	};
}