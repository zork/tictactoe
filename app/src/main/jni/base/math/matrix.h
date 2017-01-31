////
// matrix.h
////

#pragma once

#include "base/math/coordinate_system.h"

#include <iosfwd>

class FovPort;
class Vector;
class Vector4;

class Matrix {
 public:
  static Matrix Rotation(float angle, const Vector& v);
  static Matrix RotationTo(const Vector& z_axis, const Vector& y_axis);
  static Matrix RotationFrom(const Vector& z_axis, const Vector& y_axis);
  static Matrix Translation(const Vector& v);

  Matrix();
  Matrix(const Matrix& other);
  Matrix(float x0,
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
         float w3);
  ~Matrix();

  const float* value() const { return value_; }
  const float value(int row, int column) const {
    return value_[column + 4 * row];
  }

  void Identity();
  void FrustumProjection(float left,
                         float right,
                         float bottom,
                         float top,
                         float near,
                         float far);
  void FrustumProjection(const FovPort& fov,
                         float near,
                         float far,
                         math::CoordinateSystem coordinate_system);
  void OrthoProjection(float left,
                       float right,
                       float bottom,
                       float top,
                       float near,
                       float far);
  void Rotate(float angle, float x, float y, float z);
  void Rotate(float angle, const Vector& vector);
  void Translate(float x, float y, float z);
  void Translate(const Vector& vector);
  void Scale(float x, float y, float z);
  void RotateTo(const Vector& z_axis, const Vector& y_axis);
  void RotateFrom(const Vector& z_axis, const Vector& y_axis);

  const Matrix& operator=(const Matrix& other);
  Matrix operator*(const Matrix& other) const;
  Vector operator*(const Vector& v) const;
  Vector4 operator*(const Vector4& v) const;

 private:
  float value_[16];
};

std::ostream& operator<<(std::ostream& out, const Matrix& m);
