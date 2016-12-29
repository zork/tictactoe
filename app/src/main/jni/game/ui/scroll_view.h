////
// scroll_view.h
////

#pragma once

#include "base/macros.h"
#include "base/math/point.h"
#include "game/input/slide_gesture.h"
#include "game/ui/view.h"

namespace ui {

class ScrollView : public View, public SlideGesture::Listener {
 public:
  ScrollView();
  ~ScrollView() override;
  DISALLOW_COPY_AND_ASSIGN(ScrollView);

  math::Point GetScrollPosition();
  void SetScrollPosition(const math::Point& scroll_position);

 private:
  // InputListener:
  bool OnMouseEvent(const MouseEvent& event) override;

  // SlideGesture::Listener:
  void OnSlide(SlideGesture* gesture, TouchEvent::State state) override;

  // View:
  void LayoutChildren() override;
  void RenderInternal(RenderState* render_state) override;

  Mutex scroll_lock_;
  math::Point scroll_position_;
};

}  // namespace ui
