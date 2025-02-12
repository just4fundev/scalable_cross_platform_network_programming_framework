#pragma once

#include <thread>

#include "ProducerConsumerQueue.h"
#include "DatabaseWork.h"

namespace Database
{
	class DatabaseWorker final : public SynchronousDatabaseConnection
	{
	public:
		DatabaseWorker(ProducerConsumerQueue<DatabaseWork*>& queue) : SynchronousDatabaseConnection(), m_Queue(queue), m_Closed(true) { }
		~DatabaseWorker() = default;

		bool Open(const char* host, const char* username, const char* password, const char* database, std::vector<boost::mysql::string_view>& statements) override
		{
			if (!SynchronousDatabaseConnection::Open(host, username, password, database, statements))
				return false;

			m_Closed = false;
			m_Thread = std::thread(&DatabaseWorker::Work, this);

			return true;
		}

		void Close() override
		{
			m_Closed = true;
			m_Queue.Cancel();
			m_Thread.join();

			m_Mutex.lock();
			CloseStatements();
			DatabaseConnection::Close();
			m_Mutex.unlock();
		}

	private:
		void Work()
		{
			while (true)
			{
				DatabaseWork* databaseWork = nullptr;

				m_Queue.WaitAndPop(databaseWork);

				if (m_Closed || !databaseWork)
					return;

				m_Mutex.lock();
				databaseWork->Do(this);
				m_Mutex.unlock();

				delete databaseWork;
			}
		}

		std::thread m_Thread;
		std::atomic<bool> m_Closed;
		std::mutex m_Mutex;

		ProducerConsumerQueue<DatabaseWork*>& m_Queue;

	};
}