#pragma once

#include "DatabaseWorker.h"
#include "DatabaseCallback.h"

namespace Database
{
	class AsynchronousDatabase final
	{
	public:
		AsynchronousDatabase(size_t connections)
		{
			m_Workers.reserve(connections);
			for (size_t i = 0; i < connections; i++)
				m_Workers.push_back(new DatabaseWorker(m_Queue));
		}

		~AsynchronousDatabase()
		{
			for (DatabaseWorker* worker : m_Workers)
				delete worker;
		}

		bool Open(const char* host, const char* username, const char* password, const char* database, std::vector<boost::mysql::string_view>& statements)
		{
			bool success = true;

			std::vector<DatabaseWorker*> opened;
			opened.reserve(m_Workers.size());

			for (DatabaseWorker* worker : m_Workers)
			{
				success = worker->Open(host, username, password, database, statements);

				if (success)
					opened.push_back(worker);
				else
					break;
			}

			if (!success)
			{
				for (DatabaseWorker* worker : opened)
					worker->Close();

				return false;
			}

			return true;
		}

		void Close()
		{
			for (DatabaseWorker* worker : m_Workers)
				worker->Close();
		}

		DatabaseCallback ExecuteStatement(size_t index, const std::vector<boost::mysql::field> params, DatabaseResultProcessor&& resultProcessor)
		{
			DatabaseWork* work = new DatabaseWork(
				[index, params] (SynchronousDatabaseConnection * connection, boost::mysql::results & result) -> bool {
					return connection->ExecuteStatement(index, params, result);
				}
			);

			std::future<boost::mysql::results> result = work->GetResultFuture();

			m_Queue.Push(work);

			return DatabaseCallback(std::move(resultProcessor), std::move(result));
		}

	private:
		std::vector<DatabaseWorker*> m_Workers;
		ProducerConsumerQueue<DatabaseWork*> m_Queue;

	};
}