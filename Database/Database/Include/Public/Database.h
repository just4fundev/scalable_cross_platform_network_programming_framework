#pragma once

#include <boost/mysql/tcp_ssl.hpp>

#include "DatabaseCallback.h"

namespace Database
{
	class Database final
	{
	public:
		Database(size_t synchronousConnections, size_t asynchronousConnections);
		~Database();

		bool Open(const char* host, const char* username, const char* password, const char* database, std::vector<boost::mysql::string_view>& synchronousStatements, std::vector<boost::mysql::string_view>& asynchronousStatements);
		void Close();

		bool SynchronousExecuteStatement(size_t index, const std::vector<boost::mysql::field>& params, boost::mysql::results& result);
		DatabaseCallback AsynchronousExecuteStatement(size_t index, const std::vector<boost::mysql::field>& params, DatabaseResultProcessor&& resultProcessor);
		
	private:
		Database(const Database&) = delete;

		class Impl;
		Impl* m_Impl;
		
	};
}