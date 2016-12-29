////
// texture.h
////

#pragma once

#include "base/basic_types.h"
#include "base/macros.h"
#include "base/thread/mutex.h"

#include <memory>

class Bitmap;

class Texture {
 public:
  enum Filter {
    kNearest,
    kLinear,
  };

  enum Wrap {
    kRepeat,
    kClampToEdge,
  };

  static std::unique_ptr<Texture> LoadResizedImage(
      std::unique_ptr<Bitmap> image,
      Filter filter,
      Wrap wrap,
      float* texture_right,
      float* texture_top);
  static unsigned int GetFilter(Filter filter);
  static unsigned int GetWrap(Wrap wrap);

  Texture(std::unique_ptr<Bitmap> image, Filter filter, Wrap wrap);
  ~Texture();
  DISALLOW_COPY_AND_ASSIGN(Texture);

  // Bind the texture.  Called on the Render thread.
  void Bind();

 private:
  const Filter filter_;
  const Wrap wrap_;

  uint32_t texture_id_;
  std::unique_ptr<Bitmap> image_;
};
