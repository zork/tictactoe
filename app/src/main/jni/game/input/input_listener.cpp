////
// input_listener.cpp
////

#include "game/input/input_listener.h"

#include "base/logging.h"
#include "game/input/gesture.h"

InputListener::InputListener() {}

InputListener::~InputListener() {}

bool InputListener::OnKeyEvent(const KeyEvent& event) {
  return false;
}

bool InputListener::OnMouseEvent(const MouseEvent& event) {
  return false;
}

void InputListener::OnMouseLockLost() {}

bool InputListener::OnTouchEvent(std::vector<TouchEvent*>& touches, int index) {
  return false;
}

void InputListener::OnTouchDelete(TouchEvent* touch) {
  LOG(WARNING) << "Touch is deleted, but event capture doesn't listen";
}

void InputListener::AddGesture(std::unique_ptr<Gesture> gesture) {
  gestures_.push_back(std::move(gesture));
}

// protected:
bool InputListener::SendMouseEventToGestures(const MouseEvent& event) {
  for (const auto& gesture : gestures_) {
    if (gesture->OnMouseEvent(event)) {
      return true;
    }
  }
  return false;
}

bool InputListener::SendTouchEventsToGestures(std::vector<TouchEvent*>& touches,
                                              int index) {
  for (const auto& gesture : gestures_) {
    if (gesture->OnTouchEvent(touches, index)) {
      return true;
    }
  }
  return false;
}
