#pragma once

#include <IConnectionHook.h>
#include <MPSCQueue.h>
#include <ByteBuffer.h>
#include <LockedQueue.h>
#include <PreprocessedMessage.h>

namespace Connector
{
    class ConnectionHook final : public Connection::IConnectionHook
    {
    public:
        ConnectionHook(
            const bool activeTcpNoDelayProperty,
            const bool tcpNoDelay,
            const bool activeOperatingSystemReceiveBufferSizeProperty,
            const std::uint32_t operatingSystemReceiveBufferSize,
            const bool activeOperatingSystemSendBufferSizeProperty,
            const std::uint32_t operatingSystemSendBufferSize) :
            m_ActiveTcpNoDelayProperty(activeTcpNoDelayProperty),
            m_TcpNoDelay(tcpNoDelay),
            m_ActiveOperatingSystemReceiveBufferSizeProperty(activeOperatingSystemReceiveBufferSizeProperty),
            m_OperatingSystemReceiveBufferSize(operatingSystemReceiveBufferSize),
            m_ActiveOperatingSystemSendBufferSizeProperty(activeOperatingSystemSendBufferSizeProperty),
            m_OperatingSystemSendBufferSize(operatingSystemSendBufferSize) { }

        ~ConnectionHook() = default;

        bool DequeueReceivedMessage(Connection::PreprocessedMessage*& receivedMessage) override
        {
            Connection::PreprocessedMessage* preprocessedMessage = nullptr;
            bool returnValue = m_ReceivedMessages.Dequeue(preprocessedMessage);
            receivedMessage = preprocessedMessage;

            return returnValue;
        }

        bool OnStart(boost::asio::ip::tcp::socket* socket) override
        {
            if (m_ActiveOperatingSystemReceiveBufferSizeProperty)
            {
                boost::asio::socket_base::receive_buffer_size option((int)m_OperatingSystemReceiveBufferSize);

                if (!SetSocketOption(socket, option))
                    return false;
            }

            if (m_ActiveOperatingSystemSendBufferSizeProperty)
            {
                boost::asio::socket_base::send_buffer_size option((int)m_OperatingSystemSendBufferSize);

                if (!SetSocketOption(socket, option))
                    return false;
            }

            if (m_ActiveTcpNoDelayProperty)
            {
                boost::asio::ip::tcp::no_delay option(m_TcpNoDelay);

                if (!SetSocketOption(socket, option))
                    return false;
            }

            return true;
        }

        void EnqueueReceivedMessage(Connection::PreprocessedMessage&& receivedMessage) override
        {
            m_ReceivedMessages.Enqueue(new Connection::PreprocessedMessage(std::move(receivedMessage)));
        }

        void EnqueueSendingMessage(Connection::ByteBuffer&& sendingMessage) override
        {
            m_SendingMessages.Enqueue(new IntrusiveMPSCQueueByteBuffer(std::move(sendingMessage)));
        }

        bool DequeueSendingMessage(Connection::ByteBuffer*& sendingMessage) override
        {
            IntrusiveMPSCQueueByteBuffer* intrusiveByteBuffer;
            bool returnValue = m_SendingMessages.Dequeue(intrusiveByteBuffer);
            sendingMessage = intrusiveByteBuffer;

            return returnValue;
        }
        
    private:
        class IntrusiveMPSCQueueByteBuffer final : public Connection::ByteBuffer
        {
        public:
            IntrusiveMPSCQueueByteBuffer(Connection::ByteBuffer&& byteBuffer) : Connection::ByteBuffer(std::move(byteBuffer))
            {
                IntrusiveLink.store(nullptr, std::memory_order_relaxed);
            }

            virtual ~IntrusiveMPSCQueueByteBuffer() = default;

            std::atomic<IntrusiveMPSCQueueByteBuffer*> IntrusiveLink;
        };

        template <typename T> bool SetSocketOption(boost::asio::ip::tcp::socket* socket, const T& option)
        {
            boost::system::error_code errorCode;
            socket->set_option(option, errorCode);

            return !errorCode;
        }

        const bool m_ActiveTcpNoDelayProperty;
        const bool m_TcpNoDelay;
        const bool m_ActiveOperatingSystemReceiveBufferSizeProperty;
        const std::uint32_t m_OperatingSystemReceiveBufferSize;
        const bool m_ActiveOperatingSystemSendBufferSizeProperty;
        const std::uint32_t m_OperatingSystemSendBufferSize;

        Connection::LockedQueue<Connection::PreprocessedMessage> m_ReceivedMessages;
        Connection::MPSCQueue<IntrusiveMPSCQueueByteBuffer, &IntrusiveMPSCQueueByteBuffer::IntrusiveLink> m_SendingMessages;
    };
}