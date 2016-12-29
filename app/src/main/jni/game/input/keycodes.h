////
// keycodes.h
////

#pragma once

#include "base/platform.h"

#if defined(OS_ANDROID)
#include "game/input/keycodes_android.h"
#elif defined(OS_CHROME)
#include "game/input/keycodes_chrome.h"
#elif defined(OS_OSX)
#include "game/input/keycodes_osx.h"
#elif defined(OS_WIN)
#include "game/input/keycodes_win.h"
#elif defined(OS_POSIX)
#include "game/input/keycodes_posix.h"
#endif
