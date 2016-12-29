////
// time.h
////

#pragma once

#include "base/basic_types.h"
#include "base/platform.h"

#if (OS_POSIX)
typedef double InternalDateTimeValue;
typedef double InternalTimestampValue;
#elif (OS_WIN)
typedef double InternalDateTimeValue;
typedef uint64_t InternalTimestampValue;
#endif

class TimeInterval {
 public:
  static TimeInterval FromDays(double days);
  static TimeInterval FromHours(double hours);
  static TimeInterval FromMinutes(double minutes);
  static TimeInterval FromSeconds(double seconds);
  static TimeInterval FromMilliseconds(double milliseconds);

  TimeInterval();
  ~TimeInterval();

  // 60 ticks per second.
  double Seconds() const { return seconds_; }
  double Milliseconds() const { return seconds_ * 1000; }

  TimeInterval operator-(const TimeInterval& other) const;
  const TimeInterval& operator-=(const TimeInterval& other);
  bool operator==(const TimeInterval& other) const {
    return seconds_ == other.seconds_;
  }
  bool operator>(const TimeInterval& other) const {
    return seconds_ > other.seconds_;
  }
  bool operator>=(const TimeInterval& other) const {
    return seconds_ >= other.seconds_;
  }
  bool operator<(const TimeInterval& other) const {
    return seconds_ < other.seconds_;
  }
  bool operator<=(const TimeInterval& other) const {
    return seconds_ <= other.seconds_;
  }

 private:
  explicit TimeInterval(double seconds);

  double seconds_;
};

class DateTime {
 public:
  static DateTime Now();
  static DateTime FromUnixTime(double unix_time);

  DateTime();
  ~DateTime();

  double ToUnixTime();

  TimeInterval operator-(const DateTime& other) const;

  DateTime operator+(const TimeInterval& interval) const;
  DateTime operator-(const TimeInterval& interval) const;
  const DateTime& operator+=(const TimeInterval& interval);
  const DateTime& operator-=(const TimeInterval& interval);

  bool operator==(const DateTime& other) const;
  bool operator!=(const DateTime& other) const;
  bool operator<(const DateTime& other) const;
  bool operator>(const DateTime& other) const;
  bool operator<=(const DateTime& other) const;
  bool operator>=(const DateTime& other) const;

 private:
  explicit DateTime(InternalDateTimeValue value);

  static double InternalToSeconds(InternalDateTimeValue value);
  static InternalDateTimeValue SecondsToInternal(double value);

  InternalDateTimeValue value_;
};

class Timestamp {
 public:
  static Timestamp Now();
  Timestamp();
  ~Timestamp();

  TimeInterval operator-(const Timestamp& other) const;

  Timestamp operator+(const TimeInterval& interval) const;
  Timestamp operator-(const TimeInterval& interval) const;
  const Timestamp& operator+=(const TimeInterval& interval);
  const Timestamp& operator-=(const TimeInterval& interval);

  bool operator==(const Timestamp& other) const;
  bool operator!=(const Timestamp& other) const;
  bool operator<(const Timestamp& other) const;
  bool operator>(const Timestamp& other) const;
  bool operator<=(const Timestamp& other) const;
  bool operator>=(const Timestamp& other) const;

 private:
  explicit Timestamp(InternalTimestampValue value);

  static double InternalToSeconds(InternalTimestampValue value);
  static InternalTimestampValue SecondsToInternal(double value);

  InternalTimestampValue value_;
};
