#pragma once

#include <ISerializable.h>

namespace GameServer
{
	class PlayerExtension final : public BaseServer::ISerializable
	{
	public:
		explicit PlayerExtension(
			std::string& clan,
			std::string& name,
			std::uint8_t rank) :
			m_Clan(clan),
			m_Name(name),
			Rank(rank) { }

		PlayerExtension(const PlayerExtension& other) :
			m_Clan(other.m_Clan),
			m_Name(other.m_Name),
			Rank(other.Rank) { }

		~PlayerExtension() = default;

		size_t GetSize() const override
		{
			return sizeof(size_t) * 2 + sizeof(std::uint8_t) + m_Clan.length() + m_Name.length();
		}

		void Serialize(Connection::StreamWriter& streamWriter) const override
		{
			streamWriter << static_cast<std::uint8_t>(Rank);

			SerializeString(streamWriter, m_Name);
			SerializeString(streamWriter, m_Clan);
		}

		std::string GetClan() { return m_Clan; }
		std::string GetName() { return m_Name; }

		const std::uint8_t Rank;

	private:
		static void SerializeString(Connection::StreamWriter& streamWriter, const std::string& str)
		{
			size_t length = str.length();
			streamWriter << static_cast<size_t>(length);
			for (size_t i = 0; i < length; i++)
				streamWriter << static_cast<std::uint8_t>(str[i]);
		}

		std::string m_Clan;
		std::string m_Name;

	};
}