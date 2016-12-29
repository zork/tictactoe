////
// platform.h
////

#pragma once

#if defined(__APPLE__)
#include "TargetConditionals.h"
#endif

#if TARGET_OS_IPHONE
#define OS_IOS 1
#elif TARGET_OS_MAC
#define OS_OSX 1
#elif __CHROME__
#define OS_CHROME 1
#elif __ANDROID__
#define OS_ANDROID 1
#elif defined(_WIN32)
#define OS_WIN 1
#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#undef Yield
#undef GetCommandLine
#undef LoadImage
#elif defined(__linux__)
#define OS_LINUX 1
#else
#error "Define your platform"
#endif

#if OS_OSX || OS_CHROME || OS_LINUX || OS_ANDROID || OS_IOS
#define OS_POSIX 1
#endif
