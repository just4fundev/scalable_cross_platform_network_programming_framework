// Copyright Cristian Pagán Díaz. All Rights Reserved.

#pragma once

namespace BaseServer
{
	template<typename T> class SettingsProperty final
	{
	public:
		SettingsProperty() : m_Active(false), m_Value() { }
		explicit SettingsProperty(T value) : m_Active(true), m_Value(value) { }

		SettingsProperty(SettingsProperty<T> && other) : m_Active(), m_Value()
		{
			bool active = m_Active;
			T value = m_Value;

			m_Active = other.m_Active;
			m_Value = other.m_Value;

			other.m_Active = active;
			other.m_Value = value;
		}

		~SettingsProperty() = default;

		bool IsActive() const { return m_Active; }
		T GetValue() const { return m_Value; }
	private:
		bool m_Active;
		T m_Value;
	};
}