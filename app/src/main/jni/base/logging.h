////
// logging.h
////

#pragma once

#include <sstream>

#include "base/macros.h"

#ifdef NDEBUG
#ifndef _WIN32
#include <unistd.h>
#endif
#else
#include <assert.h>
#endif

#ifdef NDEBUG
#define DCHECK(x)
#if __ANDROID__
#define CHECK(x) \
  if (!(x))      \
    _exit(1);
#else
#define CHECK(x) \
  if (!(x))      \
    exit(1);
#endif
#else
#define DCHECK(x) assert(x)
#define CHECK(x) assert(x)
#endif

#ifdef NDEBUG
#define DCHECK_OP(name, op, val1, val2)
#else
#define DCHECK_OP(name, op, val1, val2)                                        \
  if (std::string* _result =                                                   \
          logging::Check##name##Impl((val1), (val2), #val1 " " #op " " #val2)) \
  logging::LogMessage(__FILE__, __LINE__, ::logging::LOG_DCHECK, _result)      \
      .stream()
#endif

#define DCHECK_EQ(val1, val2) DCHECK_OP(EQ, ==, val1, val2)
#define DCHECK_NE(val1, val2) DCHECK_OP(NE, !=, val1, val2)
#define DCHECK_LE(val1, val2) DCHECK_OP(LE, <=, val1, val2)
#define DCHECK_LT(val1, val2) DCHECK_OP(LT, <, val1, val2)
#define DCHECK_GE(val1, val2) DCHECK_OP(GE, >=, val1, val2)
#define DCHECK_GT(val1, val2) DCHECK_OP(GT, >, val1, val2)

#ifdef NDEBUG
#define CHECK_OP(name, op, val1, val2) CHECK((val1)op(val2))
#else
#define CHECK_OP(name, op, val1, val2)                                         \
  if (std::string* _result =                                                   \
          logging::Check##name##Impl((val1), (val2), #val1 " " #op " " #val2)) \
  logging::LogMessage(__FILE__, __LINE__, ::logging::LOG_CHECK, _result)       \
      .stream()
#endif

#define CHECK_EQ(val1, val2) CHECK_OP(EQ, ==, val1, val2)
#define CHECK_NE(val1, val2) CHECK_OP(NE, !=, val1, val2)
#define CHECK_LE(val1, val2) CHECK_OP(LE, <=, val1, val2)
#define CHECK_LT(val1, val2) CHECK_OP(LT, <, val1, val2)
#define CHECK_GE(val1, val2) CHECK_OP(GE, >=, val1, val2)
#define CHECK_GT(val1, val2) CHECK_OP(GT, >, val1, val2)

#define NOTREACHED() DCHECK(false)

// Logging

namespace logging {

enum Severity {
  LOG_INFO,
  LOG_WARNING,
  LOG_ERROR,
  LOG_DCHECK,
  LOG_CHECK,
  LOG_FATAL,
};

class LogMessage {
 public:
  LogMessage(const char* file, const int line, Severity severity);
  LogMessage(const char* file,
             const int line,
             Severity severity,
             std::string* in_message);
  ~LogMessage();

  std::ostream& stream() { return stream_; }

 private:
  const char* file_;
  const int line_;
  const Severity severity_;

  std::ostringstream stream_;

  DISALLOW_COPY_AND_ASSIGN(LogMessage);
};

class LogMessageVoidify {
 public:
  LogMessageVoidify() {}
  void operator&(std::ostream&) {}
};

template <class t1, class t2>
std::string* MakeCheckOpString(const t1& v1, const t2& v2, const char* names) {
  std::ostringstream ss;
  ss << names << " (" << v1 << " vs. " << v2 << ")";
  std::string* msg = new std::string(ss.str());
  return msg;
}

extern template std::string* MakeCheckOpString<int, int>(const int&,
                                                         const int&,
                                                         const char* names);
extern template std::string* MakeCheckOpString<unsigned long, unsigned long>(
    const unsigned long&,
    const unsigned long&,
    const char* names);
extern template std::string* MakeCheckOpString<unsigned long, unsigned int>(
    const unsigned long&,
    const unsigned int&,
    const char* names);
extern template std::string* MakeCheckOpString<unsigned int, unsigned long>(
    const unsigned int&,
    const unsigned long&,
    const char* names);
extern template std::string* MakeCheckOpString<std::string, std::string>(
    const std::string&,
    const std::string&,
    const char* name);

#define DEFINE_CHECK_OP_IMPL(name, op)                                       \
  template <class t1, class t2>                                              \
  inline std::string* Check##name##Impl(const t1& v1, const t2& v2,          \
                                        const char* names) {                 \
    if (v1 op v2)                                                            \
      return NULL;                                                           \
    else                                                                     \
      return MakeCheckOpString(v1, v2, names);                               \
  }                                                                          \
  inline std::string* Check##name##Impl(int v1, int v2, const char* names) { \
    if (v1 op v2)                                                            \
      return NULL;                                                           \
    else                                                                     \
      return MakeCheckOpString(v1, v2, names);                               \
  }
DEFINE_CHECK_OP_IMPL(EQ, ==)
DEFINE_CHECK_OP_IMPL(NE, !=)
DEFINE_CHECK_OP_IMPL(LE, <=)
DEFINE_CHECK_OP_IMPL(LT, <)
DEFINE_CHECK_OP_IMPL(GE, >=)
DEFINE_CHECK_OP_IMPL(GT, >)
#undef DEFINE_CHECK_OP_IMPL

}  // namespace logging

#define LOG_MESSAGE_INFO \
  ::logging::LogMessage(__FILE__, __LINE__, ::logging::LOG_INFO)
#define LOG_MESSAGE_WARNING \
  ::logging::LogMessage(__FILE__, __LINE__, ::logging::LOG_WARNING)
#define LOG_MESSAGE_ERROR \
  ::logging::LogMessage(__FILE__, __LINE__, ::logging::LOG_ERROR)
#define LOG_MESSAGE_FATAL \
  ::logging::LogMessage(__FILE__, __LINE__, ::logging::LOG_FATAL)

// ERROR is defined as 0 on Win32.
#ifdef _WIN32
#define LOG_MESSAGE_0 LOG_MESSAGE_ERROR
#endif

#define LOG_STREAM(severity) (LOG_MESSAGE_##severity.stream())

#define LAZY_STREAM(stream, condition) \
  !(condition) ? (void)0 : ::logging::LogMessageVoidify() & (stream)

#ifdef NDEBUG
#define DLOG(severity) LAZY_STREAM(LOG_STREAM(severity), false)
#else
#define DLOG(severity) LAZY_STREAM(LOG_STREAM(severity), true)
#endif

#define LOG(severity) LAZY_STREAM(LOG_STREAM(severity), true)
