#pragma once

#include <list>

#include "DatabaseCallback.h"

namespace Database
{
	class DatabaseProcessor final
	{
	public:
		DatabaseProcessor() = default;
		~DatabaseProcessor() = default;

		void AddCallback(DatabaseCallback&& callback) { m_Callbacks.emplace_back(std::move(callback)); }

		void ProcessEverithing()
		{
			while (!m_Callbacks.empty())
				TryProccess();
		}

		void TryProccess()
		{
			std::list<DatabaseCallback>::iterator itr = m_Callbacks.begin();
			while (itr != m_Callbacks.end())
			{
				DatabaseCallback& callback = *itr;

				if (callback.IsReady())
				{
					callback.Invoke();

					itr = m_Callbacks.erase(itr);
				}
				else
				{
					itr++;
				}
			}
		}

	private:
		DatabaseProcessor(const DatabaseProcessor& other) = delete;

		std::list<DatabaseCallback> m_Callbacks;

	};
}