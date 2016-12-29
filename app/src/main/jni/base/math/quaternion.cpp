////
// quaternion.cpp
////

#include "base/math/quaternion.h"

#include "base/math/math.h"
#include "base/math/matrix.h"
#include "base/math/vector.h"

#include <cmath>
#include <iomanip>
#include <iostream>

namespace {
// This will let us detect differnce in angle of up to .00006 degrees.
const float kRotationMaxDiff = 0.0000005;

void MatrixToQuaternion(const Matrix& m, float output[4]) {
  // See:
  // http://www.euclideanspace.com/maths/geometry/
  //        rotations/conversions/matrixToQuaternion/
  float trace = m.value()[0] + m.value()[5] + m.value()[10];
  if (trace > 0) {
    float s = 0.5f / sqrt(trace + 1.0f);
    output[0] = (m.value()[6] - m.value()[9]) * s;
    output[1] = (m.value()[8] - m.value()[2]) * s;
    output[2] = (m.value()[1] - m.value()[4]) * s;
    output[3] = 0.25f / s;
  } else {
    if (m.value()[0] > m.value()[5] && m.value()[0] > m.value()[10]) {
      float s = 2.0f * sqrt(1.0f + m.value()[0] - m.value()[5] - m.value()[10]);
      output[0] = 0.25f * s;
      output[1] = (m.value()[4] + m.value()[1]) / s;
      output[2] = (m.value()[8] + m.value()[2]) / s;
      output[3] = (m.value()[6] - m.value()[9]) / s;
    } else if (m.value()[5] > m.value()[10]) {
      float s = 2.0f * sqrt(1.0f + m.value()[5] - m.value()[0] - m.value()[10]);
      output[0] = (m.value()[4] + m.value()[1]) / s;
      output[1] = 0.25f * s;
      output[2] = (m.value()[9] + m.value()[6]) / s;
      output[3] = (m.value()[8] - m.value()[2]) / s;
    } else {
      float s = 2.0f * sqrt(1.0f + m.value()[10] - m.value()[0] - m.value()[5]);
      output[0] = (m.value()[8] + m.value()[2]) / s;
      output[1] = (m.value()[9] + m.value()[6]) / s;
      output[2] = 0.25f * s;
      output[3] = (m.value()[1] - m.value()[4]) / s;
    }
  }
}
}

// static
Quaternion Quaternion::FromAngleAxis(const Vector& v) {
  float theta_squared = v.Dot(v);
  if (theta_squared > 0) {
    // The value is numerically stable away from the origin.
    const float theta = sqrt(theta_squared);
    const float half_theta = theta * .5;
    const float k = sin(half_theta) / theta;
    return Quaternion(v.x() * k, v.y() * k, v.z() * k, cos(half_theta));
  } else {
    // Not numerically stable, so we'll fudge it with a taylor series, but
    // truncate one term.
    const float k = .5;
    return Quaternion(v.x() * k, v.y() * k, v.z() * k, 1.0);
  }
}

Quaternion::Quaternion(float angle, const Vector& axis) {
  double sin_value = sin(angle / 2);
  value_[0] = axis.x() * sin_value;
  value_[1] = axis.y() * sin_value;
  value_[2] = axis.z() * sin_value;
  value_[3] = cos(angle / 2);
}

Quaternion::Quaternion(const Matrix& m) {
  MatrixToQuaternion(m, value_);
}

Quaternion::Quaternion(const Vector& direction, const Vector& up) {
  MatrixToQuaternion(Matrix::RotationTo(direction, up), value_);
}

bool Quaternion::operator==(const Quaternion& other) const {
  return math::FloatEqual(std::abs(Dot(other)), 1, kRotationMaxDiff);
}

bool Quaternion::operator!=(const Quaternion& other) const {
  return !math::FloatEqual(std::abs(Dot(other)), 1, kRotationMaxDiff);
}

Quaternion Quaternion::operator*(const Quaternion& q) const {
  return Quaternion(w() * q.x() + x() * q.w() + y() * q.z() - z() * q.y(),
                    w() * q.y() - x() * q.z() + y() * q.w() + z() * q.x(),
                    w() * q.z() + x() * q.y() - y() * q.x() + z() * q.w(),
                    w() * q.w() - x() * q.x() - y() * q.y() - z() * q.z());
}

float Quaternion::Dot(const Quaternion& other) const {
  return x() * other.x() + y() * other.y() + z() * other.z() + w() * other.w();
}

