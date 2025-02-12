#pragma once

#include <mutex>
#include <thread>
#include <vector>

#include "Network/Include/Public/INetworkThread.h"
#include "Network/Include/Private/Connection.h"

namespace Connection
{
    class NetworkThread final : public INetworkThread
    {
    public:
        NetworkThread() :
            m_ConnectionsCount(0),
            m_Stopped(false),
            m_Thread(nullptr),
            m_IOService(1),
            m_AcceptSocket(m_IOService),
            m_UpdateTimer(m_IOService) { }

        ~NetworkThread()
        {
            Stop();

            if (m_Thread != nullptr)
            {
                Wait();
                delete m_Thread;
            }

            m_NewConnections.clear();
        }

        void Stop() override
        {
            m_Stopped = true;
            m_IOService.stop();
        }

        bool Start() override
        {
            if (m_Thread != nullptr)
                return false;

            m_Thread = new std::thread(&NetworkThread::Run, this);

            return true;
        }

        bool Started() const override { return m_Thread != nullptr; }

        void Wait() override
        {
            m_Thread->join();

            delete m_Thread;
            m_Thread = nullptr;
        }

        void AddConnection(std::shared_ptr<IConnection> connection) override
        {
            std::lock_guard<std::mutex> lock(m_NewConnectionsMutex);

            ++m_ConnectionsCount;
            m_NewConnections.push_back(connection);
        }

        std::uint32_t GetConnectionsCount() const override { return m_ConnectionsCount; }
        tcp::socket&& GetAcceptSocket() override { return std::move(m_AcceptSocket); }

    private:
        void AddNewConnections()
        {
            std::lock_guard<std::mutex> lock(m_NewConnectionsMutex);

            if (m_NewConnections.empty())
                return;

            for (std::shared_ptr<IConnection> connection : m_NewConnections)
            {
                Connection* ptr = static_cast<Connection*>(connection.get());
                ptr->Start();

                if (connection->IsClosed())
                    --m_ConnectionsCount;
                else
                    m_Connections.push_back(connection);
            }

            m_NewConnections.clear();
        }

        void Run()
        {
            AsyncUpdate();

            m_IOService.run();

            m_Connections.clear();
        }

        void Update()
        {
            static auto removeDecision = [this](std::shared_ptr<IConnection> connection) {
                Connection* ptr = static_cast<Connection*>(connection.get());

                if (!ptr->EndSendings())
                {
                    if (!ptr->IsClosed())
                        ptr->Close();

                    --this->m_ConnectionsCount;

                    return true;
                }

                return false;
            };

            AddNewConnections();

            m_Connections.erase(std::remove_if(m_Connections.begin(), m_Connections.end(), removeDecision), m_Connections.end());

            AsyncUpdate();
        }

        void AsyncUpdate()
        {
            if (m_Stopped)
                return;

            m_UpdateTimer.expires_from_now(boost::posix_time::milliseconds(1));
            m_UpdateTimer.async_wait([this](boost::system::error_code const&) { Update(); });
        }

        std::atomic<bool> m_Stopped;
        std::thread* m_Thread;

        boost::asio::io_service m_IOService;
        boost::asio::deadline_timer m_UpdateTimer;

        tcp::socket m_AcceptSocket;

        std::vector<std::shared_ptr<IConnection>> m_Connections;
        std::atomic<std::uint32_t> m_ConnectionsCount;

        std::vector<std::shared_ptr<IConnection>> m_NewConnections;
        std::mutex m_NewConnectionsMutex;
    };
}