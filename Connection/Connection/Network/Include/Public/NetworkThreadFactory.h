#pragma once

namespace Connection
{
	class NetworkThreadFactory
	{
	public:
		static class INetworkThread* Create();
	};
}