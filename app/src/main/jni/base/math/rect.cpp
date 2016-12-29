////
// rect.cpp
////

#include "base/math/rect.h"

#include <iomanip>
#include <iostream>

namespace math {

Rect::Rect() : x_(0), y_(0), width_(0), height_(0) {}

Rect::Rect(int x, int y, int width, int height)
    : x_(x), y_(y), width_(width), height_(height) {}

Rect::~Rect() {}

}  // namespace math

std::ostream& operator<<(std::ostream& out, const math::Rect& rect) {
  return out << "(" << rect.x() << ", " << rect.y() << "), (" << rect.right()
             << ", " << rect.bottom() << ")";
}
