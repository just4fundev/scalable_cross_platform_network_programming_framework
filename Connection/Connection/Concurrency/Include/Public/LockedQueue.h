#pragma once

#include <queue>
#include <mutex>

namespace Connection
{
	template <class T> class LockedQueue final
	{
    public:
        LockedQueue() = default;

        ~LockedQueue()
        {
            T* output;
            while (Dequeue(output))
                delete output;
        }

        void Enqueue(T* input)
        {
            std::lock_guard<std::mutex> lock(m_Lock);
            m_Queue.push(input);
        }

        bool Dequeue(T*& result)
        {
            std::lock_guard<std::mutex> lock(m_Lock);

            if (m_Queue.empty())
                return false;

            result = m_Queue.front();
            m_Queue.pop();

            return true;
        }

    private:
        LockedQueue(LockedQueue const&) = delete;
        LockedQueue& operator = (LockedQueue const&) = delete;

        std::mutex m_Lock;
        std::queue<T*> m_Queue;

	};
}