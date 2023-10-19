// Copyright Cristian Pagán Díaz. All Rights Reserved.

#pragma once

#include <boost/bind/bind.hpp>

#include <mutex>

#include <NetworkThreadFactory.h>
#include <INetworkThread.h>
#include <ConnectionFactory.h>
#include <PlattformMacros.h>
#include <UtilityMacros.h>
#include <IConnectionHookFactory.h>

#include "Network/Include/Public/IAcceptor.h"
#include "Network/Include/Public/IAcceptorObserver.h"

namespace BaseServer
{
	class Acceptor final : public IAcceptor
	{
	public:
		Acceptor(
			const Connection::IConnectionHookFactory* connectionHookFactory,
			const std::uint16_t port,
			const char* bindIp,
			const size_t threadPoolSize,
			const size_t applicationReceiveBufferSize,
			const size_t applicationSendBufferSize,
			const size_t incrementBufferSizeMultiplier) :
			m_ConnectionHookFactory(connectionHookFactory),
			m_Port(port),
			m_BindIp(bindIp),
			m_ApplicationReceiveBufferSize(applicationReceiveBufferSize),
			m_ApplicationSendBufferSize(applicationSendBufferSize),
			m_IncrementBufferSizeMultiplier(incrementBufferSizeMultiplier),
			m_IOService(1),
			m_Acceptor(m_IOService),
			m_ThreadCount(threadPoolSize),
			m_Closed(false),
			m_CurrentAcceptSocket(nullptr)
		{
			m_Observers.reserve(1);

			m_NetworkThreads = new Connection::INetworkThread * [m_ThreadCount];
			for (size_t i = 0; i < m_ThreadCount; i++)
				m_NetworkThreads[i] = Connection::NetworkThreadFactory::Create();
		}

		~Acceptor()
		{
			for (size_t i = 0; i < m_ThreadCount; i++)
				delete m_NetworkThreads[i];

			delete[] m_NetworkThreads;
			m_NetworkThreads = nullptr;
		}

		void Start() override
		{
			CRITICAL_SECTION(m_Mutex, 
				if (m_Closed)
					return;

				boost::system::error_code errorCode;

				boost::asio::ip::address address = MakeAddress(m_BindIp, errorCode);
				if (errorCode)
					return;

				boost::asio::ip::tcp::endpoint endpoint(address, m_Port);

				m_Acceptor.open(endpoint.protocol(), errorCode);
				if (errorCode)
					return;

				NO_WINDOWS_ACTION({
					m_Acceptor.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true), errorCode);
					if (errorCode)
						return;
				})

				m_Acceptor.bind(endpoint, errorCode);
				if (errorCode)
					return;

				m_Acceptor.listen(boost::asio::socket_base::max_listen_connections);

				for (size_t i = 0; i < m_ThreadCount; i++)
					m_NetworkThreads[i]->Start();

				Accept(false);
			)

