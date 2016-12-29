////
// touch_event.cpp
////

#include "game/input/touch_event.h"

#include "base/logging.h"
#include "game/input/input_listener.h"

TouchEvent::Touch::Touch(math::Point in_position, const Timestamp& in_time)
    : position(in_position), time(in_time) {}

TouchEvent::TouchEvent(long id)
    : id_(id), capture_listener_(0), captured_(false) {}

TouchEvent::~TouchEvent() {
  if (capture_listener_)
    capture_listener_->OnTouchDelete(this);
}

void TouchEvent::SetState(State new_state) {
  state_ = new_state;
}

void TouchEvent::AddPoint(int x, int y, const Timestamp& time) {
  touches_.push_back(Touch(math::Point(x, y), time));
}

void TouchEvent::Capture(InputListener* listener) {
  CHECK(!capture_listener_);
  if (capture_listener_) {
    LOG(ERROR) << "Touch already captured";
  }

  capture_listener_ = listener;
  captured_ = true;
}

void TouchEvent::ReleaseCapture() {
  CHECK(capture_listener_);
  if (!capture_listener_) {
    LOG(ERROR) << "Touch not captured";
  }

  capture_listener_ = 0;
}

void TouchEvent::SendCapturedEvent() {
  if (capture_listener_) {
    std::vector<TouchEvent*> touches;
    touches.push_back(this);

    capture_listener_->OnTouchEvent(touches, 0);
  }
}
