////
// bitmap.cpp
////

#include "base/image/bitmap.h"

#include "base/logging.h"

#include <algorithm>

Bitmap::Bitmap(int width, int height, bool has_alpha, int color_planes)
    : image_width_(width),
      image_height_(height),
      has_alpha_(has_alpha),
      color_planes_(color_planes) {
  DCHECK(color_planes_ == 1 || color_planes_ == 3);
  int pixel_size = color_planes_;
  if (has_alpha_)
    ++pixel_size;
  image_data_.resize(pixel_size * width * height);
}

Bitmap::~Bitmap() {}

std::unique_ptr<Bitmap> Bitmap::Resize(int new_width, int new_height) {
  auto new_image = std::make_unique<Bitmap>(new_width, new_height, has_alpha_,
                                            color_planes_);

  int pixel_size = color_planes_;
  if (has_alpha_)
    ++pixel_size;

  int copy_size = std::min(image_width_, new_width) * pixel_size;
  int clear_size = 0;
  if (new_width > image_width_)
    clear_size = (new_width - image_width_) * pixel_size;

  uint8_t* dest_image = &new_image->image_data_[0];

  int y;
  for (y = 0; y < image_height_; ++y) {
    // Copy the old data.
    memcpy(dest_image + y * new_width * pixel_size,
           &image_data_[0] + y * image_width_ * pixel_size, copy_size);
    // Clear the rest of the line.
    memset(dest_image + (y * new_width + image_width_) * pixel_size, 0,
           clear_size);
  }

  // Clear the remaining rows in the image.
  while (y < new_height) {
    memset(dest_image + y * new_width * pixel_size, 0, new_width * pixel_size);
    ++y;
  }

  return new_image;
}
