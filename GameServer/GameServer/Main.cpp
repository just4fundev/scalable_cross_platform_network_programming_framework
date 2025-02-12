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
#include "WorldDatabase.h"
#include "SessionExtensionSubject.h"
#include "SessionExtensionFactory.h"
#include "SessionHookFactory.h"
#include "SessionBuilder.h"
#include "GameTime.h"
#include "Timer.h"

using namespace std;
using namespace std::chrono;
using namespace boost::asio;
using namespace boost::asio::ssl;
using namespace boost::asio::ip;
using namespace boost::mysql;
using namespace boost;

using namespace GameServer;
using namespace BaseServer;
using namespace Connection;
using namespace GameServerPublic;

atomic<bool> ExecuteMainLoop(true);

time_t Time = std::time(nullptr);
uint32_t TimeDifference; 

void AbortHandler(int sigval)
{
    signal(sigval, SIG_DFL);
    boost::stacktrace::safe_dump_to("./abort.dump"); 
    raise(SIGABRT);
}

void SignalHandler(boost::system::error_code const& error, int signalNumber)
{
    if (!error)
        ExecuteMainLoop = false;
}

time_t GameServer::GameTime::GetClockSeconds()
{
    return Time;
}

void MainLoop(Game& world)
{
    uint32_t minUpdateDifference = 1; 

    uint32_t realCurrentTime = 0;
    uint32_t realPreviousTime = GetTimeInMilliseconds();

    while (ExecuteMainLoop)
    {
        realCurrentTime = GetTimeInMilliseconds();

        TimeDifference = GetTimeDifferenceInMilliseconds(realPreviousTime, realCurrentTime);
        if (TimeDifference < minUpdateDifference)
        {
            this_thread::sleep_for(milliseconds(minUpdateDifference - TimeDifference));

            continue;
        }

        Time = std::time(nullptr);
        world.Update(TimeDifference);

        realPreviousTime = realCurrentTime;
    }
}

void CriticalInitializations()
{
    steady_clock::time_point time = GetApplicationStartTime(); 

    unsigned int seed = 0;
    seed += static_cast<unsigned int>(std::time(nullptr));
    seed += static_cast<unsigned int>(time.time_since_epoch().count());

    Random::Seed(seed);
}

int main(int argc, char* argv[])
{
    CriticalInitializations();

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

    SessionExtensionSubject sessionExtensionSubject;
    SessionExtensionFactory sessionExtensionFactory(&sessionExtensionSubject);
    ISessionHookFactory* sessionHookFactory = new SessionHookFactory(&sessionExtensionFactory);

    IAcceptor* acceptor = AcceptorFactory::Create(
        connectionHookFactory,
        sessionHookFactory,
        networkSettings.GetPortProperty().GetValue(),
        networkSettings.GetBindIpProperty().GetValue(),
        networkSettings.GetThreadPoolSizeProperty().GetValue(),
        networkSettings.GetApplicationReceiveBufferSizeProperty().GetValue(),
        networkSettings.GetOperatingSystemSendBufferSizeProperty().GetValue(),
        networkSettings.GetIncrementBufferSizeMultiplierProperty().GetValue()
    );

    thread acceptorThread(&IAcceptor::Start, acceptor);

    MessageHandlerRegistry<SessionWrapper> messageHandlerRegistry(GetMessageHandlers(), ClientOpcodeSize);

    Database::Database accountsDB(1, 1);
    Database::Database worldDB(1, 1);

    std::vector<boost::mysql::string_view> accountSyncStmts;
    accountSyncStmts.resize(0, boost::mysql::string_view(""));
    std::vector<boost::mysql::string_view> accountAsyncStmts;
    accountAsyncStmts.resize(AccountsADBStmtsSize, boost::mysql::string_view(""));
    accountAsyncStmts[AccountsADBStmts::ADB_SelectPlayer] = "SELECT config, map, position_x, position_y, max_hp, max_shield, hp, shield, speed, min_damage, max_damage, clan, nickname, medal, faction, attack_radius, attack_time FROM state WHERE id = ?";
    accountAsyncStmts[AccountsADBStmts::ADB_UpdatePlayer] = "UPDATE state SET map = ?, position_x = ?, position_y = ?, hp = ?, shield = ? WHERE id = ?";

    std::vector<boost::mysql::string_view> worldSyncStmts;
    worldSyncStmts.resize(WorldSDBStmtsSize, boost::mysql::string_view(""));
    worldSyncStmts[WorldSDBStmts::SDB_SelectMaps] = "SELECT id FROM map";
    worldSyncStmts[WorldSDBStmts::SDB_SelectMapPortals] = "SELECT src.position_x, src.position_y, dst.map_id, dst.position_x, dst.position_y FROM portal AS src INNER JOIN portal AS dst ON src.destination = dst.id WHERE src.map_id = ?";
    worldSyncStmts[WorldSDBStmts::SDB_SelectMapNPC] = "SELECT spawner.count, spawner.milliseconds, npc.id, npc.max_hp, npc.max_shield, npc.hp, npc.shield, npc.speed, npc.min_damage, npc.max_damage, npc.attack_radius, npc.ai, npc.faction, npc.attack_time FROM npc_spawner AS spawner INNER JOIN npc ON spawner.npc_id = npc.id WHERE spawner.map_id = ?";
    worldSyncStmts[WorldSDBStmts::SDB_SelectMapCollectables] = "SELECT spawner.count, spawner.milliseconds, spawner.argument_1, spawner.argument_2, spawner.argument_3, collectable.id, collectable.seconds FROM collectable_spawner AS spawner INNER JOIN collectable ON spawner.collectable_id = collectable.id WHERE spawner.map_id = ?";
    worldSyncStmts[WorldSDBStmts::SDB_SelectMapAreas] = "SELECT position_x, position_y, radius, script FROM map_area where map_id = ?";
    std::vector<boost::mysql::string_view> worldAsyncStmts;
    worldAsyncStmts.resize(0, boost::mysql::string_view(""));

    SessionBuilder* sessionBuiler = new SessionBuilder(messageHandlerRegistry);
    acceptor->SubscribeObserver(sessionBuiler);
    sessionExtensionSubject.SubscribeObserver(sessionBuiler);

    Game* world = new Game(accountsDB, worldDB);
    sessionBuiler->SubscribeObserver(world);

    bool databaseOpened = true;
    databaseOpened &= accountsDB.Open("localhost", "root", "root", "accounts", accountSyncStmts, accountAsyncStmts);
    databaseOpened &= worldDB.Open("localhost", "root", "root", "world", worldSyncStmts, worldAsyncStmts);

    if (databaseOpened)
    {
        world->Init();
        MainLoop(*world);
    }

    sessionBuiler->UnsubscribeObserver(world);
    
    delete world;

    sessionExtensionSubject.UnsubscribeObserver(sessionBuiler);
    acceptor->UnsubscribeObserver(sessionBuiler);

    delete sessionBuiler;

    if (databaseOpened)
    {
        accountsDB.Close();
        worldDB.Close();
    }

    generalIOContext.stop();
    threadPool.join();

    acceptor->Close();
    acceptorThread.join();

    delete acceptor;
    delete sessionHookFactory;
    delete connectionHookFactory;

    return 0;
}