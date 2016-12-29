////
// mouse_event.h
////

#pragma once

#include "base/math/point.h"
#include "base/time.h"

class InputListener;

class MouseEvent {
 public:
  enum Type {
    kMouseDown,
    kMouseUp,
    kMouseMove,
    kMouseEnter,
    kMouseLeave,
    kMouseScroll,
  };

  enum Button {
    kButtonNone,
    kButtonLeft,
    kButtonRight,
    kButtonMiddle,
  };

  MouseEvent(Type type,
             Button button,
             const math::Point& position,
             const math::Point& delta,
             const Timestamp& time);
  ~MouseEvent();

  Type type() const { return type_; }
  Button button() const { return button_; }
  const math::Point& position() const { return position_; }
  const math::Point& delta() const { return delta_; }
  const Timestamp& time() const { return time_; }

 private:
  Type type_;
  Button button_;
  math::Point position_;
  math::Point delta_;
  Timestamp time_;
};
