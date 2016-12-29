////
// image.cpp
////

#include "game/ui/image.h"

#include "base/image/bitmap.h"
#include "game/ui/ui_texture.h"

namespace ui {

Image::Image() : layout_fill_(false) {}

Image::~Image() {}

void Image::SetImage(const std::string& filename) {
  if (!image_)
    image_ = std::make_unique<UiTexture>();
  image_->SetImage(filename);
  RequestLayout();
}

void Image::SetImage(std::unique_ptr<Bitmap> image) {
  if (!image_)
    image_ = std::make_unique<UiTexture>();
  image_->SetImage(std::move(image));
  RequestLayout();
}

// protected:
// View:
void Image::Measure(int* width, int* height) {
  if (layout_fill_) {
    *width = -1;
    *height = -1;
  } else {
    if (layout_width() >= 0) {
      *width = layout_width();
    } else if (image_) {
      *width = image_->width();
    } else {
      *width = -1;
    }

    if (layout_height() >= 0) {
      *height = layout_height();
    } else if (image_) {
      *height = image_->height();
    } else {
      *height = -1;
    }
  }
}

void Image::RenderInternal(RenderState* render_state) {
  DrawImage(render_state, image_.get(), bounds());
  RenderChildren(render_state);
}

}  // namespace ui
