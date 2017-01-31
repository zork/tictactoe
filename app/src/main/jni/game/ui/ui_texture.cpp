////
// ui_texture.cpp
////

#include "game/ui/ui_texture.h"

#include "base/file/file.h"
#include "base/file/file_manager.h"
#include "base/image/pcx.h"
#include "base/strings/string_utils.h"
#include "base/thread/thread_util.h"

namespace ui {

UiTexture::UiTexture() : width_(0), height_(0), right_(0), top_(0) {}

UiTexture::~UiTexture() {}

void UiTexture::SetImage(const std::string& filename) {
  std::unique_ptr<File> file = FileManager::Get()->OpenAsset(filename);
  std::unique_ptr<Bitmap> image;
  std::string extension = GetFileExtension(filename);
  if (extension == "pcx") {
    image = LoadPcx(file.get());
  } else {
    NOTREACHED();
  }
  SetImage(std::move(image));
}

void UiTexture::SetImage(std::unique_ptr<Bitmap> image) {
  AutoLock lock(&lock_);
  image_ = std::move(image);
  width_ = image_->width();
  height_ = image_->height();
}

bool UiTexture::UploadTexture() {
  CHECK_THREAD(thread::Render);
  // Load the new texture, if it exists.
  AutoLock lock(&lock_);
  if (image_) {
    texture_ = Texture::LoadResizedImage(std::move(image_), Texture::kNearest,
                                         Texture::kNearestMipmapLinear,
                                         Texture::kClampToEdge, &right_, &top_);
    return true;
  }
  return false;
}

}  // namespace ui
