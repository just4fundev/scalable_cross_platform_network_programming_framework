#include "pch.h"

#include "Network/Include/Private/ConnectionManager.h"
#include "Network/Include/Private/ConnectionHookFactory.h"
#include "Network/Include/Private/SessionHookFactory.h"

#include <boost/bind/bind.hpp>

#include <vector>
#include <string>

#include <NetworkThreadFactory.h>
#include <INetworkThread.h>
#include <ConnectionFactory.h>
#include <IConnectionHookFactory.h>
#include <IConnection.h>

namespace Connector
{
    struct ConnectionManager final
    {
    public:
        ConnectionManager(
            const Connection::IConnectionHookFactory* connectionHookFactory,
            const Connection::ISessionHookFactory* sessionHookFactory,
            const size_t threadPoolSize,
            const size_t applicationReceiveBufferSize,
            const size_t applicationSendBufferSize,
            const size_t incrementBufferSizeMultiplier) :
            m_ConnectionHookFactory(connectionHookFactory),
            m_SessionHookFactory(sessionHookFactory),
            m_ApplicationReceiveBufferSize(applicationReceiveBufferSize),
            m_ApplicationSendBufferSize(applicationSendBufferSize),
            m_IncrementBufferSizeMultiplier(incrementBufferSizeMultiplier),
            m_ThreadCount(threadPoolSize),
            m_Started(false)
        {
            m_NetworkThreads = new Connection::INetworkThread * [m_ThreadCount];
            for (size_t i = 0; i < m_ThreadCount; i++)
                m_NetworkThreads[i] = Connection::NetworkThreadFactory::Create();
        }

        ~ConnectionManager()
        {
            for (size_t i = 0; i < m_ThreadCount; i++)
                delete m_NetworkThreads[i];

            delete[] m_NetworkThreads;
            m_NetworkThreads = nullptr;

            delete m_ConnectionHookFactory;
            delete m_SessionHookFactory;
        }

        void Start()
        {
            if (m_Started)
                return;

            for (size_t i = 0; i < m_ThreadCount; i++)
                m_NetworkThreads[i]->Start();

            m_Started = true;
        }

        void Stop()
        {
            if (!m_Started)
                return;

            for (size_t i = 0; i < m_ThreadCount; i++)
                m_NetworkThreads[i]->Stop();

            for (size_t i = 0; i < m_ThreadCount; i++)
            {
                if (m_NetworkThreads[i]->Started())
                    m_NetworkThreads[i]->Wait();
            }

            m_Started = false;
        }

        void AsyncConnect(std::vector<std::string> ipAddresses, std::uint16_t port, OnConnectCallback onConnectCallback)
        {
            if (!m_Started)
                return;

            size_t threadIndex = SelectThreadWithMinConnections();
            boost::asio::ip::tcp::socket* socket = new boost::asio::ip::tcp::socket(std::move(m_NetworkThreads[threadIndex]->GetAcceptSocket()));

            Connection::IConnectionHook* connectionHook = m_ConnectionHookFactory->Create();
            Connection::ISessionHook* sessionHook = m_SessionHookFactory->Create();

            std::shared_ptr<Connection::IConnection> newConnectionPtr = Connection::ConnectionFactory::Create(
                socket,
                connectionHook,
                sessionHook,
                m_ApplicationReceiveBufferSize,
                m_ApplicationSendBufferSize,
                m_IncrementBufferSizeMultiplier
            );

            std::shared_ptr<Connection::IConnection> newConnection(newConnectionPtr);

            AsyncConnect(std::move(ipAddresses), 0, port, socket, newConnection, threadIndex, onConnectCallback);
        }

        bool SendTo(size_t connectionId, Connection::ByteBuffer&& sendingMessage)
        {
            std::shared_ptr<Connection::IConnection> connection = nullptr;

            try {
                connection = m_Connections.at(connectionId);
            }
            catch (const std::out_of_range&) {
                return false;
            }

            if (connection == nullptr)
                return false;

            connection->Send(std::move(sendingMessage));

            return true;
        }

