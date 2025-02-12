#pragma once

#include <condition_variable>
#include <mutex>
#include <queue>
#include <atomic>
#include <type_traits>

namespace Database
{
    template <typename T>
    class ProducerConsumerQueue
    {
    public:
        ProducerConsumerQueue() : m_Shutdown(false) { }

        void Push(T const& value)
        {
            std::lock_guard<std::mutex> lock(m_Mutex);
            m_Queue.push(value);

            m_Condition.notify_one();
        }

        void WaitAndPop(T& value)
        {
            std::unique_lock<std::mutex> lock(m_Mutex);

            while (m_Queue.empty() && !m_Shutdown)
                m_Condition.wait(lock);

            if (m_Queue.empty() || m_Shutdown)
                return;

            value = m_Queue.front();

            m_Queue.pop();
        }

        void Cancel()
        {
            std::unique_lock<std::mutex> lock(m_Mutex);

            while (!m_Queue.empty())
            {
                T& value = m_Queue.front();

                if constexpr (std::is_pointer_v<T>)
                    delete value;

                m_Queue.pop();
            }

            m_Shutdown = true;

            m_Condition.notify_all();
        }

    private:
        mutable std::mutex m_Mutex;

        std::queue<T> m_Queue;
        std::condition_variable m_Condition;
        std::atomic<bool> m_Shutdown;

    };
}