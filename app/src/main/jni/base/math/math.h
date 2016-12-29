////
// math.h
////

#pragma once

#include <math.h>

#define TO_RADIANS(x) (x * math::Pi / 180.0)

#define TO_DEGREES(x) (x * 180.0 / math::Pi)

namespace math {

const double Pi = 3.14159265358979323846264338327950288;
const float kStandardMaxDiff = 0.0005f;

int NextPower2(int x);
inline bool FloatEqual(float a, float b, float max_diff = kStandardMaxDiff) {
  return fabs(b - a) < max_diff;
}

inline bool FloatLessThan(float a, float b, float max_diff = kStandardMaxDiff) {
  return a < b && !FloatEqual(a, b, max_diff);
}

template <typename T>
T Clamp(T value, T min, T max) {
  if (value < min)
    return min;
  if (value > max)
    return max;
  return value;
}

}  // namespace math
