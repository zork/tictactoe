////
// matrix.cpp
////

#include "base/math/matrix.h"

#include "base/logging.h"
#include "base/math/fov_port.h"
#include "base/math/quaternion.h"
#include "base/math/vector.h"

#include <math.h>
#include <string.h>
#include <iomanip>
#include <iostream>

// static
Matrix Matrix::Rotation(float angle, const Vector& v) {
  float cos_val = cos(angle);
  float sin_val = sin(angle);

  return Matrix(
      // First Column
      cos_val + v.x() * v.x() * (1 - cos_val),
      v.y() * v.x() * (1 - cos_val) + v.z() * sin_val,
      v.z() * v.x() * (1 - cos_val) - v.y() * sin_val, 0,

      // Second Column
      v.x() * v.y() * (1 - cos_val) - v.z() * sin_val,
      cos_val + v.y() * v.y() * (1 - cos_val),
      v.z() * v.y() * (1 - cos_val) + v.x() * sin_val, 0,

      // Third Column
      v.x() * v.z() * (1 - cos_val) + v.y() * sin_val,
      v.y() * v.z() * (1 - cos_val) - v.x() * sin_val,
      cos_val + v.z() * v.z() * (1 - cos_val), 0,

      // Fourth Column
      0, 0, 0, 1);
}

// static
Matrix Matrix::RotationTo(const Vector& z_axis, const Vector& y_axis) {
  Vector x_axis = y_axis.Cross(z_axis);

  return Matrix(
      // First Column
      x_axis.x(), x_axis.y(), x_axis.z(), 0,

      // Second Column
      y_axis.x(), y_axis.y(), y_axis.z(), 0,

      // Third Column
      z_axis.x(), z_axis.y(), z_axis.z(), 0,

      // Fourth Column
      0, 0, 0, 1);
}

// static
Matrix Matrix::RotationFrom(const Vector& z_axis, const Vector& y_axis) {
  Vector x_axis = y_axis.Cross(z_axis);

  return Matrix(
      // First Column
      x_axis.x(), y_axis.x(), z_axis.x(), 0,

      // Second Column
      x_axis.y(), y_axis.y(), z_axis.y(), 0,

      // Third Column
      x_axis.z(), y_axis.z(), z_axis.z(), 0,

      // Fourth Column
      0, 0, 0, 1);
}

// static
Matrix Matrix::Translation(const Vector& v) {
  return Matrix(
      // First Column
      1, 0, 0, 0,
      // Second Column
      0, 1, 0, 0,
      // Third Column
      0, 0, 1, 0,
      // Fourth Column
      v.x(), v.y(), v.z(), 1);
}

Matrix::Matrix() {
  // First Column
  value_[0] = 1;
  value_[1] = 0;
  value_[2] = 0;
  value_[3] = 0;

  // Second Column
  value_[4] = 0;
  value_[5] = 1;
  value_[6] = 0;
  value_[7] = 0;

  // Third Column
  value_[8] = 0;
  value_[9] = 0;
  value_[10] = 1;
  value_[11] = 0;

  // Fourth Column
  value_[12] = 0;
  value_[13] = 0;
  value_[14] = 0;
  value_[15] = 1;
}

Matrix::Matrix(const Matrix& other) {
  *this = other;
}

Matrix::Matrix(float x0,
               float x1,
               float x2,
               float x3,
               float y0,
               float y1,
               float y2,
               float y3,
               float z0,
               float z1,
               float z2,
               float z3,
               float w0,
               float w1,
               float w2,
               float w3) {
  // First Column
  value_[0] = x0;
  value_[1] = x1;
  value_[2] = x2;
  value_[3] = x3;

  // Second Column
  value_[4] = y0;
  value_[5] = y1;
  value_[6] = y2;
  value_[7] = y3;

  // Third Column
  value_[8] = z0;
  value_[9] = z1;
  value_[10] = z2;
  value_[11] = z3;

  // Fourth Column
  value_[12] = w0;
  value_[13] = w1;
  value_[14] = w2;
  value_[15] = w3;
}

Matrix::~Matrix() {}

