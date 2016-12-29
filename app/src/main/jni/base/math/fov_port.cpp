////
// fov_port.cpp
////

#include "base/math/fov_port.h"

#include <math.h>

FovPort::FovPort() : left_tan_(0), right_tan_(0), down_tan_(0), up_tan_(0) {}

FovPort::FovPort(int width, int height, float vertical_fov) {
  float aspect = (float)width / height;
  down_tan_ = tan(vertical_fov / 2);
  up_tan_ = down_tan_;
  right_tan_ = down_tan_ * aspect;
  left_tan_ = up_tan_ * aspect;
}

FovPort::FovPort(float left_tan, float right_tan, float down_tan, float up_tan)
    : left_tan_(left_tan),
      right_tan_(right_tan),
      down_tan_(down_tan),
      up_tan_(up_tan) {}
