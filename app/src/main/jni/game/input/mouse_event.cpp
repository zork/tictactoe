////
// mouse_event.cpp
////

#include "game/input/mouse_event.h"

#include "base/basic_types.h"

MouseEvent::MouseEvent(Type type,
                       Button button,
                       const math::Point& position,
                       const math::Point& delta,
                       const Timestamp& time)
    : type_(type),
      button_(button),
      position_(position),
      delta_(delta),
      time_(time) {}

MouseEvent::~MouseEvent() {}
