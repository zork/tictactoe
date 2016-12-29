////
// singleton.h
////

#include "base/macros.h"

template <typename T>
class Singleton {
 public:
  static T* Get() {
    static T g_instance;
    return &g_instance;
  }

 protected:
  Singleton() {}

 private:
  DISALLOW_COPY_AND_ASSIGN(Singleton<T>);
};
