////
// point.cpp
////

#include "base/math/point.h"

#include "base/math/math.h"

#include <math.h>

namespace math {

// static
float Point::Distance(const Point& lhs, const Point& rhs) {
  float x_diff = rhs.x() - lhs.x();
  float y_diff = rhs.y() - lhs.y();
  return sqrt(x_diff * x_diff + y_diff * y_diff);
}

bool Point::operator==(const Point& other) const {
  return math::FloatEqual(x(), other.x()) && math::FloatEqual(y(), other.y());
}

bool Point::operator!=(const Point& other) const {
  return !math::FloatEqual(x(), other.x()) || !math::FloatEqual(y(), other.y());
}

Point Point::operator+(const Point& other) const {
  return Point(x() + other.x(), y() + other.y());
}

Point Point::operator-(const Point& other) const {
  return Point(x() - other.x(), y() - other.y());
}

Point Point::operator*(float scalar) const {
  return Point(x() * scalar, y() * scalar);
}

Point Point::operator/(float scalar) const {
  return Point(x() / scalar, y() / scalar);
}

const Point& Point::operator+=(const Point& other) {
  x_ += other.x();
  y_ += other.y();
  return *this;
}

const Point& Point::operator-=(const Point& other) {
  x_ -= other.x();
  y_ -= other.y();
  return *this;
}

const Point& Point::operator*=(float scalar) {
  x_ *= scalar;
  y_ *= scalar;
  return *this;
}

const Point& Point::operator/=(float scalar) {
  x_ /= scalar;
  y_ /= scalar;
  return *this;
}

}  // namespace math
