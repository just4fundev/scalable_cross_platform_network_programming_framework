#pragma once

#define IGNORE_EXCEPTIONS(ACTION) try { ACTION } catch (...) { }
#define CRITICAL_SECTION(MUTEX, ACTION) MUTEX.lock(); ACTION MUTEX.unlock();