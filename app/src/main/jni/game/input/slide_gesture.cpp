////
// slide_gesture.cpp
////

#include "game/input/slide_gesture.h"

#include <cmath>

using math::Point;

namespace {
const int kSlideThreshold = 20;
}

SlideGesture::SlideGesture(SlideGesture::Listener* listener)
    : listener_(listener), captured_touch_(0) {}

SlideGesture::~SlideGesture() {
  if (captured_touch_)
    captured_touch_->ReleaseCapture();
}

bool SlideGesture::OnMouseEvent(const MouseEvent& event) {
  return false;
}

bool SlideGesture::OnTouchEvent(std::vector<TouchEvent*>& touches, int index) {
  if (touches.size() > 1)
    return false;

  TouchEvent* touch = touches[index];
  TouchEvent::State state = touch->state();

  const TouchEvent::Touch& end = touch->touches().back();

  if (!captured_touch_) {
    if (touch->state() == TouchEvent::kTouchStart) {
      return false;
    }

    const TouchEvent::Touch& start = touch->touches().front();
    if (std::abs(end.position.x() - start.position.x()) >= kSlideThreshold ||
        std::abs(end.position.y() - start.position.y()) >= kSlideThreshold) {
      last_position_ = start.position;
      current_position_ = end.position;
      captured_touch_ = touch;
      touch->Capture(this);
      state = TouchEvent::kTouchStart;
    } else {
      return false;
    }
  } else {
    last_position_ = current_position_;
    current_position_ = end.position;
  }

  if (delta_x() || delta_y()) {
    listener_->OnSlide(this, state);
  }
  return true;
}

void SlideGesture::OnTouchDelete(TouchEvent* touch) {
  if (touch == captured_touch_) {
    captured_touch_ = 0;
  }
}
