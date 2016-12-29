////
// thread_local.cpp
////

#include "base/thread/thread_local.h"

#include "base/logging.h"

namespace thread {

void AllocThreadLocalSlot(ThreadLocalSlot* slot) {
#if OS_POSIX
  int error = pthread_key_create(slot, nullptr);
  CHECK_EQ(error, 0);
#elif OS_WIN
  *slot = TlsAlloc();
  CHECK_NE(*slot, TLS_OUT_OF_INDEXES);
#endif
}

void FreeThreadLocalSlot(ThreadLocalSlot slot) {
#if OS_POSIX
  int error = pthread_key_delete(slot);
  CHECK_EQ(error, 0);
#elif OS_WIN
  if (!TlsFree(slot)) {
    LOG(FATAL) << "Failed to free TLS slot.";
  }
#endif
}

void* GetThreadLocalSlot(ThreadLocalSlot slot) {
#if OS_POSIX
  return pthread_getspecific(slot);
#elif OS_WIN
  return TlsGetValue(slot);
#endif
}

void SetThreadLocalSlot(ThreadLocalSlot slot, void* value) {
#if OS_POSIX
  int error = pthread_setspecific(slot, value);
  DCHECK_EQ(error, 0);
#elif OS_WIN
  if (!TlsSetValue(slot, value)) {
    LOG(FATAL) << "Failed to set TLS value.";
  }
#endif
}

}  // namespace thread