			m_IOService.run();
			OnStopIOService();
		}

		void Close() override
		{
			if (m_Closed.exchange(true))
				return;

			m_IOService.stop();

			CRITICAL_SECTION(m_Mutex,
				boost::system::error_code errorCode;
				m_Acceptor.close(errorCode);

				for (size_t i = 0; i < m_ThreadCount; i++)
					m_NetworkThreads[i]->Stop();

				for (size_t i = 0; i < m_ThreadCount; i++)
				{
					if (m_NetworkThreads[i]->Started())
						m_NetworkThreads[i]->Wait();
				}
			)
		}

		void SubscribeObserver(IAcceptorObserver* observer) override
		{
			std::lock_guard<std::mutex> lock(m_ObserverSubjectMutex);

			m_Observers.push_back(observer);
		}

		void UnsubscribeObserver(IAcceptorObserver* observer) override
		{
			std::lock_guard<std::mutex> lock(m_ObserverSubjectMutex);

			static auto removeDecision = [&observer](IAcceptorObserver* item) {
				if (item == observer)
					return true;

				return false;
			};

			m_Observers.erase(std::remove_if(m_Observers.begin(), m_Observers.end(), removeDecision), m_Observers.end());
		}

	private:
		void Accept(bool lock = true)
		{
			if (m_Closed)
				return;

			size_t threadIndex = SelectThreadWithMinConnections();
			m_CurrentAcceptSocket = new boost::asio::ip::tcp::socket(std::move(m_NetworkThreads[threadIndex]->GetAcceptSocket()));

			AsyncAccept(threadIndex, lock);
		}

		void AsyncAccept(size_t threadIndex, bool lock)
		{
			if (lock)
			{
				CRITICAL_SECTION(m_Mutex,
					m_Acceptor.async_accept(*m_CurrentAcceptSocket, boost::bind(&Acceptor::HandleAccept, this, threadIndex, boost::asio::placeholders::error));
				)
			}
			else
			{
				m_Acceptor.async_accept(*m_CurrentAcceptSocket, boost::bind(&Acceptor::HandleAccept, this, threadIndex, boost::asio::placeholders::error));
			}
		}

		void HandleAccept(size_t threadIndex, const boost::system::error_code& error)
		{
			if (error)
				return;

			Connection::IConnectionHook* connectionHook = m_ConnectionHookFactory->Create();

			std::shared_ptr<Connection::IConnection> newConnection(
				Connection::ConnectionFactory::Create(
					m_CurrentAcceptSocket,
					connectionHook,
					m_ApplicationReceiveBufferSize,
					m_ApplicationSendBufferSize,
					m_IncrementBufferSizeMultiplier
				)
			);

			m_NetworkThreads[threadIndex]->AddConnection(newConnection);

			m_CurrentAcceptSocket = nullptr;

			NotifyObservers(newConnection);

			Accept();
		}

		boost::asio::ip::address MakeAddress(const char* ipAddressStr, boost::system::error_code& errorCode)
		{
			boost::asio::ip::address_v4 ipv4_address = boost::asio::ip::make_address_v4(ipAddressStr, errorCode);

			if (!errorCode)
				return boost::asio::ip::address(ipv4_address);

			boost::asio::ip::address_v6 ipv6_address = boost::asio::ip::make_address_v6(ipAddressStr, errorCode);

			if (!errorCode)
				return boost::asio::ip::address(ipv6_address);

			return boost::asio::ip::address();
		}

		size_t SelectThreadWithMinConnections() const
		{
			size_t min = 0;

			for (size_t i = 1; i < m_ThreadCount; ++i)
				if (m_NetworkThreads[i]->GetConnectionsCount() < m_NetworkThreads[min]->GetConnectionsCount())
					min = i;

			return min;
		}

		void NotifyObservers(std::shared_ptr<Connection::IConnection> newConnection)
		{
			std::lock_guard<std::mutex> lock(m_ObserverSubjectMutex);

			for (IAcceptorObserver* acceptorObserver : m_Observers)
				acceptorObserver->Notify(newConnection);
		}

		void OnStopIOService()
		{
			if (m_CurrentAcceptSocket != nullptr)
			{
				boost::system::error_code errorCode;
				m_CurrentAcceptSocket->close(errorCode);
				delete m_CurrentAcceptSocket;
			}
		}

		const std::uint16_t m_Port;
		const char* m_BindIp;
		const size_t m_ApplicationReceiveBufferSize;
		const size_t m_ApplicationSendBufferSize;
		const size_t m_IncrementBufferSizeMultiplier;

		boost::asio::io_service m_IOService;
		boost::asio::ip::tcp::acceptor m_Acceptor;

		const size_t m_ThreadCount;
		Connection::INetworkThread** m_NetworkThreads;

		std::atomic<bool> m_Closed;

		std::mutex m_Mutex;

		const Connection::IConnectionHookFactory* m_ConnectionHookFactory;

		std::mutex m_ObserverSubjectMutex;
		std::vector<IAcceptorObserver*> m_Observers;

		boost::asio::ip::tcp::socket* m_CurrentAcceptSocket;
	};
}