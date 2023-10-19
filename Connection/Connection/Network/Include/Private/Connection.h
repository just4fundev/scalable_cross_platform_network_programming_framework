// Copyright Cristian Pagán Díaz. All Rights Reserved.

#pragma once

#include <queue>

#include <boost/asio.hpp>
#include <boost/bind/bind.hpp>

#include "Network/Include/Public/IConnection.h"
#include "Network/Include/Public/IConnectionHook.h"

#include "Network/Include/Private/NetworkByteBuffer.h"

using boost::asio::ip::tcp;

namespace Connection
{
    typedef std::uint16_t MessageSize;

    class Connection : public IConnection
    {
    public:
        Connection(
            tcp::socket* socket, 
            IConnectionHook* connectionHook,
            size_t aplicationReceiveBuferSize,
            size_t aplicationSendBufferSize,
            size_t incrementBufferSizeMultiplier) :
            m_Socket(socket),
            m_ConnectionHook(connectionHook),
            m_ExtractedMessage(nullptr),
            m_ProcessingQueue(false),
            m_Closed(false),
            m_AplicationReceiveBuferSize(aplicationReceiveBuferSize),
            m_AplicationSendBufferSize(aplicationSendBufferSize),
            m_IncrementBufferSizeMultiplier(incrementBufferSizeMultiplier) { }

        virtual ~Connection()
        {
            m_Closed = true;

            boost::system::error_code errorCode;
            m_Socket->close(errorCode);
            delete m_Socket;

            if (m_ExtractedMessage != nullptr)
                delete m_ExtractedMessage;

            delete m_ConnectionHook;
        }

        void Start() override
        {
            m_NetworkReceiveByteBuffer.GetByteBuffer().Resize(m_AplicationReceiveBuferSize);

            m_Socket->non_blocking(true);

            if (!m_ConnectionHook->OnStart(m_Socket))
                return;

            AsyncReadSome();
        }

        void Send(ByteBuffer&& sendingMessage) override
        {
            if (m_Closed)
                return;

            m_ConnectionHook->EnqueueSendingMessage(std::move(sendingMessage));
        }

        bool EndSendings()
        {
            NetworkByteBuffer networkByteBuffer;

            networkByteBuffer.GetByteBuffer().Resize(m_AplicationSendBufferSize);

            ByteBuffer* sendingMessage;
            while (m_ConnectionHook->DequeueSendingMessage(sendingMessage))
            {
                if (networkByteBuffer.CalculateRemainingSpace() < sendingMessage->GetSize())
                {
                    EnqueueByteBuffer(std::move(networkByteBuffer));
                    networkByteBuffer.GetByteBuffer().Resize(m_AplicationSendBufferSize);
                }

                if (networkByteBuffer.CalculateRemainingSpace() >= sendingMessage->GetSize())
                {
                    std::memmove(networkByteBuffer.GetByteBuffer().GetBytes() + networkByteBuffer.GetWritePosition().GetValue(), sendingMessage->GetBytes(), sendingMessage->GetSize());
                    networkByteBuffer.GetWritePosition() += sendingMessage->GetSize();
                }
                else
                {
                    NetworkByteBuffer singleMessage;
                    singleMessage.GetByteBuffer().Resize(sendingMessage->GetSize());

                    std::memmove(singleMessage.GetByteBuffer().GetBytes() + singleMessage.GetWritePosition().GetValue(), sendingMessage->GetBytes(), sendingMessage->GetSize());
                    singleMessage.GetWritePosition() += sendingMessage->GetSize();

                    EnqueueByteBuffer(std::move(singleMessage));
                }

                delete sendingMessage;
            }

            if (networkByteBuffer.CalculateActiveSize() > 0)
                EnqueueByteBuffer(std::move(networkByteBuffer));

            return !m_Closed;
        }

        void Close() override
        {
            if (m_Closed.exchange(true))
                return;

            boost::system::error_code errorCode;
            m_Socket->shutdown(boost::asio::socket_base::shutdown_send, errorCode);
            if (errorCode)
                return;
        }

        bool IsClosed() const override { return m_Closed; }

        bool Receive(ByteBuffer*& receivedMessage) override
        {
            return m_ConnectionHook->DequeueReceivedMessage(receivedMessage);
        }

