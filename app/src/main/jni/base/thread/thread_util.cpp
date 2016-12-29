////
// thread_util.cpp
////

#include "base/thread/thread_util.h"

#include "base/thread/mutex.h"

namespace thread {

namespace {
Mutex g_thread_id_lock;
bool g_initialized = false;
bool g_used_threads[kMaxThreads];

void EnsureInitialized() {
  if (g_initialized)
    return;
  memset(g_used_threads, false, sizeof(g_used_threads));
  g_initialized = true;
}

ThreadId GetPlatformThreadId() {
#if OS_POSIX
  return pthread_self();
#elif OS_WIN
  return ::GetCurrentThreadId();
#endif
}
}

ThreadId g_thread_ids[kMaxThreads];

void InitThread(ID thread_id) {
  g_thread_ids[thread_id] = GetPlatformThreadId();
}

int AllocThreadId() {
  AutoLock lock(&g_thread_id_lock);
  EnsureInitialized();
  int thread_id = Unknown;
  for (int i = kNumNamedThreads; i < kMaxThreads; ++i) {
    if (!g_used_threads[i]) {
      thread_id = i;
      g_used_threads[i] = true;
      break;
    }
  }
  CHECK_LT(thread_id, kMaxThreads);
  return thread_id;
}

void ReleaseThreadId(int thread_id) {
  DCHECK(g_used_threads[thread_id]);
  AutoLock lock(&g_thread_id_lock);
  g_used_threads[thread_id] = false;
}

int CurrentThread() {
  for (int i = 0; i < kMaxThreads; ++i) {
    if (CurrentlyOn(i))
      return i;
  }

  NOTREACHED();
  LOG(ERROR) << "Unknown thread id";
  return Unknown;
}

bool CurrentlyOn(int thread_id) {
  DCHECK_LT(thread_id, kMaxThreads);
#if OS_POSIX
  return pthread_equal(pthread_self(), g_thread_ids[thread_id]);
#elif OS_WIN
  return GetPlatformThreadId() == g_thread_ids[thread_id];
#endif
}

}  // namespace thread
