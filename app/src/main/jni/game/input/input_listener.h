////
// input_listener.h
////

#pragma once

#include "base/macros.h"

#include <memory>
#include <vector>

class Gesture;
class KeyEvent;
class MouseEvent;
class TouchEvent;

class InputListener {
 public:
  InputListener();
  virtual ~InputListener();
  DISALLOW_COPY_AND_ASSIGN(InputListener);

  virtual bool OnKeyEvent(const KeyEvent& event);
  virtual bool OnMouseEvent(const MouseEvent& event);
  virtual void OnMouseLockLost();
  virtual bool OnTouchEvent(std::vector<TouchEvent*>& touches, int index);
  virtual void OnTouchDelete(TouchEvent* touch);

  void AddGesture(std::unique_ptr<Gesture> gesture);

 protected:
  bool SendMouseEventToGestures(const MouseEvent& event);
  bool SendTouchEventsToGestures(std::vector<TouchEvent*>& touches, int index);

 private:
  std::vector<std::unique_ptr<Gesture>> gestures_;
};