        bool ReceiveFrom(size_t connectionId, Connection::PreprocessedMessage*& receivedMessage)
        {
            std::shared_ptr<Connection::IConnection> connection = nullptr;

            try {
                connection = m_Connections.at(connectionId);
            }
            catch (const std::out_of_range&) {
                return false;
            }

            if (connection == nullptr)
                return false;

            return connection->Receive(receivedMessage);
        }

        bool CloseConnection(size_t id)
        {
            std::shared_ptr<Connection::IConnection> connection = nullptr;

            try {
                connection = m_Connections.at(id);
            }
            catch (const std::out_of_range&) {
                return false;
            }

            if (connection == nullptr)
                return false;

            connection->Close();
            m_Connections[id] = nullptr;

            return true;
        }

        bool ConnectionClosed(size_t id)
        {
            std::shared_ptr<Connection::IConnection> connection = nullptr;

            try {
                connection = m_Connections.at(id);
            }
            catch (const std::out_of_range&) {
                return true;
            }

            if (connection == nullptr)
                return true;

            return connection->IsClosed();
        }

    private:
        void HandleConnect(
            std::vector<std::string> ipAddresses,
            size_t index,
            std::uint16_t port,
            boost::asio::ip::tcp::socket* socket,
            std::shared_ptr<Connection::IConnection> newConnection,
            size_t threadIndex,
            OnConnectCallback onConnectCallback,
            const boost::system::error_code& error)
        {
            if (!error)
            {
                size_t connectionId = m_Connections.size();

                m_NetworkThreads[threadIndex]->AddConnection(newConnection);
                m_Connections.push_back(newConnection);

                onConnectCallback(false, 0, connectionId);
            }
            else if (index < ipAddresses.size())
            {
                AsyncConnect(std::move(ipAddresses), index, port, socket, newConnection, threadIndex, onConnectCallback);
            }
            else
            {
                onConnectCallback(true, error.value(), 0);

                return;
            }
        }

        void AsyncConnect(
            std::vector<std::string> ipAddresses,
            size_t index,
            std::uint16_t port,
            boost::asio::ip::tcp::socket* socket,
            std::shared_ptr<Connection::IConnection> newConnection,
            size_t threadIndex,
            OnConnectCallback onConnectCallback)
        {
            boost::asio::ip::tcp::endpoint endpoint(boost::asio::ip::address::from_string(ipAddresses[index]), port);

            socket->async_connect(endpoint, boost::bind(&ConnectionManager::HandleConnect, this,
                std::move(ipAddresses),
                index + 1,
                port,
                socket,
                newConnection,
                threadIndex,
                onConnectCallback,
                boost::asio::placeholders::error
            ));
        }

        size_t SelectThreadWithMinConnections() const
        {
            size_t min = 0;

            for (size_t i = 1; i < m_ThreadCount; ++i)
                if (m_NetworkThreads[i]->GetConnectionsCount() < m_NetworkThreads[min]->GetConnectionsCount())
                    min = i;

            return min;
        }

        const size_t m_ApplicationReceiveBufferSize;
        const size_t m_ApplicationSendBufferSize;
        const size_t m_IncrementBufferSizeMultiplier;

        bool m_Started;

        const size_t m_ThreadCount;
        Connection::INetworkThread** m_NetworkThreads;

        std::vector<std::shared_ptr<Connection::IConnection>> m_Connections;

        const Connection::IConnectionHookFactory* m_ConnectionHookFactory;
        const Connection::ISessionHookFactory* m_SessionHookFactory;
    };
}

class IncomingByteBuffer final : public Connection::ByteBuffer
{
public:
    IncomingByteBuffer(std::uint8_t* bytes, size_t size) : Connection::ByteBuffer()
    {
        m_Bytes = bytes;
        m_Size = size;
    }

