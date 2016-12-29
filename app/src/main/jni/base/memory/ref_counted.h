////
// ref_counted.h
////

#pragma once

#include "base/logging.h"

namespace base {

template <typename T>
class RefCounted {
 public:
  RefCounted() : ref_count_(0) {}
  ~RefCounted() {}

  void AddRef() { ++ref_count_; }

  void Release() {
    DCHECK_GT(ref_count_, 0);
    if (!--ref_count_) {
      delete this;
    }
  }

 private:
  int ref_count_;
};

}  // namespace base

template <typename T>
class scoped_refptr {
 public:
  scoped_refptr() : ptr_(nullptr) {}
  scoped_refptr(T* ptr) : ptr_(ptr) {
    if (ptr_)
      ptr_->AddRef();
  }

  ~scoped_refptr() {
    if (ptr_)
      ptr_->Release();
  }

  T* get() const { return ptr_; }

  T& operator*() const {
    DCHECK(ptr_);
    return *ptr_;
  }

  T* operator->() const {
    DCHECK(ptr_);
    return ptr_;
  }

 private:
  typedef T* scoped_refptr::*Testable;

 public:
  operator Testable() const { return ptr_ ? &scoped_refptr::ptr_ : nullptr; }

  template <typename U>
  bool operator==(const scoped_refptr<U>& other) const {
    return ptr_ == other.ptr_;
  }

  template <typename U>
  bool operator!=(const scoped_refptr<U>& other) const {
    return ptr_ != other.ptr_;
  }

 private:
  template <typename U>
  friend class scoped_refptr;

  T* ptr_;
};
