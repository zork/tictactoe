////
// button.cpp
////

#include "game/ui/button.h"

#include "base/thread/task.h"
#include "base/thread/thread_util.h"
#include "game/input/key_event.h"
#include "game/input/keycodes.h"
#include "game/input/mouse_event.h"
#include "game/input/touch_event.h"
#include "game/ui/accessibility_action.h"
#include "game/ui/accessibility_info.h"
#include "game/ui/render_state.h"
#include "game/ui/root_view.h"

namespace ui {

class Button::ButtonClickTask : public Task {
 public:
  ButtonClickTask(Button* button) : button_(button) {}
  void Cancel() { button_ = nullptr; }

 private:
  void Execute() override {
    if (button_)
      button_->Click();
  }

  Button* button_;
};

Button::Button()
    : button_listener_(nullptr),
      captured_touch_(nullptr),
      button_click_task_(nullptr),
      enabled_(true),
      touch_hovered_(false) {
  SetTextHAlign(kHAlignCenter);
  SetTextVAlign(kVAlignCenter);
}

Button::~Button() {
  CHECK_THREAD(thread::Ui);
  if (button_click_task_) {
    button_click_task_->Cancel();
  }
  if (captured_touch_) {
    captured_touch_->ReleaseCapture();
  }
}

void Button::Click() {
  CHECK_THREAD(thread::Ui);
  if (button_listener_)
    button_listener_->OnClick(this);
}

// private:
void Button::PostClickTask() {
  DCHECK(root_view());
  root_view()->PostUiTask(std::make_unique<ButtonClickTask>(this));
}

// View:
void Button::GetAccessibilityInfo(ui::AccessibilityInfo* info) {
  Label::GetAccessibilityInfo(info);
  info->role = ui::AccessibilityInfo::ROLE_BUTTON;
}

void Button::SendAccessibilityAction(const ui::AccessibilityAction& action) {
  if (action.action == ui::AccessibilityAction::ACTION_CLICK) {
    PostClickTask();
  }
}

void Button::RenderInternal(RenderState* render_state) {
  if (!enabled_) {
    render_state->PushUiColor(1, 1, 1, .5);
  } else if (touch_hovered_) {
    render_state->PushUiColor(.5, .5, .5, 1);
  }
  Label::RenderInternal(render_state);
  if (touch_hovered_ || !enabled_) {
    render_state->PopUiColor();
  }
}

// InputListener:
bool Button::OnKeyEvent(const KeyEvent& event) {
  if (event.key_code() == VKEY_RETURN || event.key_code() == VKEY_SELECT) {
    if (event.type() == KeyEvent::kKeyDown) {
      PostClickTask();
    }
    return true;
  }
  return false;
}

bool Button::OnMouseEvent(const MouseEvent& event) {
  // TODO: Proper mouse handling, with hover and whatnot
  if (!enabled_) {
    return false;
  }

  if (event.type() == MouseEvent::kMouseUp &&
      event.button() == MouseEvent::kButtonLeft) {
    PostClickTask();
  }
  return true;
}

bool Button::OnTouchEvent(std::vector<TouchEvent*>& touches, int index) {
  TouchEvent* current_touch = touches[index];

  if (!enabled_) {
    if (captured_touch_) {
      bool was_captured = (captured_touch_ == current_touch);
      captured_touch_->ReleaseCapture();
      captured_touch_ = nullptr;
      if (was_captured)
        return true;
    }

    return false;
  }

  if (current_touch == captured_touch_) {
    const math::Point& position = current_touch->touches().back().position;
    touch_hovered_ = Contains(position);

    if (current_touch->state() == TouchEvent::kTouchEnd) {
      if (touch_hovered_) {
        PostClickTask();
      }

      touch_hovered_ = false;
      captured_touch_->ReleaseCapture();
      captured_touch_ = nullptr;

      return true;
    }
  } else if (!captured_touch_) {
    captured_touch_ = current_touch;
    touch_hovered_ = true;
    current_touch->Capture(this);
    return true;
  }

  return SendTouchEventsToChildren(touches, index);
}

void Button::OnTouchDelete(TouchEvent* touch) {
  if (touch == captured_touch_) {
    captured_touch_->ReleaseCapture();
    captured_touch_ = nullptr;
  }
}

}  // namespace ui
