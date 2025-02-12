#include "Database.h"

#include "Include/Private/SynchronousDatabase.h"
#include "Include/Private/AsynchronousDatabase.h"

class Database::Database::Impl final
{
public:
	__forceinline Impl(size_t synchronousConnections, size_t asynchronousConnections) : m_SynchronousDatabase(synchronousConnections), m_AsynchronousDatabase(asynchronousConnections) { }
	__forceinline ~Impl() = default;

	__forceinline bool Open(const char* host, const char* username, const char* password, const char* database, std::vector<boost::mysql::string_view>& synchronousStatements, std::vector<boost::mysql::string_view>& asynchronousStatements)
	{
		if (!m_SynchronousDatabase.Open(host, username, password, database, synchronousStatements))
			return false;

		if (!m_AsynchronousDatabase.Open(host, username, password, database, asynchronousStatements))
		{
			m_SynchronousDatabase.Close();

			return false;
		}

		return true;
	}

	__forceinline void Close()
	{
		m_SynchronousDatabase.Close();
		m_AsynchronousDatabase.Close();
	}

	__forceinline bool SynchronousExecuteStatement(size_t index, const std::vector<boost::mysql::field>& params, boost::mysql::results& result)
	{
		return m_SynchronousDatabase.ExecuteStatement(index, params, result);
	}

	__forceinline DatabaseCallback AsynchronousExecuteStatement(size_t index, const std::vector<boost::mysql::field>& params, DatabaseResultProcessor&& resultProcessor)
	{
		return m_AsynchronousDatabase.ExecuteStatement(index, params, std::move(resultProcessor));
	}
	
private:
	SynchronousDatabase m_SynchronousDatabase;
	AsynchronousDatabase m_AsynchronousDatabase;

};

Database::Database::Database(size_t synchronousConnections, size_t asynchronousConnections) : m_Impl(new Impl(synchronousConnections, asynchronousConnections)) { }

Database::Database::~Database()
{
	delete m_Impl;
}

bool Database::Database::Open(const char* host, const char* username, const char* password, const char* database, std::vector<boost::mysql::string_view>& synchronousStatements, std::vector<boost::mysql::string_view>& asynchronousStatements)
{
	return m_Impl->Open(host, username, password, database, synchronousStatements, asynchronousStatements);
}

void Database::Database::Close()
{
	m_Impl->Close();
}

bool Database::Database::SynchronousExecuteStatement(size_t index, const std::vector<boost::mysql::field>& params, boost::mysql::results& result)
{
	return m_Impl->SynchronousExecuteStatement(index, params, result);
}

Database::DatabaseCallback Database::Database::AsynchronousExecuteStatement(size_t index, const std::vector<boost::mysql::field>& params, DatabaseResultProcessor&& resultProcessor)
{
	return m_Impl->AsynchronousExecuteStatement(index, params, std::move(resultProcessor));
}