////
// vector.cpp
////

#include "base/math/vector.h"

#include "base/logging.h"
#include "base/math/math.h"

#include <math.h>
#include <iomanip>
#include <iostream>

bool Vector::operator==(const Vector& other) const {
  return math::FloatEqual(x(), other.x()) && math::FloatEqual(y(), other.y()) &&
         math::FloatEqual(z(), other.z());
}

bool Vector::operator!=(const Vector& other) const {
  return !math::FloatEqual(x(), other.x()) ||
         !math::FloatEqual(y(), other.y()) || !math::FloatEqual(z(), other.z());
}

Vector Vector::Cross(const Vector& other) const {
  return Vector(y() * other.z() - z() * other.y(),
                z() * other.x() - x() * other.z(),
                x() * other.y() - y() * other.x());
}

float Vector::Dot(const Vector& other) const {
  return x() * other.x() + y() * other.y() + z() * other.z();
}

Vector& Vector::Normalize() {
  float sq_length = x() * x() + y() * y() + z() * z();
  if (sq_length) {
    float length = sqrt(sq_length);
    value_[0] /= length;
    value_[1] /= length;
    value_[2] /= length;
    DCHECK(math::FloatEqual(Dot(*this), 1.0));
  }
  return *this;
}

bool Vector::IsNormalized() const {
  return math::FloatEqual(Dot(*this), 1);
}

bool Vector4::operator==(const Vector4& other) const {
  return math::FloatEqual(x(), other.x()) && math::FloatEqual(y(), other.y()) &&
         math::FloatEqual(z(), other.z()) && math::FloatEqual(w(), other.w());
}

bool Vector4::operator!=(const Vector4& other) const {
  return !math::FloatEqual(x(), other.x()) ||
         !math::FloatEqual(y(), other.y()) ||
         !math::FloatEqual(z(), other.z()) || !math::FloatEqual(w(), other.w());
}

std::ostream& operator<<(std::ostream& out, const Vector& vector) {
  return out << std::fixed << std::setprecision(2) << "(" << vector.x() << ", "
             << vector.y() << ", " << vector.z() << ")";
}

std::ostream& operator<<(std::ostream& out, const Vector4& vector) {
  return out << std::fixed << std::setprecision(2) << "(" << vector.x() << ", "
             << vector.y() << ", " << vector.z() << ", " << vector.w() << ")";
}
