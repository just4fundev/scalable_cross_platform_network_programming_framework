#pragma once

#include <mutex>
#include <atomic>

#include "SynchronousDatabaseConnection.h"

namespace Database
{
	class SynchronousDatabase final
	{
	public:
		SynchronousDatabase(size_t connections)
		{
			m_Connections.reserve(connections);
			for (size_t i = 0; i < connections; i++)
				m_Connections.push_back(new BlockingDatabase());
		}

		~SynchronousDatabase()
		{
			for (BlockingDatabase* connection : m_Connections)
				delete connection;
		}

		bool Open(const char* host, const char* username, const char* password, const char* database, std::vector<boost::mysql::string_view>& statements)
		{
			bool success = true;

			std::vector<BlockingDatabase*> opened;
			opened.reserve(m_Connections.size());

			for (BlockingDatabase* connection : m_Connections)
			{
				success = connection->Open(host, username, password, database, statements);

				if (success)
					opened.push_back(connection);
				else
					break;
			}

			if (!success)
			{
				for (BlockingDatabase* connection : opened)
					connection->Close();

				return false;
			}

			return true;
		}

		void Close()
		{
			for (BlockingDatabase* connection : m_Connections)
				connection->Close();
		}

		bool ExecuteStatement(size_t index, const std::vector<boost::mysql::field>& params, boost::mysql::results& result)
		{
			BlockingDatabase* connection = Wait();
			return connection->ExecuteStatement(index, params, result);
		}

	private:
		class BlockingDatabase final
		{
		public:
			BlockingDatabase() : m_Connection() { }
			~BlockingDatabase() = default;

			bool Open(const char* host, const char* username, const char* password, const char* database, std::vector<boost::mysql::string_view>& statements)
			{
				std::lock_guard<std::mutex> lock(m_Mutex);

				return m_Connection.Open(host, username, password, database, statements);
			}

			void Close()
			{
				std::lock_guard<std::mutex> lock(m_Mutex);

				m_Connection.Close();
			}

			bool ExecuteStatement(size_t index, const std::vector<boost::mysql::field>& params, boost::mysql::results& result)
			{
				std::lock_guard<std::mutex> lock(m_Mutex);
				m_Working = true;

				bool error = m_Connection.ExecuteStatement(index, params, result);

				m_Working = false;

				return error;
			}

			bool IsWorking() { return m_Working; }

			SynchronousDatabaseConnection m_Connection;

			std::atomic<bool> m_Working;
			std::mutex m_Mutex;
		};

		BlockingDatabase* Wait()
		{
			while (true)
			{
				for (BlockingDatabase* connection : m_Connections)
				{
					if (connection->IsWorking())
						continue;

					return connection;
				}
			}
		}

		std::vector<BlockingDatabase*> m_Connections;

	};
}