#pragma once

#include <cstdint>

#include <SettingsProperty.h>

namespace GameServer
{
	class NetworkSettings final
	{
	public:
		NetworkSettings(
			BaseServer::SettingsProperty<std::uint16_t>& portProperty,
			BaseServer::SettingsProperty<bool>& tcpNoDelayProperty,
			BaseServer::SettingsProperty<size_t>& applicationReceiveBufferSizeProperty,
			BaseServer::SettingsProperty<size_t>& applicationSendBufferSizeProperty,
			BaseServer::SettingsProperty<std::uint32_t>& operatingSystemReceiveBufferSizeProperty,
			BaseServer::SettingsProperty<std::uint32_t>& operatingSystemSendBufferSizeProperty,
			BaseServer::SettingsProperty<size_t>& incrementBufferSizeMultiplierProperty,
			BaseServer::SettingsProperty<size_t>& threadPoolSizeProperty,
			BaseServer::SettingsProperty<const char*>& bindIpProperty) :
			m_PortProperty(portProperty),
			m_TCPNoDelayProperty(tcpNoDelayProperty),
			m_ApplicationReceiveBufferSizeProperty(applicationReceiveBufferSizeProperty),
			m_ApplicationSendBufferSizeProperty(applicationSendBufferSizeProperty),
			m_OperatingSystemReceiveBufferSizeProperty(operatingSystemReceiveBufferSizeProperty),
			m_OperatingSystemSendBufferSizeProperty(operatingSystemSendBufferSizeProperty),
			m_IncrementBufferSizeMultiplierProperty(incrementBufferSizeMultiplierProperty),
			m_ThreadPoolSizeProperty(threadPoolSizeProperty),
			m_BindIpProperty(bindIpProperty) { }

		~NetworkSettings() = default;

		BaseServer::SettingsProperty<std::uint16_t>& GetPortProperty() const { return m_PortProperty; }
		BaseServer::SettingsProperty<bool>& GetTCPNoDelayProperty() const { return m_TCPNoDelayProperty; }
		BaseServer::SettingsProperty<size_t>& GetApplicationReceiveBufferSizeProperty() const { return m_ApplicationReceiveBufferSizeProperty; }
		BaseServer::SettingsProperty<size_t>& GetApplicationSendBufferSizeProperty() const { return m_ApplicationSendBufferSizeProperty; }
		BaseServer::SettingsProperty<std::uint32_t>& GetOperatingSystemReceiveBufferSizeProperty() const { return m_OperatingSystemReceiveBufferSizeProperty; }
		BaseServer::SettingsProperty<std::uint32_t>& GetOperatingSystemSendBufferSizeProperty() const { return m_OperatingSystemSendBufferSizeProperty; }
		BaseServer::SettingsProperty<size_t>& GetIncrementBufferSizeMultiplierProperty() const { return m_IncrementBufferSizeMultiplierProperty; }
		BaseServer::SettingsProperty<size_t>& GetThreadPoolSizeProperty() const { return m_ThreadPoolSizeProperty; }
		BaseServer::SettingsProperty<const char*>& GetBindIpProperty() const { return m_BindIpProperty; }

	private:
		BaseServer::SettingsProperty<std::uint16_t>& m_PortProperty;
		BaseServer::SettingsProperty<bool>& m_TCPNoDelayProperty;
		BaseServer::SettingsProperty<size_t>& m_ApplicationReceiveBufferSizeProperty;
		BaseServer::SettingsProperty<size_t>& m_ApplicationSendBufferSizeProperty;
		BaseServer::SettingsProperty<std::uint32_t>& m_OperatingSystemReceiveBufferSizeProperty;
		BaseServer::SettingsProperty<std::uint32_t>& m_OperatingSystemSendBufferSizeProperty;
		BaseServer::SettingsProperty<size_t>& m_IncrementBufferSizeMultiplierProperty;
		BaseServer::SettingsProperty<size_t>& m_ThreadPoolSizeProperty;
		BaseServer::SettingsProperty<const char*>& m_BindIpProperty;
	};
}