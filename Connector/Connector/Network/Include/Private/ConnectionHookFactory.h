#pragma once

#include <IConnectionHookFactory.h>

#include "ConnectionHook.h"

namespace Connector
{
	class ConnectionHookFactory final : public Connection::IConnectionHookFactory
	{
	public:
		ConnectionHookFactory(
			const bool activeTcpNoDelayProperty,
			const bool tcpNoDelay,
			const bool activeOperatingSystemReceiveBufferSizeProperty,
			const std::uint32_t operatingSystemReceiveBufferSize,
			const bool activeOperatingSystemSendBufferSizeProperty,
			const std::uint32_t operatingSystemSendBufferSize) :
			m_ActiveTcpNoDelayProperty(activeTcpNoDelayProperty),
			m_TcpNoDelay(tcpNoDelay),
			m_ActiveOperatingSystemReceiveBufferSizeProperty(activeOperatingSystemReceiveBufferSizeProperty),
			m_OperatingSystemReceiveBufferSize(operatingSystemReceiveBufferSize),
			m_ActiveOperatingSystemSendBufferSizeProperty(activeOperatingSystemSendBufferSizeProperty),
			m_OperatingSystemSendBufferSize(operatingSystemSendBufferSize) { }

		~ConnectionHookFactory() = default;

		Connection::IConnectionHook* Create() const override
		{
			return new ConnectionHook(
				m_ActiveTcpNoDelayProperty,
				m_TcpNoDelay,
				m_ActiveOperatingSystemReceiveBufferSizeProperty,
				m_OperatingSystemReceiveBufferSize,
				m_ActiveOperatingSystemSendBufferSizeProperty,
				m_OperatingSystemSendBufferSize
			);
		}

	private:
		const bool m_ActiveTcpNoDelayProperty;
		const bool m_TcpNoDelay;
		const bool m_ActiveOperatingSystemReceiveBufferSizeProperty;
		const std::uint32_t m_OperatingSystemReceiveBufferSize;
		const bool m_ActiveOperatingSystemSendBufferSizeProperty;
		const std::uint32_t m_OperatingSystemSendBufferSize;

	};
}