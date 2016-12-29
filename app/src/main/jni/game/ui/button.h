////
// button.h
////

#pragma once

#include "base/macros.h"
#include "game/ui/label.h"

namespace ui {

class Button : public Label {
 public:
  class Listener {
   public:
    virtual ~Listener() {}
    virtual void OnClick(Button* button) = 0;
  };

  Button();
  ~Button() override;
  DISALLOW_COPY_AND_ASSIGN(Button);

  void SetButtonListener(Listener* listener) { button_listener_ = listener; }
  void SetEnabled(bool enabled) { enabled_ = enabled; }

  virtual void Click();

 private:
  class ButtonClickTask;

  void PostClickTask();

  // View:
  void GetAccessibilityInfo(ui::AccessibilityInfo* info) override;
  void SendAccessibilityAction(const ui::AccessibilityAction& action) override;
  bool IsFocusable() const override { return enabled_; }
  void RenderInternal(RenderState* render_state) override;

  // InputListener:
  bool OnKeyEvent(const KeyEvent& event) override;
  bool OnMouseEvent(const MouseEvent& event) override;
  bool OnTouchEvent(std::vector<TouchEvent*>& touches, int index) override;
  void OnTouchDelete(TouchEvent* touch) override;

  Listener* button_listener_;
  TouchEvent* captured_touch_;
  ButtonClickTask* button_click_task_;

  bool enabled_;
  bool touch_hovered_;
};

}  // namespace ui
