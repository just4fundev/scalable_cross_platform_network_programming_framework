#pragma once

#include <tuple>

#include "DatabaseConnection.h"

namespace Database
{
	class SynchronousDatabaseConnection : public DatabaseConnection
	{
	public:
		SynchronousDatabaseConnection() : DatabaseConnection() { }
		virtual ~SynchronousDatabaseConnection() = default;

		virtual bool Open(const char* host, const char* username, const char* password, const char* database, std::vector<boost::mysql::string_view>& statements) override
		{
			if (!DatabaseConnection::Open(host, username, password, database))
				return false;

			if (!PrepareStatements(statements))
			{
				CloseStatements();

				return false;
			}

			return true;
		}

		virtual void Close() override
		{
			CloseStatements();

			DatabaseConnection::Close();
		}

		bool ExecuteStatement(size_t index, const std::vector<boost::mysql::field>& params, boost::mysql::results& result)
		{
			boost::mysql::error_code err;
			boost::mysql::diagnostics diag;

			boost::mysql::statement& stmt = m_Statements[index];
			m_Connection.execute(stmt.bind(params.begin(), params.end()), result, err, diag);

			return !err;
		}

	};
}