////
// slide_gesture.h
////

#pragma once

#include "base/math/point.h"
#include "game/input/gesture.h"
#include "game/input/touch_event.h"

class SlideGesture : public Gesture {
 public:
  class Listener {
   public:
    virtual void OnSlide(SlideGesture* gesture, TouchEvent::State state) = 0;
  };

  SlideGesture(Listener* listener);
  ~SlideGesture() override;

  const math::Point& initial_position() {
    return captured_touch_->touches().front().position;
  }
  const math::Point& current_position() { return current_position_; }
  int delta_x() { return (int)(current_position_.x() - last_position_.x()); };
  int delta_y() { return (int)(current_position_.y() - last_position_.y()); };

 private:
  // InputListener overrides
  bool OnMouseEvent(const MouseEvent& event) override;
  bool OnTouchEvent(std::vector<TouchEvent*>& touches, int index) override;
  void OnTouchDelete(TouchEvent* touch) override;

  Listener* listener_;
  TouchEvent* captured_touch_;
  math::Point current_position_;
  math::Point last_position_;
};
