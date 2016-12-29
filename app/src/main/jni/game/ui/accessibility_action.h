////
// accessibility_action.h
////

#pragma once

namespace ui {

struct AccessibilityAction {
  enum Action {
    ACTION_NONE,
    ACTION_CLICK,
  };
  AccessibilityAction();
  ~AccessibilityAction();

  Action action;
};

}  // namespace ui
