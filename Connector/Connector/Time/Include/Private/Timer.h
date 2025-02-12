#pragma once

#include <cstdint>

#include "Connector.h"

#ifdef __cplusplus
extern "C" {
#endif
    Connector_API void InitializeApplicationStartTime();
    Connector_API uint32_t GetTimeInMilliseconds();
#ifdef __cplusplus
}
#endif