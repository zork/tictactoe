////
// mutex.h
////

#pragma once

#include "base/macros.h"
#include "base/platform.h"

#if OS_POSIX
#include <pthread.h>
#endif

// NOTE: DEBUG_MUTEX only works with pthreads.
// #define DEBUG_MUTEX 1

class Mutex {
 public:
  Mutex();
  ~Mutex();

  void Lock();
  void Unlock();

#ifndef NDEBUG
  bool IsLocked();
#endif

 private:
#if OS_POSIX
  typedef pthread_mutex_t MutexHandle;
#elif OS_WIN
  typedef CRITICAL_SECTION MutexHandle;
#endif

  MutexHandle mutex_;
#ifndef NDEBUG
  int locking_thread_;
#endif

#if (DEBUG_MUTEX)
  int lock_id_;
#endif
  DISALLOW_COPY_AND_ASSIGN(Mutex);
};

class AutoLock {
 public:
  AutoLock(Mutex* mutex);
  ~AutoLock();

 private:
  Mutex* mutex_;

  DISALLOW_COPY_AND_ASSIGN(AutoLock);
};
