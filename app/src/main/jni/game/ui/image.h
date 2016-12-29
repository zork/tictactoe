////
// image.h
////

#pragma once

#include "base/macros.h"
#include "game/ui/view.h"

#include <memory>

class Bitmap;

namespace ui {

class UiTexture;

class Image : public View {
 public:
  Image();
  ~Image() override;
  DISALLOW_COPY_AND_ASSIGN(Image);

  void SetLayoutFill(bool fill) { layout_fill_ = fill; }
  bool layout_fill() const { return layout_fill_; }

  void SetImage(const std::string& filename);
  void SetImage(std::unique_ptr<Bitmap> image);

 protected:
  UiTexture* image() { return image_.get(); }

  // View:
  void Measure(int* width, int* height) override;
  void RenderInternal(RenderState* render_state) override;

 private:
  bool layout_fill_;
  std::unique_ptr<UiTexture> image_;
};

}  // namespace ui
