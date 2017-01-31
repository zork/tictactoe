////
// time.cpp
////

#include "base/time.h"

#include "base/macros.h"

#include <time.h>

#if (OS_IOS || OS_OSX)
#include <mach/clock.h>
#include <mach/mach.h>
#include <mach/mach_time.h>
#endif

namespace {
#if (OS_WIN)
const int64_t kTimeTToMicrosecondsOffset = 11644473600000000I64;
#endif
}

////
// TimeInterval
////

// static
TimeInterval TimeInterval::FromDays(double days) {
  // 24 * 60 * 60 seconds per day.
  return TimeInterval(24 * 60 * 60 * days);
}

// static
TimeInterval TimeInterval::FromHours(double hours) {
  // 60 * 60 seconds per hour.
  return TimeInterval(60 * 60 * hours);
}

// static
TimeInterval TimeInterval::FromMinutes(double minutes) {
  // 60 seconds per minute.
  return TimeInterval(60 * minutes);
}

// static
TimeInterval TimeInterval::FromSeconds(double seconds) {
  // 1 seconds per second
  return TimeInterval(seconds);
}

// static
TimeInterval TimeInterval::FromMilliseconds(double milliseconds) {
  // 1000 milliseconds per second
  return TimeInterval(milliseconds / 1000.0);
}

// static
TimeInterval TimeInterval::FromMicroseconds(double microseconds) {
  // 1000000 microseconds per second
  return TimeInterval(microseconds / 1000000.0);
}

// static
TimeInterval TimeInterval::FromNanoseconds(double nanoseconds) {
  // 1000000000 nanoseconds per second
  return TimeInterval(nanoseconds / 1000000000.0);
}

TimeInterval::TimeInterval() : seconds_(0) {}

TimeInterval::~TimeInterval() {}

TimeInterval TimeInterval::operator-(const TimeInterval& other) const {
  return TimeInterval(seconds_ - other.seconds_);
}

const TimeInterval& TimeInterval::operator-=(const TimeInterval& other) {
  seconds_ -= other.seconds_;
  return *this;
}

// private:
TimeInterval::TimeInterval(double seconds) : seconds_(seconds) {}

////
// DateTime
////

// static
DateTime DateTime::Now() {
#if (OS_IOS || OS_OSX)
  struct timespec res;
  clock_serv_t cclock;
  mach_timespec_t mts;
  host_get_clock_service(mach_host_self(), CALENDAR_CLOCK, &cclock);
  clock_get_time(cclock, &mts);
  mach_port_deallocate(mach_task_self(), cclock);
  res.tv_sec = mts.tv_sec;
  res.tv_nsec = mts.tv_nsec;
  return DateTime(res.tv_sec + res.tv_nsec / 1.0e9);

#elif (OS_ANDROID || OS_CHROME || OS_LINUX)
  struct timespec res;
  clock_gettime(CLOCK_REALTIME, &res);
  return DateTime(res.tv_sec + res.tv_nsec / 1.0e9);

#elif (OS_WIN)
  FILETIME ft;
  ::GetSystemTimeAsFileTime(&ft);
  int64_t ms = bit_cast<int64_t, FILETIME>(ft) / 10;
  // Change epoc from 1601 to 1970.
  ms -= kTimeTToMicrosecondsOffset;
  return DateTime(ms / 1000000.0);
#endif
}

// static
DateTime DateTime::FromUnixTime(double unix_time) {
  return DateTime(unix_time);
}

DateTime::DateTime() : value_(0) {}

DateTime::~DateTime() {}

double DateTime::ToUnixTime() {
  return value_;
}

TimeInterval DateTime::operator-(const DateTime& other) const {
  InternalDateTimeValue difference = value_ - other.value_;
  return TimeInterval::FromSeconds(InternalToSeconds(difference));
}

DateTime DateTime::operator+(const TimeInterval& interval) const {
  return DateTime(value_ + SecondsToInternal(interval.Seconds()));
}

DateTime DateTime::operator-(const TimeInterval& interval) const {
  return DateTime(value_ - SecondsToInternal(interval.Seconds()));
}

