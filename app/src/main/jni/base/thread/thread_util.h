////
// thread_util.h
////

#pragma once

#include "base/logging.h"
#include "base/platform.h"

#if OS_POSIX
#include <pthread.h>

#define CHECK_THREAD(thread_id) \
  DCHECK(pthread_equal(pthread_self(), thread::g_thread_ids[thread_id]))
#elif OS_WIN

#define CHECK_THREAD(thread_id) \
  DCHECK_EQ(::GetCurrentThreadId(), thread::g_thread_ids[thread_id])
#endif

namespace thread {

#if OS_POSIX
typedef pthread_t ThreadId;
#elif OS_WIN
typedef DWORD ThreadId;
#endif

const int kMaxThreads = 32;

enum ID {
  Unknown = -1,
  Ui = 0,
  Game = 1,
  Render = 2,
  Background = 3,
  kNumNamedThreads,
};

extern ThreadId g_thread_ids[];

void InitThread(ID thread_id);
int AllocThreadId();
void ReleaseThreadId(int thread_id);

int CurrentThread();
bool CurrentlyOn(int thread_id);

}  // namespace thread
