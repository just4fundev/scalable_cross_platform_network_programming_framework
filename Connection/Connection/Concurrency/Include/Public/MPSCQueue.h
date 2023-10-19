#pragma once

#include <atomic>
#include <utility>

namespace Connection
{
    /** C++ implementation of Dmitry Vyukov's lock free MPSC queue
      * http://www.1024cores.net/home/lock-free-algorithms/queues/non-intrusive-mpsc-node-based-queue */

    template<typename T> class MPSCQueueNonIntrusive final
    {
        struct Node
        {
            Node() = default;

            explicit Node(T* data) : Data(data)
            {
                Next.store(nullptr, std::memory_order_relaxed);
            }

            T* Data;
            std::atomic<Node*> Next;
        };
    public:
        MPSCQueueNonIntrusive() : mHead(new Node()), mTail(mHead.load(std::memory_order_relaxed))
        {
            Node* front = mHead.load(std::memory_order_relaxed);
            front->Next.store(nullptr, std::memory_order_relaxed);
        }

        ~MPSCQueueNonIntrusive()
        {
            T* output;
            while (Dequeue(output))
                delete output;

            Node* front = mHead.load(std::memory_order_relaxed);
            delete front;
        }

        void Enqueue(T* input)
        {
            Node* node = new Node(input);
            Node* prevHead = mHead.exchange(node, std::memory_order_acq_rel);
            prevHead->Next.store(node, std::memory_order_release);
        }

        bool Dequeue(T*& result)
        {
            Node* tail = mTail.load(std::memory_order_relaxed);
            Node* next = tail->Next.load(std::memory_order_acquire);
            if (!next)
                return false;

            result = next->Data;
            mTail.store(next, std::memory_order_release);
            delete tail;
            return true;
        }

    private:
        MPSCQueueNonIntrusive(MPSCQueueNonIntrusive const&) = delete;
        MPSCQueueNonIntrusive& operator = (MPSCQueueNonIntrusive const&) = delete;

        std::atomic<Node*> mHead;
        std::atomic<Node*> mTail;
    };

    /** C++ implementation of Dmitry Vyukov's lock free MPSC queue
      * http://www.1024cores.net/home/lock-free-algorithms/queues/intrusive-mpsc-node-based-queue */

    template<typename T, std::atomic<T*> T::* IntrusiveLink> class MPSCQueueIntrusive final
    {
    public:
        MPSCQueueIntrusive() :
            m_dummyPtr(reinterpret_cast<T*>(std::addressof(m_Dummy))),
            m_Head(m_dummyPtr),
            m_Tail(m_dummyPtr)
        {
            // _dummy is constructed from aligned_storage and is intentionally left uninitialized (it might not be default constructible)
            // so we init only its IntrusiveLink here
            std::atomic<T*>* dummyNext = new (&(m_dummyPtr->*IntrusiveLink)) std::atomic<T*>();
            dummyNext->store(nullptr, std::memory_order_relaxed);
        }

        ~MPSCQueueIntrusive()
        {
            T* output;
            while (Dequeue(output))
                delete output;
        }

        void Enqueue(T* input)
        {
            (input->*IntrusiveLink).store(nullptr, std::memory_order_release);
            T* prevHead = m_Head.exchange(input, std::memory_order_acq_rel);
            (prevHead->*IntrusiveLink).store(input, std::memory_order_release);
        }

        bool Dequeue(T*& result)
        {
            T* tail = m_Tail.load(std::memory_order_relaxed);
            T* next = (tail->*IntrusiveLink).load(std::memory_order_acquire);
            if (tail == m_dummyPtr)
            {
                if (!next)
                    return false;

                m_Tail.store(next, std::memory_order_release);
                tail = next;
                next = (next->*IntrusiveLink).load(std::memory_order_acquire);
            }

            if (next)
            {
                m_Tail.store(next, std::memory_order_release);
                result = tail;
                return true;
            }

            T* head = m_Head.load(std::memory_order_acquire);
            if (tail != head)
                return false;

            Enqueue(m_dummyPtr);
            next = (tail->*IntrusiveLink).load(std::memory_order_acquire);
            if (next)
            {
                m_Tail.store(next, std::memory_order_release);
                result = tail;
                return true;
            }
            return false;
        }

    private:
        MPSCQueueIntrusive(MPSCQueueIntrusive const&) = delete;
        MPSCQueueIntrusive& operator=(MPSCQueueIntrusive const&) = delete;

        std::aligned_storage_t<sizeof(T), alignof(T)> m_Dummy;
        T* m_dummyPtr;
        std::atomic<T*> m_Head;
        std::atomic<T*> m_Tail;
    };

    template<typename T, std::atomic<T*> T::* IntrusiveLink = nullptr> using MPSCQueue = std::conditional_t<IntrusiveLink != nullptr, MPSCQueueIntrusive<T, IntrusiveLink>, MPSCQueueNonIntrusive<T>>;
}