const DateTime& DateTime::operator+=(const TimeInterval& interval) {
  value_ += SecondsToInternal(interval.Seconds());
  return *this;
}

const DateTime& DateTime::operator-=(const TimeInterval& interval) {
  value_ -= SecondsToInternal(interval.Seconds());
  return *this;
}

bool DateTime::operator==(const DateTime& other) const {
  return value_ == other.value_;
}

bool DateTime::operator!=(const DateTime& other) const {
  return value_ != other.value_;
}

bool DateTime::operator<(const DateTime& other) const {
  return value_ < other.value_;
}

bool DateTime::operator>(const DateTime& other) const {
  return value_ > other.value_;
}

bool DateTime::operator<=(const DateTime& other) const {
  return value_ <= other.value_;
}

bool DateTime::operator>=(const DateTime& other) const {
  return value_ >= other.value_;
}

// private:
DateTime::DateTime(InternalDateTimeValue value) : value_(value) {}

// static
double DateTime::InternalToSeconds(InternalDateTimeValue value) {
  return value;
}

// static
InternalDateTimeValue DateTime::SecondsToInternal(double value) {
  return value;
}

////
// Timestamp
////

// static
Timestamp Timestamp::Now() {
#if (OS_IOS || OS_OSX)
  mach_timebase_info_data_t info;
  mach_timebase_info(&info);

  // Convert to seconds
  return Timestamp(mach_absolute_time() * info.numer / (info.denom * 1.0e9));
#elif (OS_ANDROID || OS_CHROME || OS_LINUX)
  struct timespec res;
  clock_gettime(CLOCK_MONOTONIC, &res);
  return Timestamp(res.tv_sec + res.tv_nsec / 1.0e9);
#elif (OS_WIN)
  return Timestamp(GetTickCount64());
#endif
}

Timestamp::Timestamp() : value_(0) {}

Timestamp::~Timestamp() {}

TimeInterval Timestamp::operator-(const Timestamp& other) const {
  InternalTimestampValue difference = value_ - other.value_;
  return TimeInterval::FromSeconds(InternalToSeconds(difference));
}

Timestamp Timestamp::operator+(const TimeInterval& interval) const {
  return Timestamp(value_ + SecondsToInternal(interval.Seconds()));
}

Timestamp Timestamp::operator-(const TimeInterval& interval) const {
  return Timestamp(value_ - SecondsToInternal(interval.Seconds()));
}

const Timestamp& Timestamp::operator+=(const TimeInterval& interval) {
  value_ += SecondsToInternal(interval.Seconds());
  return *this;
}

const Timestamp& Timestamp::operator-=(const TimeInterval& interval) {
  value_ -= SecondsToInternal(interval.Seconds());
  return *this;
}

bool Timestamp::operator==(const Timestamp& other) const {
  return value_ == other.value_;
}

bool Timestamp::operator!=(const Timestamp& other) const {
  return value_ != other.value_;
}

bool Timestamp::operator<(const Timestamp& other) const {
  return value_ < other.value_;
}

bool Timestamp::operator>(const Timestamp& other) const {
  return value_ > other.value_;
}

bool Timestamp::operator<=(const Timestamp& other) const {
  return value_ <= other.value_;
}

bool Timestamp::operator>=(const Timestamp& other) const {
  return value_ >= other.value_;
}

// private:
Timestamp::Timestamp(InternalTimestampValue value) : value_(value) {}

// static
double Timestamp::InternalToSeconds(InternalTimestampValue value) {
#if (OS_ANDROID || OS_CHROME || OS_IOS || OS_OSX || OS_LINUX)
  return value;
#elif (OS_WIN)
  return value / 1000.0;
#endif
}

// static
InternalTimestampValue Timestamp::SecondsToInternal(double value) {
#if (OS_ANDROID || OS_CHROME || OS_IOS || OS_OSX || OS_LINUX)
  return value;
#elif (OS_WIN)
  return value * 1000.0;
#endif
}
