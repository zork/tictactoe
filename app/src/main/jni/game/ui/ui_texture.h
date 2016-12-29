////
// ui_texture.h
////

#pragma once

#include "base/macros.h"
#include "base/thread/mutex.h"
#include "game/render/texture.h"

#include <memory>
#include <string>

class Bitmap;
class Texture;

namespace ui {

class UiTexture {
 public:
  UiTexture();
  ~UiTexture();
  DISALLOW_COPY_AND_ASSIGN(UiTexture);

  void SetImage(const std::string& filename);
  void SetImage(std::unique_ptr<Bitmap> image);

  bool UploadTexture();

  bool IsReady() { return texture_.get(); }
  void Bind() { texture_->Bind(); }

  bool IsSet() {
    AutoLock lock(&lock_);
    return image_ || texture_;
  }

  int width() const { return width_; }
  int height() const { return height_; }
  float right() const { return right_; }
  float top() const { return top_; }

 private:
  // Dimensions of the image.
  int width_;
  int height_;

  // Position of the texture.
  float right_;
  float top_;

  std::unique_ptr<Texture> texture_;

  Mutex lock_;
  std::unique_ptr<Bitmap> image_;
};

}  // namespace ui
