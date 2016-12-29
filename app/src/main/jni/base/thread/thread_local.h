////
// thread_local.h
////

#include "base/macros.h"
#include "base/platform.h"

#if OS_POSIX
#include <pthread.h>
#endif

namespace thread {
#if OS_POSIX
typedef pthread_key_t ThreadLocalSlot;
#elif OS_WIN
typedef DWORD ThreadLocalSlot;
#endif
void AllocThreadLocalSlot(ThreadLocalSlot* slot);
void FreeThreadLocalSlot(ThreadLocalSlot slot);
void* GetThreadLocalSlot(ThreadLocalSlot slot);
void SetThreadLocalSlot(ThreadLocalSlot slot, void* value);
}  // namespace thread

class ThreadLocalInt {
 public:
  ThreadLocalInt() { thread::AllocThreadLocalSlot(&slot_); }
  ~ThreadLocalInt() { thread::FreeThreadLocalSlot(slot_); }

  long Get() {
    return reinterpret_cast<long>(thread::GetThreadLocalSlot(slot_));
  }

  void Set(long value) {
    thread::SetThreadLocalSlot(slot_, reinterpret_cast<void*>(value));
  }

 private:
  thread::ThreadLocalSlot slot_;

  DISALLOW_COPY_AND_ASSIGN(ThreadLocalInt);
};

template <typename T>
class ThreadLocalPtr {
 public:
  ThreadLocalPtr() { thread::AllocThreadLocalSlot(&slot_); }
  ~ThreadLocalPtr() { thread::FreeThreadLocalSlot(slot_); }

  T* Get() { return static_cast<T*>(thread::GetThreadLocalSlot(slot_)); }

  void Set(T* value) {
    thread::SetThreadLocalSlot(slot_, static_cast<void*>(value));
  }

 private:
  thread::ThreadLocalSlot slot_;

  DISALLOW_COPY_AND_ASSIGN(ThreadLocalPtr);
};
