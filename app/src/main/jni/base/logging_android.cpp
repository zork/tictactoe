////
// logging_android.cpp
////

#include <android/log.h>

namespace logging {

void Info(const char* format, ...) {
  va_list args;
  va_start(args, format);
  __android_log_vprint(ANDROID_LOG_INFO, "game", format, args);
  va_end(args);
}

void Warn(const char* format, ...) {
  va_list args;
  va_start(args, format);
  __android_log_vprint(ANDROID_LOG_WARN, "game", format, args);
  va_end(args);
}

void Err(const char* format, ...) {
  va_list args;
  va_start(args, format);
  __android_log_vprint(ANDROID_LOG_ERROR, "game", format, args);
  va_end(args);
}

}  // logging
