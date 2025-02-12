#pragma once

#include <chrono>

inline std::chrono::steady_clock::time_point GetApplicationStartTime()
{
    static const std::chrono::steady_clock::time_point applicationStartTime = std::chrono::steady_clock::now();

    return applicationStartTime;
}

inline uint32_t GetTimeInMilliseconds()
{
    return uint32_t(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - GetApplicationStartTime()).count());
}

inline uint32_t GetTimeDifferenceInMilliseconds(uint32_t oldTimeInMilliseconds, uint32_t newTimeInMilliseconds)
{
    if (oldTimeInMilliseconds > newTimeInMilliseconds)
        return (UINT32_MAX - oldTimeInMilliseconds) + newTimeInMilliseconds;
    else
        return newTimeInMilliseconds - oldTimeInMilliseconds;
}