    ~IncomingByteBuffer() = default;
};

class OutgoingByteBuffer final : public Connection::ByteBuffer
{
public:
    OutgoingByteBuffer(Connection::ByteBuffer&& other) : Connection::ByteBuffer(std::move(other)) { }

    ~OutgoingByteBuffer() = default;

    void Reset()
    {
        m_Bytes = nullptr;
        m_Size = 0;
    }
};

#ifdef __cplusplus
extern "C" {
#endif

    TypeName* ConnectionManagerConstructor(
        bool activeTCPNoDelay,
        bool tcpNoDelay,
        size_t applicationReceiveBufferSize,
        size_t applicationSendBufferSize,
        bool activeOperatingSystemReceiveBufferSize,
        std::uint32_t operatingSystemReceiveBufferSize,
        bool activeOperatingSystemSendBufferSize,
        std::uint32_t operatingSystemSendBufferSize,
        size_t incrementBufferSizeMultiplier,
        size_t threadPoolSize)
    {
        const Connection::IConnectionHookFactory* connectionHookFactory = new Connector::ConnectionHookFactory(
            activeTCPNoDelay,
            tcpNoDelay,
            activeOperatingSystemReceiveBufferSize,
            operatingSystemReceiveBufferSize,
            activeOperatingSystemSendBufferSize,
            operatingSystemSendBufferSize
        );

        const Connection::ISessionHookFactory* sessionHookFactory = new Connector::SessionHookFactory();

        return new Connector::ConnectionManager(
            connectionHookFactory,
            sessionHookFactory,
            threadPoolSize,
            applicationReceiveBufferSize,
            applicationSendBufferSize,
            incrementBufferSizeMultiplier
        );
    }

    void ConnectionManagerDestructor(TypeName* thisPtr)
    {
        delete thisPtr;
    }

    void ConnectionManagerStart(TypeName* thisPtr)
    {
        thisPtr->Start();
    }

    void ConnectionManagerStop(TypeName* thisPtr)
    {
        thisPtr->Stop();
    }

    void ConnectionManagerAsyncConnect(TypeName* thisPtr, void* ipAddresses, size_t ipAddressesCount, std::uint16_t port, Connector::OnConnectCallback onConnectCallback)
    {
        std::vector<std::string> ipAddressesVector;
        ipAddressesVector.reserve(ipAddressesCount);
        const char** ipAddressesArray = static_cast<const char**>(ipAddresses);

        for (size_t i = 0; i < ipAddressesCount; i++)
            ipAddressesVector.push_back(std::move(std::string(ipAddressesArray[i])));

        thisPtr->AsyncConnect(ipAddressesVector, port, onConnectCallback);
    }

    bool ConnectionManagerSendTo(TypeName* thisPtr, size_t connectionId, std::uint8_t* bytes, size_t size)
    {
        return thisPtr->SendTo(connectionId, std::move(IncomingByteBuffer(bytes, size)));
    }

    bool ConnectionManagerReceiveFrom(TypeName* thisPtr, size_t connectionId, std::uint8_t*& bytes, size_t& size)
    {
        Connection::PreprocessedMessage* preprocessedMessage;
        if (!thisPtr->ReceiveFrom(connectionId, preprocessedMessage))
            return false;

        OutgoingByteBuffer outgoingByteBuffer(std::move(preprocessedMessage->Data));
        delete preprocessedMessage;

        bytes = outgoingByteBuffer.GetBytes();
        size = outgoingByteBuffer.GetSize();

        outgoingByteBuffer.Reset();

        return true;
    }

    bool ConnectionManagerCloseConnection(TypeName* thisPtr, size_t id)
    {
        return thisPtr->CloseConnection(id);
    }

    bool ConnectionManagerConnectionClosed(TypeName* thisPtr, size_t id)
    {
        return thisPtr->ConnectionClosed(id);
    }

#ifdef __cplusplus
}
#endif