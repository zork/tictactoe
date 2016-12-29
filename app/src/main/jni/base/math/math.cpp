////
// math.cpp
////

#include "base/math/math.h"

namespace math {

int NextPower2(int x) {
  x--;
  x |= x >> 1;
  x |= x >> 2;
  x |= x >> 4;
  x |= x >> 8;
  x |= x >> 16;
  x++;
  return x;
}

}  // namespace math
