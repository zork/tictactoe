////
// bitmap.h
////

#pragma once

#include "base/basic_types.h"
#include "base/macros.h"

#include <memory>
#include <vector>

class Bitmap {
 public:
  Bitmap(int width, int height, bool has_alpha = true, int color_planes = 3);
  ~Bitmap();
  DISALLOW_COPY_AND_ASSIGN(Bitmap);

  std::unique_ptr<Bitmap> Resize(int new_width, int new_height);

  unsigned int width() const { return image_width_; }
  unsigned int height() const { return image_height_; }
  bool has_alpha() const { return has_alpha_; }
  int color_planes() const { return color_planes_; }
  const uint8_t* image_data() const { return image_data_.data(); }
  uint8_t* image_data() { return image_data_.data(); }

 private:
  const int image_width_;
  const int image_height_;

  const bool has_alpha_;
  const int color_planes_;

  std::vector<uint8_t> image_data_;
};
