#include "pch.h"

#include "Memory/Include/Private/NativeMemoryOperations.h"

#include <memory>

#ifdef __cplusplus
extern "C" {
#endif

    void* NativeMemoryAllocation(size_t size)
    {
        return std::realloc(nullptr, size);
    }

    void NativeMemoryFree(void* ptr)
    {
        std::free(ptr);
    }

#ifdef __cplusplus
}
#endif