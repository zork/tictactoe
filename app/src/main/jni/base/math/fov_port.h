////
// fov_port.h
////

#pragma once

// Represents a field of view.
class FovPort {
 public:
  FovPort();
  FovPort(int width, int height, float vertical_fov);
  FovPort(float left_tan, float right_tan, float down_tan, float up_tan);

  float left_tan() const { return left_tan_; }
  float right_tan() const { return right_tan_; }
  float down_tan() const { return down_tan_; }
  float up_tan() const { return up_tan_; }

 private:
  float left_tan_;
  float right_tan_;
  float down_tan_;
  float up_tan_;
};
