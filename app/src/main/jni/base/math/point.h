////
// point.h
////

#pragma once

namespace math {

class Point {
 public:
  static float Distance(const Point& lhs, const Point& rhs);

  Point() : x_(0), y_(0) {}
  ~Point() {}

  Point(float in_x, float in_y) : x_(in_x), y_(in_y) {}

  float x() const { return x_; }
  float y() const { return y_; }

  void SetX(float x) { x_ = x; }
  void SetY(float y) { y_ = y; }

  bool operator==(const Point& other) const;
  bool operator!=(const Point& other) const;
  Point operator+(const Point& other) const;
  Point operator-(const Point& other) const;
  Point operator*(float scalar) const;
  Point operator/(float scalar) const;
  const Point& operator+=(const Point& other);
  const Point& operator-=(const Point& other);
  const Point& operator*=(float scalar);
  const Point& operator/=(float scalar);

 private:
  float x_;
  float y_;
};

}  // namespace math
