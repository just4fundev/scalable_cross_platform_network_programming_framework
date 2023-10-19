// Copyright Cristian Pagán Díaz. All Rights Reserved.

#pragma once

#if defined(_WIN64) || defined(__WIN32__) || defined(WIN32) || defined(_WIN32)
#	define WINDOWS_PLATTFORM
#elif defined(__APPLE_CC__)
#	define APPLE_PLATFORM
#elif defined(__INTEL_COMPILER)
#	define INTEL_PLATFORM
#else
#	define UNIX_PLATFORM
#endif

#ifdef WINDOWS_PLATTFORM
#	define WINDOWS_ACTION(ACTION) ACTION
#else
#	define WINDOWS_ACTION(ACTION)
#endif

#ifdef APPLE_PLATFORM
#	define APPLE_ACTION(ACTION) ACTION
#else
#	define APPLE_ACTION(ACTION)
#endif

#ifdef INTEL_PLATFORM
#	define INTEL_ACTION(ACTION) ACTION
#else
#	define INTEL_ACTION(ACTION)
#endif

#ifdef UNIX_PLATFORM
#	define UNIX_ACTION(ACTION) ACTION
#else
#	define UNIX_ACTION(ACTION)
#endif

#ifndef WINDOWS_PLATTFORM
#	define NO_WINDOWS_ACTION(ACTION) ACTION
#else
#	define NO_WINDOWS_ACTION(ACTION)
#endif

#ifndef APPLE_PLATFORM
#	define NO_APPLE_ACTION(ACTION) ACTION
#else
#	define NO_APPLE_ACTION(ACTION)
#endif

#ifndef INTEL_PLATFORM
#	define NO_INTEL_ACTION(ACTION) ACTION
#else
#	define NO_INTEL_ACTION(ACTION)
#endif

#ifndef UNIX_PLATFORM
#	define NO_UNIX_ACTION(ACTION) ACTION
#else
#	define NO_UNIX_ACTION(ACTION)
#endif