Quaternion& Quaternion::Normalize() {
  float sq_length = x() * x() + y() * y() + z() * z() + w() * w();
  if (!math::FloatEqual(sq_length, 0)) {
    float length = sqrt(sq_length);
    value_[0] /= length;
    value_[1] /= length;
    value_[2] /= length;
    value_[3] /= length;
  }
  return *this;
}

Matrix Quaternion::Rotation() const {
  return Matrix(
      // First Column
      1 - 2 * y() * y() - 2 * z() * z(), 2 * x() * y() + 2 * z() * w(),
      2 * x() * z() - 2 * y() * w(), 0,

      // Second Column
      2 * x() * y() - 2 * z() * w(), 1 - 2 * x() * x() - 2 * z() * z(),
      2 * y() * z() + 2 * x() * w(), 0,

      // Third Column
      2 * x() * z() + 2 * y() * w(), 2 * y() * z() - 2 * x() * w(),
      1 - 2 * x() * x() - 2 * y() * y(), 0,

      // Fourth Column
      0, 0, 0, 1);
}

Vector Quaternion::Rotate(const Vector& v) const {
  return Rotation() * v;
}

Vector Quaternion::ToAngleAxis() {
  const float sin_squared = x() * x() + y() * y() + z() * z();
  if (sin_squared > 0) {
    // For non-zero rotation, the value is stable.
    const float sin_theta = sqrt(sin_squared);
    const float k = 2.0 * atan2(sin_theta, w()) / sin_theta;
    return Vector(x() * k, y() * k, z() * k);
  } else {
    // Not numerically stable, so we'll fudge it with a taylor series, but
    // truncate one term.
    const float k = 2;
    return Vector(x() * k, y() * k, z() * k);
  }
}

void Quaternion::ConvertCoordinateSystem(math::CoordinateSystem from,
                                         math::CoordinateSystem to) {
  if (from == to)
    return;

  bool flip_x =
      (from == math::kLeftHandedYDown && to == math::kRightHandedYDown) ||
      (from == math::kLeftHandedYDown && to == math::kRightHandedYUp) ||
      (from == math::kLeftHandedYUp && to == math::kRightHandedYDown) ||
      (from == math::kLeftHandedYUp && to == math::kRightHandedYUp) ||
      (from == math::kRightHandedYDown && to == math::kLeftHandedYDown) ||
      (from == math::kRightHandedYDown && to == math::kLeftHandedYUp) ||
      (from == math::kRightHandedYUp && to == math::kLeftHandedYDown) ||
      (from == math::kRightHandedYUp && to == math::kLeftHandedYUp);

  bool flip_y =
      (from == math::kLeftHandedYDown && to == math::kLeftHandedYUp) ||
      (from == math::kLeftHandedYDown && to == math::kRightHandedYDown) ||
      (from == math::kLeftHandedYUp && to == math::kLeftHandedYDown) ||
      (from == math::kLeftHandedYUp && to == math::kRightHandedYUp) ||
      (from == math::kRightHandedYDown && to == math::kLeftHandedYDown) ||
      (from == math::kRightHandedYDown && to == math::kRightHandedYUp) ||
      (from == math::kRightHandedYUp && to == math::kLeftHandedYUp) ||
      (from == math::kRightHandedYUp && to == math::kRightHandedYDown);

  bool flip_z =
      (from == math::kLeftHandedYDown && to == math::kLeftHandedYUp) ||
      (from == math::kLeftHandedYDown && to == math::kRightHandedYUp) ||
      (from == math::kLeftHandedYUp && to == math::kLeftHandedYDown) ||
      (from == math::kLeftHandedYUp && to == math::kRightHandedYDown) ||
      (from == math::kRightHandedYDown && to == math::kLeftHandedYUp) ||
      (from == math::kRightHandedYDown && to == math::kRightHandedYUp) ||
      (from == math::kRightHandedYUp && to == math::kLeftHandedYDown) ||
      (from == math::kRightHandedYUp && to == math::kRightHandedYDown);

  if (flip_x)
    value_[0] = -value_[0];
  if (flip_y)
    value_[1] = -value_[1];
  if (flip_z)
    value_[2] = -value_[2];
}

std::ostream& operator<<(std::ostream& out, const Quaternion& quaternion) {
  return out << std::fixed << std::setprecision(2) << "(" << quaternion.x()
             << ", " << quaternion.y() << ", " << quaternion.z() << ", "
             << quaternion.w() << ")";
}
