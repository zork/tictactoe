////
// logging.cpp
////

#include "base/logging.h"

#include "base/platform.h"

#include <exception>
#include <memory>

namespace logging {

void Info(const char* format, ...);
void Warn(const char* format, ...);
void Err(const char* format, ...);

namespace {
#if OS_WIN
const char kPathChar = '\\';
#else
const char kPathChar = '/';
#endif

#if OS_WIN
#define ABORT() exit(1)
#else
#define ABORT() std::terminate()
#endif
}

LogMessage::LogMessage(const char* file, const int line, Severity severity)
    : file_(file), line_(line), severity_(severity) {}

LogMessage::LogMessage(const char* file,
                       const int line,
                       Severity severity,
                       std::string* in_message)
    : file_(file), line_(line), severity_(severity) {
  std::unique_ptr<std::string> message(in_message);
  stream_ << *message;
}

LogMessage::~LogMessage() {
  const char* filename = strrchr(file_, kPathChar);
  if (filename)
    filename = filename + 1;
  else
    filename = file_;
  switch (severity_) {
    case LOG_INFO:
      logging::Info("%s(%d): %s", filename, line_, stream_.str().c_str());
      break;

    case LOG_WARNING:
      logging::Warn("%s(%d): %s", filename, line_, stream_.str().c_str());
      break;

    case LOG_ERROR:
      logging::Err("%s(%d): %s", filename, line_, stream_.str().c_str());
      break;

    case LOG_DCHECK:
      logging::Err("DCHECK failed %s(%d): %s", filename, line_,
                   stream_.str().c_str());
      DCHECK(false);
      break;

    case LOG_CHECK:
      logging::Err("CHECK failed %s(%d): %s", filename, line_,
                   stream_.str().c_str());
      ABORT();
      break;

    case LOG_FATAL:
      logging::Err("%s(%d): %s", filename, line_, stream_.str().c_str());
      ABORT();
      break;
  }
}

template std::string* MakeCheckOpString<int, int>(const int&,
                                                  const int&,
                                                  const char* names);
template std::string* MakeCheckOpString<unsigned long, unsigned long>(
    const unsigned long&,
    const unsigned long&,
    const char* names);
template std::string* MakeCheckOpString<unsigned long, unsigned int>(
    const unsigned long&,
    const unsigned int&,
    const char* names);
template std::string* MakeCheckOpString<unsigned int, unsigned long>(
    const unsigned int&,
    const unsigned long&,
    const char* names);
template std::string* MakeCheckOpString<std::string, std::string>(
    const std::string&,
    const std::string&,
    const char* name);

}  // namespace logging
