#include "pch.h"

#include "Time/Include/Private/Timer.h"

#include <chrono>

std::chrono::steady_clock::time_point ApplicationStartTime;

#ifdef __cplusplus
extern "C" {
#endif

    void InitializeApplicationStartTime()
    {
        ApplicationStartTime = std::chrono::steady_clock::now();
    }

    uint32_t GetTimeInMilliseconds()
    {
        return uint32_t(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - ApplicationStartTime).count());
    }

#ifdef __cplusplus
}
#endif