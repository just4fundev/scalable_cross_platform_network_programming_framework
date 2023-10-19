// Copyright Cristian Pagán Díaz. All Rights Reserved.

#pragma once

#include "Connector.h"

#ifdef __cplusplus
extern "C" {
#endif
    Connector_API void* NativeMemoryAllocation(size_t size);
    Connector_API void NativeMemoryFree(void* ptr);
#ifdef __cplusplus
}
#endif