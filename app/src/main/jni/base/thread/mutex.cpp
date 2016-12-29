////
// mutex.cpp
////

#include "base/thread/mutex.h"

#include "base/logging.h"
#include "base/thread/thread_local.h"
#include "base/thread/thread_util.h"

#include <atomic>

#ifndef NDEBUG
namespace {
ThreadLocalInt t_thread_id;
std::atomic<int> g_next_thread_id(1);

int GetMutexThreadId() {
  int thread_id = t_thread_id.Get();
  if (!thread_id) {
    thread_id = g_next_thread_id++;
    t_thread_id.Set(thread_id);
  }
  return thread_id;
}
}
#endif

#if (DEBUG_MUTEX)

#include <map>
#include <set>
#include <vector>

namespace {
const int kMaxThreads = 256;
std::atomic<int> g_next_lock_id(0);
bool g_debug_locks_init = false;
pthread_mutex_t g_debug_mutex;
std::vector<int> g_locks[kMaxThreads];
std::set<std::pair<int, int>> g_invalid_locks;

std::vector<int>& GetThreadLocks() {
  int thread_id = GetMutexThreadId();
  CHECK_LE(thread_id, kMaxThreads);
  return g_locks[thread_id - 1];
}

void MaybeInitMutexDebug() {
  if (!g_debug_locks_init) {
    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
    int error = pthread_mutex_init(&g_debug_mutex, &attr);
    if (error)
      LOG(ERROR) << "pthread_mutex_init failed: " << error;
    CHECK(!error);
    g_debug_locks_init = true;
  }
}

void LockDebugInfo() {
  int error = pthread_mutex_lock(&g_debug_mutex);
  if (error)
    LOG(ERROR) << "pthread_mutex_lock failed: " << error;
  CHECK(!error);
}

void UnlockDebugInfo() {
  int error = pthread_mutex_unlock(&g_debug_mutex);
  if (error)
    LOG(ERROR) << "pthread_mutex_unlock failed: " << error;
  CHECK(!error);
}

void CheckValidLockOrder(int first, int second) {
  if (g_invalid_locks.find(std::make_pair(first, second)) !=
      g_invalid_locks.end()) {
    LOG(ERROR) << "Invalid lock order: " << first << ", " << second;
    NOTREACHED();
  }
}

void AddInvalidLockOrder(int first, int second) {
  g_invalid_locks.insert(std::make_pair(first, second));
}

void PushLock(int id) {
  LockDebugInfo();
  std::vector<int>& locks = GetThreadLocks();
  if (id != thread::Unknown) {
    for (const auto& lock_id : locks) {
      CheckValidLockOrder(lock_id, id);
      AddInvalidLockOrder(id, lock_id);
    }
  }
  locks.push_back(id);
  UnlockDebugInfo();
}

void PopLock(int id) {
  LockDebugInfo();
  std::vector<int>& locks = GetThreadLocks();
  CHECK_EQ(locks.back(), id);
  locks.pop_back();
  UnlockDebugInfo();
}
}
#endif

////
// Mutex
////
Mutex::Mutex()
#ifndef NDEBUG
    : locking_thread_(0)
#endif
{
#if (DEBUG_MUTEX)
  MaybeInitMutexDebug();
  LockDebugInfo();
  lock_id_ = g_next_lock_id++;
  UnlockDebugInfo();
#endif
#if OS_POSIX
  pthread_mutexattr_t attr;
  pthread_mutexattr_init(&attr);
#ifdef NDEBUG
  pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_NORMAL);
#else
  pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_ERRORCHECK);
#endif
  int error = pthread_mutex_init(&mutex_, &attr);
  if (error)
    LOG(FATAL) << "pthread_mutex_init failed: " << error;
#elif OS_WIN
  ::InitializeCriticalSectionAndSpinCount(&mutex_, 2000);
#endif
}

Mutex::~Mutex() {
  DCHECK(!locking_thread_);
#if OS_POSIX
  int error = pthread_mutex_destroy(&mutex_);
  if (error)
    LOG(FATAL) << "pthread_mutex_destroy failed: " << error;
#elif OS_WIN
  ::DeleteCriticalSection(&mutex_);
#endif
}

void Mutex::Lock() {
#if OS_POSIX
  int error = pthread_mutex_lock(&mutex_);
  DCHECK(!error);
#elif OS_WIN
  ::EnterCriticalSection(&mutex_);
#endif

#if (DEBUG_MUTEX)
  PushLock(lock_id_);
#endif

#ifndef NDEBUG
  locking_thread_ = GetMutexThreadId();
#endif
}

void Mutex::Unlock() {
  DCHECK(locking_thread_);
#ifndef NDEBUG
  locking_thread_ = 0;
#endif
#if (DEBUG_MUTEX)
  PopLock(lock_id_);
#endif
#if OS_POSIX
  int error = pthread_mutex_unlock(&mutex_);
  DCHECK(!error);
#elif OS_WIN
  ::LeaveCriticalSection(&mutex_);
#endif
}

#ifndef NDEBUG
bool Mutex::IsLocked() {
  return locking_thread_ == GetMutexThreadId();
}
#endif

////
// AutoLock
////
AutoLock::AutoLock(Mutex* mutex) : mutex_(mutex) {
  mutex_->Lock();
}

AutoLock::~AutoLock() {
  mutex_->Unlock();
}
