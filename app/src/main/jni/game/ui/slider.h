////
// slider.h
////

#pragma once

#include "base/macros.h"
#include "game/ui/view.h"

#include <memory>
#include <string>

class Bitmap;

namespace ui {

class Slider : public View {
 public:
  class Listener {
   public:
    virtual ~Listener() {}
    virtual void OnChange(Slider* slider) = 0;
  };

  Slider();
  ~Slider() override;
  DISALLOW_COPY_AND_ASSIGN(Slider);

  void SetSliderListener(Listener* listener) { listener_ = listener; }

  void SetMin(int value) { min_ = value; }
  void SetMax(int value) { max_ = value; }
  void SetValue(int value) { value_ = value; }
  int value() { return value_; }

  void SetLeftCap(int value) { left_cap_ = value; }
  void SetRightCap(int value) { right_cap_ = value; }

  void SetThumbImage(const std::string& filename);
  void SetThumbImage(std::unique_ptr<Bitmap> image);

  void SetMinImage(const std::string& filename);
  void SetMinImage(std::unique_ptr<Bitmap> image);

  void SetMaxImage(const std::string& filename);
  void SetMaxImage(std::unique_ptr<Bitmap> image);

 private:
  int SliderWidth() { return bounds().width() - left_cap_ - right_cap_; }

  void UpdateLayoutHeightFromImage(UiTexture* texture);
  void DrawMinImage(RenderState* render_state, int mid_point);
  void DrawMaxImage(RenderState* render_state, int mid_point);
  void DrawThumbImage(RenderState* render_state, int mid_point);

  // View:
  void RenderInternal(RenderState* render_state) override;

  // InputListener:
  bool OnMouseEvent(const MouseEvent& event) override;
  bool OnTouchEvent(std::vector<TouchEvent*>& touches, int index) override;
  void OnTouchDelete(TouchEvent* touch) override;

  Listener* listener_;
  bool mouse_captured_;
  TouchEvent* captured_touch_;

  int min_;
  int max_;
  int value_;

  int left_cap_;
  int right_cap_;

  std::unique_ptr<UiTexture> thumb_image_;
  std::unique_ptr<UiTexture> min_image_;
  std::unique_ptr<UiTexture> max_image_;
};

}  // namespace ui
