////
// touch_event.h
////

#pragma once

#include "base/macros.h"
#include "base/math/point.h"
#include "base/time.h"

#include <vector>

class InputListener;

class TouchEvent {
 public:
  enum State {
    kTouchStart,
    kTouchMove,
    kTouchEnd,
  };

  struct Touch {
    Touch(math::Point in_position, const Timestamp& in_time);

    math::Point position;
    Timestamp time;
  };

  TouchEvent(long id);
  ~TouchEvent();
  DISALLOW_COPY_AND_ASSIGN(TouchEvent);

  long id() const { return id_; }
  const std::vector<Touch>& touches() const { return touches_; }
  const State state() const { return state_; }

  void SetState(State new_state);
  void AddPoint(int x, int y, const Timestamp& time);

  void Capture(InputListener* listener);
  void ReleaseCapture();
  void SendCapturedEvent();

  bool captured() const { return captured_; }

  TimeInterval Duration() const {
    return touches_.back().time - touches_.front().time;
  }

 private:
  long id_;
  std::vector<Touch> touches_;
  State state_;

  InputListener* capture_listener_;
  bool captured_;
};
