#pragma once

#include <boost/mysql/tcp_ssl.hpp>
#include <vector>

namespace Database
{
	class DatabaseConnection
	{
	public:
		DatabaseConnection() :
			m_IOContext(1),
			m_SSLContext(boost::asio::ssl::context::tls_client),
			m_Connection(m_IOContext.get_executor(), m_SSLContext) { }

		virtual ~DatabaseConnection() = default;

		virtual bool Open(const char* host, const char* username, const char* password, const char* database, std::vector<boost::mysql::string_view>& statements) = 0;

		virtual void Close()
		{
			boost::mysql::error_code err;
			boost::mysql::diagnostics diag;
			m_Connection.close(err, diag);
		}

	private:
		bool PrepareStatement(size_t index, boost::mysql::string_view sql)
		{
			boost::mysql::statement& stmt = m_Statements[index];

			if (stmt.valid())
				return false;

			boost::mysql::error_code err;
			boost::mysql::diagnostics diag;

			m_Statements[index] = m_Connection.prepare_statement(sql, err, diag);

			return !err;
		}

		void CloseStatement(size_t index)
		{
			boost::mysql::statement& stmt = m_Statements[index];

			if (!stmt.valid())
				return;

			boost::mysql::error_code err;
			boost::mysql::diagnostics diag;

			m_Connection.close_statement(m_Statements[index], err, diag);
			m_Statements[index] = boost::mysql::statement();
		}

		boost::asio::io_context m_IOContext;
		boost::asio::ssl::context m_SSLContext;

	protected:
		bool Open(const char* host, const char* username, const char* password, const char* database)
		{
			boost::asio::ip::tcp::resolver resolver(m_IOContext.get_executor());
			boost::asio::ip::basic_resolver_results<boost::asio::ip::tcp> endpoints = resolver.resolve(host, boost::mysql::default_port_string);
			boost::mysql::handshake_params params(username, password, database);

			boost::mysql::error_code err;
			boost::mysql::diagnostics diag;
			m_Connection.connect(*endpoints.begin(), params, err, diag);

			return !err;
		}

		bool PrepareStatements(std::vector<boost::mysql::string_view>& statements)
		{
			m_Statements.resize(statements.size(), boost::mysql::statement());

			for (size_t i = 0; i < statements.size(); i++)
			{
				if (!PrepareStatement(i, statements[i]))
					return false;
			}

			return true;
		}

		void CloseStatements()
		{
			for (size_t i = 0; i < m_Statements.size(); i++)
				CloseStatement(i);
		}

		boost::mysql::tcp_ssl_connection m_Connection;
		std::vector<boost::mysql::statement> m_Statements;

	};
}