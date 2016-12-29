////
// ptr_util.h
////

#pragma once

#include <memory>
#include <stdlib.h>
#include <utility>

template <typename T>
std::unique_ptr<T> WrapUnique(T* ptr) {
  return std::unique_ptr<T>(ptr);
}

namespace base {
struct FreeDeleter {
  inline void operator()(void* ptr) const {
    free(ptr);
  }
};
} // namespace base
