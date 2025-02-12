#pragma once

#include <boost/mysql/results.hpp>

#include <functional>
#include <future>

#include "SynchronousDatabaseConnection.h"
#include "DatabaseWorkException.h"

namespace Database
{
	class DatabaseWork final
	{
	public:
		DatabaseWork(std::function<bool(SynchronousDatabaseConnection*, boost::mysql::results&)>&& work) : m_Work(std::move(work)) {}
		~DatabaseWork() = default;

		void Do(SynchronousDatabaseConnection* connection)
		{
			boost::mysql::results result;
			bool success = m_Work(connection, result);

			if (!success)
			{
				try
				{
					throw DatabaseWorkException();
				}
				catch (...)
				{
					m_ResultPromise.set_exception(std::current_exception());
				}

				return;
			}
			
			m_ResultPromise.set_value(std::move(result));
		}

		std::future<boost::mysql::results> GetResultFuture() { return m_ResultPromise.get_future(); }

	private:
		const std::function<bool(SynchronousDatabaseConnection*, boost::mysql::results&)> m_Work;
		std::promise<boost::mysql::results> m_ResultPromise;

	};
}