    private:
        void AsyncReadSome()
        {
            if (m_Closed)
                return;

            NormalizeBuffer();

            size_t remainingSize = m_NetworkReceiveByteBuffer.CalculateRemainingSpace();

            if (remainingSize == 0)
                IncrementBufferSize(m_IncrementBufferSizeMultiplier);

            m_Socket->async_read_some(boost::asio::buffer(m_NetworkReceiveByteBuffer.GetByteBuffer().GetBytes() + m_NetworkReceiveByteBuffer.GetWritePosition().GetValue(), remainingSize), boost::bind(&Connection::HandleRead, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
        }

        void HandleRead(const boost::system::error_code& error, size_t bytesTransferred)
        {
            if (error)
            {
                Close();
                return;
            }

            m_NetworkReceiveByteBuffer.GetWritePosition() += bytesTransferred;

            ExtractMessages();

            AsyncReadSome();
        }

        void NormalizeBuffer()
        {
            if (m_NetworkReceiveByteBuffer.GetReadPosition().GetValue() > 0)
            {
                if (m_NetworkReceiveByteBuffer.GetReadPosition().GetValue() != m_NetworkReceiveByteBuffer.GetWritePosition().GetValue())
                {
                    uint8_t* bytes = m_NetworkReceiveByteBuffer.GetByteBuffer().GetBytes();
                    size_t activeSize = m_NetworkReceiveByteBuffer.CalculateActiveSize();

                    std::memmove(bytes, bytes + m_NetworkReceiveByteBuffer.GetReadPosition().GetValue(), activeSize);
                }

                m_NetworkReceiveByteBuffer.GetWritePosition() -= m_NetworkReceiveByteBuffer.GetReadPosition().GetValue();
                m_NetworkReceiveByteBuffer.GetReadPosition().Reset();
            }
        }

        void IncrementBufferSize(size_t multiplier)
        {
            ByteBuffer& byteBuffer = m_NetworkReceiveByteBuffer.GetByteBuffer();
            byteBuffer.Resize(byteBuffer.GetSize() * multiplier);
        }

        void ExtractMessages()
        {
            while (m_NetworkReceiveByteBuffer.CalculateActiveSize() > 0)
            {
                if (m_ExtractedMessage == nullptr)
                {
                    if (m_NetworkReceiveByteBuffer.CalculateActiveSize() < sizeof(MessageSize))
                        return;

                    MessageSize messageSize;
                    std::memmove(&messageSize, m_NetworkReceiveByteBuffer.GetByteBuffer().GetBytes() + m_NetworkReceiveByteBuffer.GetReadPosition().GetValue(), sizeof(MessageSize));
                    m_NetworkReceiveByteBuffer.GetReadPosition() += sizeof(MessageSize);

                    m_ExtractedMessage = new NetworkByteBuffer();
                    m_ExtractedMessage->GetByteBuffer().Resize(messageSize);
                }

                if (m_NetworkReceiveByteBuffer.CalculateActiveSize() < m_ExtractedMessage->CalculateRemainingSpace())
                {
                    std::memmove(m_ExtractedMessage->GetByteBuffer().GetBytes() + m_ExtractedMessage->GetWritePosition().GetValue(), m_NetworkReceiveByteBuffer.GetByteBuffer().GetBytes() + m_NetworkReceiveByteBuffer.GetReadPosition().GetValue(), m_NetworkReceiveByteBuffer.CalculateActiveSize());
                    m_ExtractedMessage->GetWritePosition() += m_NetworkReceiveByteBuffer.CalculateActiveSize();
                    m_NetworkReceiveByteBuffer.GetReadPosition() += m_NetworkReceiveByteBuffer.CalculateActiveSize();

                    return;
                }

                std::memmove(m_ExtractedMessage->GetByteBuffer().GetBytes() + m_ExtractedMessage->GetWritePosition().GetValue(), m_NetworkReceiveByteBuffer.GetByteBuffer().GetBytes() + m_NetworkReceiveByteBuffer.GetReadPosition().GetValue(), m_ExtractedMessage->CalculateRemainingSpace());
                m_NetworkReceiveByteBuffer.GetReadPosition() += m_ExtractedMessage->CalculateRemainingSpace();

                m_ConnectionHook->EnqueueReceivedMessage(std::move(m_ExtractedMessage->GetByteBuffer()));

                delete m_ExtractedMessage;
                m_ExtractedMessage = nullptr;
            }
        }

        void EnqueueByteBuffer(NetworkByteBuffer&& networkByteBuffer)
        {
            m_SensingNetworkByteBuffers.push(std::move(networkByteBuffer));

            AsyncProcessSendingQueue();
        }

        void AsyncProcessSendingQueue()
        {
            if (m_ProcessingQueue)
                return;

            m_ProcessingQueue = true;

            NetworkByteBuffer& networkByteBuffer = m_SensingNetworkByteBuffers.front();
            m_Socket->async_write_some(boost::asio::buffer(networkByteBuffer.GetByteBuffer().GetBytes() + networkByteBuffer.GetReadPosition().GetValue(), networkByteBuffer.CalculateActiveSize()), boost::bind(&Connection::WriteHandler, this, boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
        }

        void WriteHandler(const boost::system::error_code& error, size_t bytesTransferred)
        {
            if (error)
            {
                Close();
                return;
            }

            m_ProcessingQueue = false;

            NetworkByteBuffer& networkByteBuffer = m_SensingNetworkByteBuffers.front();
            networkByteBuffer.GetReadPosition() += bytesTransferred;

            if (networkByteBuffer.CalculateActiveSize() == 0)
                m_SensingNetworkByteBuffers.pop();

            if (!m_SensingNetworkByteBuffers.empty())
                AsyncProcessSendingQueue();
        }

        const size_t m_AplicationReceiveBuferSize;
        const size_t m_AplicationSendBufferSize;
        const size_t m_IncrementBufferSizeMultiplier;

        NetworkByteBuffer m_NetworkReceiveByteBuffer;
        NetworkByteBuffer* m_ExtractedMessage;

        std::queue<NetworkByteBuffer> m_SensingNetworkByteBuffers;
        bool m_ProcessingQueue;

        std::atomic<bool> m_Closed;
        tcp::socket* m_Socket;

        IConnectionHook* m_ConnectionHook;
    };
}