void Matrix::Identity() {
  // First Column
  value_[0] = 1;
  value_[1] = 0;
  value_[2] = 0;
  value_[3] = 0;

  // Second Column
  value_[4] = 0;
  value_[5] = 1;
  value_[6] = 0;
  value_[7] = 0;

  // Third Column
  value_[8] = 0;
  value_[9] = 0;
  value_[10] = 1;
  value_[11] = 0;

  // Fourth Column
  value_[12] = 0;
  value_[13] = 0;
  value_[14] = 0;
  value_[15] = 1;
}

void Matrix::FrustumProjection(float left,
                               float right,
                               float bottom,
                               float top,
                               float near,
                               float far) {
  float a = (right + left) / (right - left);
  float b = (top + bottom) / (top - bottom);
  float c = -(far + near) / (far - near);
  float d = -2.0 * far * near / (far - near);

  // First Column
  value_[0] = 2.0 * near / (right - left);
  value_[1] = 0.0;
  value_[2] = 0.0;
  value_[3] = 0.0;

  // Second Column
  value_[4] = 0.0;
  value_[5] = 2.0 * near / (top - bottom);
  value_[6] = 0.0;
  value_[7] = 0.0;

  // Third Column
  value_[8] = a;
  value_[9] = b;
  value_[10] = c;
  value_[11] = -1.0;

  // Fourth Column
  value_[12] = 0.0;
  value_[13] = 0.0;
  value_[14] = d;
  value_[15] = 0.0;
}

void Matrix::FrustumProjection(const FovPort& fov,
                               float near,
                               float far,
                               math::CoordinateSystem coordinate_system) {
  float left = -near * fov.left_tan();
  float right = near * fov.right_tan();
  float bottom = -near * fov.down_tan();
  float top = near * fov.up_tan();

  switch (coordinate_system) {
    case math::kLeftHandedYUp:
    case math::kRightHandedYUp:
      break;

    case math::kLeftHandedYDown:
    case math::kRightHandedYDown:
      bottom = -bottom;
      top = -top;
      break;
  }

  FrustumProjection(left, right, bottom, top, near, far);

  switch (coordinate_system) {
    case math::kLeftHandedYDown:
    case math::kRightHandedYUp:
      break;

    case math::kLeftHandedYUp:
    case math::kRightHandedYDown:
      value_[8] = -value_[8];
      value_[9] = -value_[9];
      value_[10] = -value_[10];
      value_[11] = -value_[11];
      break;
  }
}

void Matrix::OrthoProjection(float left,
                             float right,
                             float bottom,
                             float top,
                             float near,
                             float far) {
  // First Column
  value_[0] = 2.0 / (right - left);
  value_[1] = 0.0;
  value_[2] = 0.0;
  value_[3] = 0.0;

  // Second Column
  value_[4] = 0.0;
  value_[5] = 2.0 / (top - bottom);
  value_[6] = 0.0;
  value_[7] = 0.0;

  // Third Column
  value_[8] = 0.0;
  value_[9] = 0.0;
  value_[10] = -2.0 / (far - near);
  value_[11] = 0.0;

  // Fourth Column
  value_[12] = (right + left) / (left - right);
  value_[13] = (top + bottom) / (bottom - top);
  value_[14] = (far + near) / (near - far);
  value_[15] = 1;
}

void Matrix::Rotate(float angle, float x, float y, float z) {
  *this = *this * Rotation(angle, Vector(x, y, z));
}

void Matrix::Rotate(float angle, const Vector& vector) {
  *this = *this * Rotation(angle, vector);
}

void Matrix::Translate(float x, float y, float z) {
  *this = *this * Translation(Vector(x, y, z));
}

void Matrix::Translate(const Vector& vector) {
  *this = *this * Translation(vector);
}

void Matrix::Scale(float x, float y, float z) {
  const Matrix scale_matrix(
      // First Column
      x, 0, 0, 0,
      // Second Column
      0, y, 0, 0,
      // Third Column
      0, 0, z, 0,
      // Fourth Column
      0, 0, 0, 1);
  *this = *this * scale_matrix;
}

void Matrix::RotateTo(const Vector& z_axis, const Vector& y_axis) {
  *this = *this * RotationTo(z_axis, y_axis);
}

void Matrix::RotateFrom(const Vector& z_axis, const Vector& y_axis) {
  *this = *this * RotationFrom(z_axis, y_axis);
}

const Matrix& Matrix::operator=(const Matrix& other) {
  memcpy(value_, other.value_, sizeof(value_));
  return *this;
}

