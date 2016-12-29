////
// vector.h
////

#pragma once

#include <iosfwd>

class Vector {
 public:
  Vector() {
    value_[0] = 0;
    value_[1] = 0;
    value_[2] = 0;
  }
  Vector(const Vector& other) { *this = other; }
  Vector(float x, float y, float z) {
    value_[0] = x;
    value_[1] = y;
    value_[2] = z;
  }
  ~Vector() {}

  const float* value() const { return value_; }
  float* mutable_value() { return value_; }

  const float x() const { return value_[0]; }
  const float y() const { return value_[1]; }
  const float z() const { return value_[2]; }

  void SetX(float in_x) { value_[0] = in_x; }
  void SetY(float in_y) { value_[1] = in_y; }
  void SetZ(float in_z) { value_[2] = in_z; }

  const Vector& operator=(const Vector& other) {
    value_[0] = other.value_[0];
    value_[1] = other.value_[1];
    value_[2] = other.value_[2];
    return *this;
  }

  bool operator==(const Vector& other) const;
  bool operator!=(const Vector& other) const;

  Vector operator-() const { return Vector(-x(), -y(), -z()); }
  Vector operator*(float scalar) const {
    return Vector(x() * scalar, y() * scalar, z() * scalar);
  }
  Vector operator*(double scalar) const {
    return Vector(x() * scalar, y() * scalar, z() * scalar);
  }
  Vector operator*(int scalar) const {
    return Vector(x() * scalar, y() * scalar, z() * scalar);
  }
  Vector operator/(float scalar) const {
    return Vector(x() / scalar, y() / scalar, z() / scalar);
  }
  Vector operator/(double scalar) const {
    return Vector(x() / scalar, y() / scalar, z() / scalar);
  }
  Vector operator/(int scalar) const {
    return Vector(x() / scalar, y() / scalar, z() / scalar);
  }
  Vector operator+(const Vector& other) const {
    return Vector(x() + other.x(), y() + other.y(), z() + other.z());
  }
  Vector operator-(const Vector& other) const {
    return Vector(x() - other.x(), y() - other.y(), z() - other.z());
  }
  Vector& operator+=(const Vector& other) {
    value_[0] += other.value_[0];
    value_[1] += other.value_[1];
    value_[2] += other.value_[2];
    return *this;
  }
  Vector& operator-=(const Vector& other) {
    value_[0] -= other.value_[0];
    value_[1] -= other.value_[1];
    value_[2] -= other.value_[2];
    return *this;
  }
  Vector& operator*=(float scalar) {
    *this = *this * scalar;
    return *this;
  }
  Vector& operator*=(int scalar) {
    *this = *this * scalar;
    return *this;
  }
  Vector& operator/=(float scalar) {
    *this = *this / scalar;
    return *this;
  }
  Vector& operator/=(int scalar) {
    *this = *this / scalar;
    return *this;
  }

  Vector Cross(const Vector& other) const;
  float Dot(const Vector& other) const;
  Vector& Normalize();
  bool IsNormalized() const;

 private:
  float value_[3];
};

class Vector4 {
 public:
  Vector4() {
    value_[0] = 0;
    value_[1] = 0;
    value_[2] = 0;
    value_[3] = 1;
  }
  Vector4(const Vector4& other) { *this = other; }
  Vector4(const Vector& other) {
    value_[0] = other.x();
    value_[1] = other.y();
    value_[2] = other.z();
    value_[3] = 1;
  }
  Vector4(float x, float y, float z, float w) {
    value_[0] = x;
    value_[1] = y;
    value_[2] = z;
    value_[3] = w;
  }
  ~Vector4() {}

  const float* value() const { return value_; }
  float* mutable_value() { return value_; }

  const float x() const { return value_[0]; }
  const float y() const { return value_[1]; }
  const float z() const { return value_[2]; }
  const float w() const { return value_[3]; }

  void SetX(float in_x) { value_[0] = in_x; }
  void SetY(float in_y) { value_[1] = in_y; }
  void SetZ(float in_z) { value_[2] = in_z; }
  void SetW(float in_w) { value_[3] = in_w; }

  const Vector4& operator=(const Vector4& other) {
    value_[0] = other.value_[0];
    value_[1] = other.value_[1];
    value_[2] = other.value_[2];
    value_[3] = other.value_[3];
    return *this;
  }

  bool operator==(const Vector4& other) const;
  bool operator!=(const Vector4& other) const;

  Vector4 operator-() const { return Vector4(-x(), -y(), -z(), -w()); }
  Vector4 operator*(float scalar) const {
    return Vector4(x() * scalar, y() * scalar, z() * scalar, w() * scalar);
  }
  Vector4 operator*(int scalar) const {
    return Vector4(x() * scalar, y() * scalar, z() * scalar, w() * scalar);
  }
  Vector4 operator/(float scalar) const {
    return Vector4(x() / scalar, y() / scalar, z() / scalar, w() / scalar);
  }
  Vector4 operator/(int scalar) const {
    return Vector4(x() / scalar, y() / scalar, z() / scalar, w() / scalar);
  }
  Vector4 operator+(const Vector4& other) const {
    return Vector4(x() + other.x(), y() + other.y(), z() + other.z(),
                   w() + other.w());
  }
  Vector4 operator-(const Vector4& other) const {
    return Vector4(x() - other.x(), y() - other.y(), z() - other.z(),
                   w() - other.w());
  }
  Vector4& operator+=(const Vector4& other) {
    value_[0] += other.value_[0];
    value_[1] += other.value_[1];
    value_[2] += other.value_[2];
    value_[3] += other.value_[3];
    return *this;
  }
  Vector4& operator-=(const Vector4& other) {
    value_[0] -= other.value_[0];
    value_[1] -= other.value_[1];
    value_[2] -= other.value_[2];
    value_[3] -= other.value_[3];
    return *this;
  }
  Vector4& operator*=(float scalar) {
    *this = *this * scalar;
    return *this;
  }
  Vector4& operator*=(int scalar) {
    *this = *this * scalar;
    return *this;
  }
  Vector4& operator/=(float scalar) {
    *this = *this / scalar;
    return *this;
  }
  Vector4& operator/=(int scalar) {
    *this = *this / scalar;
    return *this;
  }

 private:
  float value_[4];
};

std::ostream& operator<<(std::ostream& out, const Vector& vector);
std::ostream& operator<<(std::ostream& out, const Vector4& vector);
