////
// quaternion.h
////

#pragma once

#include "base/math/coordinate_system.h"

#include <iosfwd>

class Matrix;
class Vector;

class Quaternion {
 public:
  static Quaternion FromAngleAxis(const Vector& v);

  Quaternion() {
    value_[0] = 0;
    value_[1] = 0;
    value_[2] = 0;
    value_[3] = 1;
  }
  Quaternion(const Quaternion& other) { *this = other; }
  Quaternion(float x, float y, float z, float w) {
    value_[0] = x;
    value_[1] = y;
    value_[2] = z;
    value_[3] = w;
  }
  Quaternion(float angle, const Vector& axis);
  Quaternion(const Matrix& m);
  Quaternion(const Vector& direction, const Vector& up);
  ~Quaternion() {}

  const float* value() const { return value_; }
  float* mutable_value() { return value_; }

  const float x() const { return value_[0]; }
  const float y() const { return value_[1]; }
  const float z() const { return value_[2]; }
  const float w() const { return value_[3]; }

  const Quaternion& operator=(const Quaternion& other) {
    value_[0] = other.value_[0];
    value_[1] = other.value_[1];
    value_[2] = other.value_[2];
    value_[3] = other.value_[3];
    return *this;
  }

  bool operator==(const Quaternion& other) const;
  bool operator!=(const Quaternion& other) const;
  Quaternion operator*(const Quaternion& q) const;

  float Dot(const Quaternion& other) const;
  Quaternion& Normalize();
  Quaternion Inverse() const { return Quaternion(-x(), -y(), -z(), w()); }

  Matrix Rotation() const;
  Vector Rotate(const Vector& v) const;

  Vector ToAngleAxis();
  void ConvertCoordinateSystem(math::CoordinateSystem from,
                               math::CoordinateSystem to);

 private:
  float value_[4];
};

std::ostream& operator<<(std::ostream& out, const Quaternion& quaternion);
