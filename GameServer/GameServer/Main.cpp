// Copyright Cristian Pagán Díaz. All Rights Reserved.

#include <iostream>
#include <csignal>
#include <thread>
#include <chrono>
#include <ctime>

#include <boost/asio/signal_set.hpp>
#include <boost/asio/thread_pool.hpp>
#include <boost/stacktrace.hpp>

#include <PlattformMacros.h>
#include <AcceptorFactory.h>
#include <Random.h>
#include <IAcceptor.h>

#include "NetworkSettings.h"
#include "Game.h"
#include "MessageHandlers.h"
#include "ConnectionHookFactory.h"

using namespace std;
using namespace std::chrono;
using namespace boost::asio;

using namespace GameServer;
using namespace BaseServer;
using namespace Connection;

static atomic<bool> ExecuteMainLoop(true);

static void AbortHandler(int sigval)
{
    signal(sigval, SIG_DFL);
    boost::stacktrace::safe_dump_to("./abort.dump");
    raise(SIGABRT);
}

static void SignalHandler(boost::system::error_code const& error, int signalNumber)
{
    if (!error)
        ExecuteMainLoop = false;
}

static steady_clock::time_point GetApplicationStartTime()
{
    static const steady_clock::time_point applicationStartTime = steady_clock::now();

    return applicationStartTime;
}

static uint32_t GetTimeInMilliseconds()
{
    return uint32_t(duration_cast<milliseconds>(steady_clock::now() - GetApplicationStartTime()).count());
}

static uint32_t GetTimeDifferenceInMilliseconds(uint32_t oldTimeInMilliseconds, uint32_t newTimeInMilliseconds)
{
    if (oldTimeInMilliseconds > newTimeInMilliseconds)
        return (UINT32_MAX - oldTimeInMilliseconds) + newTimeInMilliseconds;
    else
        return newTimeInMilliseconds - oldTimeInMilliseconds;
}

static void MainLoop(Game& world)
{
    uint32_t minUpdateDifference = 1;

    uint32_t realCurrentTime = 0;
    uint32_t realPreviousTime = GetTimeInMilliseconds();

    while (ExecuteMainLoop)
    {
        realCurrentTime = GetTimeInMilliseconds();

        uint32_t timeDifference = GetTimeDifferenceInMilliseconds(realPreviousTime, realCurrentTime);
        if (timeDifference < minUpdateDifference)
        {
            this_thread::sleep_for(milliseconds(minUpdateDifference - timeDifference));

            continue;
        }

        world.Update(timeDifference);

        realPreviousTime = realCurrentTime;
    }
}

int main(int argc, char* argv[])
{
    Random::Seed((unsigned int)std::time(nullptr));

    signal(SIGABRT, &AbortHandler);

    io_context generalIOContext;

    signal_set signals(generalIOContext, SIGINT, SIGTERM);
    WINDOWS_ACTION(signals.add(SIGBREAK));
    signals.async_wait(SignalHandler);

    size_t threadPoolSize = 1;
    thread_pool threadPool(threadPoolSize);

    for (size_t i = 0; i < threadPoolSize; i++)
        post(threadPool, [&generalIOContext] { generalIOContext.run(); });

    SettingsProperty<std::uint16_t> portProperty(9876);
    SettingsProperty<bool> tcpNoDelayProperty(true);
    SettingsProperty<size_t> applicationReceiveBufferSizeProperty(4096);
    SettingsProperty<size_t> applicationSendBufferSizeProperty(4096);
    SettingsProperty<std::uint32_t> operatingSystemReceiveBufferSizeProperty(8192);
    SettingsProperty<std::uint32_t> operatingSystemSendBufferSizeProperty(8192);
    SettingsProperty<size_t> incrementBufferSizeMultiplierProperty(2);
    SettingsProperty<size_t> threadPoolSizeProperty(std::thread::hardware_concurrency());
    SettingsProperty<const char*> bindIpProperty("0.0.0.0");

    NetworkSettings networkSettings(portProperty, tcpNoDelayProperty, applicationReceiveBufferSizeProperty, applicationSendBufferSizeProperty, operatingSystemReceiveBufferSizeProperty, operatingSystemSendBufferSizeProperty, incrementBufferSizeMultiplierProperty, threadPoolSizeProperty, bindIpProperty);

    const IConnectionHookFactory* connectionHookFactory = new ConnectionHookFactory(
        networkSettings.GetTCPNoDelayProperty().IsActive(),
        networkSettings.GetTCPNoDelayProperty().GetValue(),
        networkSettings.GetOperatingSystemReceiveBufferSizeProperty().IsActive(),
        networkSettings.GetOperatingSystemReceiveBufferSizeProperty().GetValue(),
        networkSettings.GetOperatingSystemSendBufferSizeProperty().IsActive(),
        networkSettings.GetOperatingSystemSendBufferSizeProperty().GetValue()
    );

    IAcceptor* acceptor = AcceptorFactory::Create(
        connectionHookFactory,
        networkSettings.GetPortProperty().GetValue(),
        networkSettings.GetBindIpProperty().GetValue(),
        networkSettings.GetThreadPoolSizeProperty().GetValue(),
        networkSettings.GetApplicationReceiveBufferSizeProperty().GetValue(),
        networkSettings.GetOperatingSystemSendBufferSizeProperty().GetValue(),
        networkSettings.GetIncrementBufferSizeMultiplierProperty().GetValue()
    );

    thread acceptorThread(&IAcceptor::Start, acceptor);

    MessageHandlerRegistry<SessionWrapper> messageHandlerRegistry(GetMessageHandlers(), ClientOpcodeSize);

    Game* world = new Game(messageHandlerRegistry);
    acceptor->SubscribeObserver(world);

    MainLoop(*world);

    acceptor->UnsubscribeObserver(world);
    delete world;

    generalIOContext.stop();
    threadPool.join();

    acceptor->Close();
    acceptorThread.join();

    delete acceptor;
    delete connectionHookFactory;

    return 0;
}