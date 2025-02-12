#pragma once

#include <boost/mysql/results.hpp>
#include <future>

#include "DatabaseWorkException.h"

namespace Database
{
	using DatabaseResultProcessor = std::function<void(boost::mysql::results&)>;

	class DatabaseCallback final
	{
	public:
		DatabaseCallback(DatabaseResultProcessor&& callback, std::future<boost::mysql::results>&& result) : m_ResultProcessor(std::move(callback)), m_Result(std::move(result)) { }
		DatabaseCallback(DatabaseCallback&& right) noexcept : m_ResultProcessor(std::move(right.m_ResultProcessor)), m_Result(std::move(right.m_Result)) { }
		
		~DatabaseCallback() = default;

		bool IsReady() { return m_Result.wait_for(std::chrono::seconds(0)) == std::future_status::ready; }

		void Invoke()
		{
			try
			{
				boost::mysql::results result = m_Result.get();
				m_ResultProcessor(result);
			}
			catch (DatabaseWorkException&)
			{
				return;
			}
		}

	private:
		DatabaseCallback(DatabaseCallback const&) = delete;
		DatabaseCallback& operator=(DatabaseCallback const&) = delete;

		DatabaseResultProcessor m_ResultProcessor;
		std::future<boost::mysql::results> m_Result;

	};
}