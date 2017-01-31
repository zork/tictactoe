////
// rect.h
////

#pragma once

#include <iosfwd>

namespace math {

class Rect {
 public:
  Rect();
  ~Rect();

  static const Rect MakeXYWH(int x, int y, int width, int height) {
    return Rect(x, y, width, height);
  }

  static const Rect MakeXYRB(int x, int y, int right, int bottom) {
    return Rect(x, y, right - x, bottom - y);
  }

  static const Rect MakeXYRT(int x, int y, int right, int top) {
    return Rect(x, y, right - x, top - y);
  }

  int x() const { return x_; }
  int y() const { return y_; }
  int width() const { return width_; }
  int height() const { return height_; }

  void SetX(int x) { x_ = x; }
  void SetY(int y) { y_ = y; }
  void SetWidth(int width) { width_ = width; }
  void SetHeight(int height) { height_ = height; }

  int bottom() const { return y_ + height_; }
  int right() const { return x_ + width_; }
  int top() const { return y_ + height_; }

  bool Contains(int x, int y) const {
    return (x >= x_ && y >= y_ && x < x_ + width_ && y < y_ + height_);
  }

  bool Intersects(const Rect& other) const {
    return x_ < other.right() && other.x_ < right() && y_ < other.top() &&
           other.y_ < top();
  }

 private:
  Rect(int x, int y, int width, int height);

  int x_;
  int y_;
  int width_;
  int height_;
};

}  // namespace math

std::ostream& operator<<(std::ostream& out, const math::Rect& rect);
