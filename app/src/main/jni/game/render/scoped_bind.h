////
// scoped_bind.h
////

#pragma once

template <typename T>
class ScopedBind {
 public:
  ScopedBind(T* t) : t_(t) { t_->Bind(); }
  ~ScopedBind() { t_->Unbind(); }

 private:
  T* t_;

  DISALLOW_COPY_AND_ASSIGN(ScopedBind<T>);
};