Matrix Matrix::operator*(const Matrix& other) const {
  return Matrix(
      // 0, 0
      other.value_[0] * value_[0] + other.value_[1] * value_[4] +
          other.value_[2] * value_[8] + other.value_[3] * value_[12],

      // 0, 1
      other.value_[0] * value_[1] + other.value_[1] * value_[5] +
          other.value_[2] * value_[9] + other.value_[3] * value_[13],

      // 0, 2
      other.value_[0] * value_[2] + other.value_[1] * value_[6] +
          other.value_[2] * value_[10] + other.value_[3] * value_[14],

      // 0, 3
      other.value_[0] * value_[3] + other.value_[1] * value_[7] +
          other.value_[2] * value_[11] + other.value_[3] * value_[15],

      // 1, 0
      other.value_[4] * value_[0] + other.value_[5] * value_[4] +
          other.value_[6] * value_[8] + other.value_[7] * value_[12],

      // 1, 1
      other.value_[4] * value_[1] + other.value_[5] * value_[5] +
          other.value_[6] * value_[9] + other.value_[7] * value_[13],

      // 1, 2
      other.value_[4] * value_[2] + other.value_[5] * value_[6] +
          other.value_[6] * value_[10] + other.value_[7] * value_[14],

      // 1, 3
      other.value_[4] * value_[3] + other.value_[5] * value_[7] +
          other.value_[6] * value_[11] + other.value_[7] * value_[15],

      // 2, 0
      other.value_[8] * value_[0] + other.value_[9] * value_[4] +
          other.value_[10] * value_[8] + other.value_[11] * value_[12],

      // 2, 1
      other.value_[8] * value_[1] + other.value_[9] * value_[5] +
          other.value_[10] * value_[9] + other.value_[11] * value_[13],

      // 2, 2
      other.value_[8] * value_[2] + other.value_[9] * value_[6] +
          other.value_[10] * value_[10] + other.value_[11] * value_[14],

      // 2, 3
      other.value_[8] * value_[3] + other.value_[9] * value_[7] +
          other.value_[10] * value_[11] + other.value_[11] * value_[15],

      // 3, 0
      other.value_[12] * value_[0] + other.value_[13] * value_[4] +
          other.value_[14] * value_[8] + other.value_[15] * value_[12],

      // 3, 1
      other.value_[12] * value_[1] + other.value_[13] * value_[5] +
          other.value_[14] * value_[9] + other.value_[15] * value_[13],

      // 3, 2
      other.value_[12] * value_[2] + other.value_[13] * value_[6] +
          other.value_[14] * value_[10] + other.value_[15] * value_[14],

      // 3, 3
      other.value_[12] * value_[3] + other.value_[13] * value_[7] +
          other.value_[14] * value_[11] + other.value_[15] * value_[15]);
}

Vector Matrix::operator*(const Vector& v) const {
  return Vector(
      v.x() * value_[0] + v.y() * value_[4] + v.z() * value_[8] + value_[12],
      v.x() * value_[1] + v.y() * value_[5] + v.z() * value_[9] + value_[13],
      v.x() * value_[2] + v.y() * value_[6] + v.z() * value_[10] + value_[14]);
}

Vector4 Matrix::operator*(const Vector4& v) const {
  return Vector4(v.x() * value_[0] + v.y() * value_[4] + v.z() * value_[8] +
                     v.w() * value_[12],

                 v.x() * value_[1] + v.y() * value_[5] + v.z() * value_[9] +
                     v.w() * value_[13],

                 v.x() * value_[2] + v.y() * value_[6] + v.z() * value_[10] +
                     v.w() * value_[14],

                 v.x() * value_[3] + v.y() * value_[7] + v.z() * value_[11] +
                     v.w() * value_[15]);
}

std::ostream& operator<<(std::ostream& out, const Matrix& m) {
  return out << std::fixed << std::setprecision(2) << "(" << m.value()[0]
             << ", " << m.value()[4] << ", " << m.value()[8] << ", "
             << m.value()[12] << "\n " << m.value()[1] << ", " << m.value()[5]
             << ", " << m.value()[9] << ", " << m.value()[13] << "\n "
             << m.value()[2] << ", " << m.value()[6] << ", " << m.value()[10]
             << ", " << m.value()[14] << "\n " << m.value()[3] << ", "
             << m.value()[7] << ", " << m.value()[11] << ", " << m.value()[15]
             << ")";
}
