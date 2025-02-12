#pragma once

namespace GameServer
{
	class ISessionExtensionAccessor
	{
	public:
		virtual bool IsConnectionIdle() const = 0;

		virtual ~ISessionExtensionAccessor() = default;
	};
}