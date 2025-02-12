#pragma once

#include <unordered_set>

namespace GameServer
{
	class AccountManager final
	{
	public:
		void SignIn(size_t id) { m_IDs.insert(id); }
		void SignOut(size_t id) { m_IDs.erase(id); }

		bool IsSignedIn(size_t id) { return m_IDs.count(id) != 0; }

	private:
		std::unordered_set<size_t> m_IDs;

	};
}