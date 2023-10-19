// Copyright Cristian Pagán Díaz. All Rights Reserved.

#pragma once

namespace Connection
{
	class NetworkThreadFactory
	{
	public:
		static class INetworkThread* Create();
	};
}