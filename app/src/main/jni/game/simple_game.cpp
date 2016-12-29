////
// simple_game.cpp
////

#include "game/simple_game.h"

#include "base/logging.h"
#include "base/thread/task.h"
#include "base/thread/thread_util.h"
#include "game/core/platform_delegate.h"
#include "game/input/key_event.h"
#include "game/input/keycodes.h"
#include "game/input/touch_event.h"
#include "game/render/basic_texture_shader.h"
#include "game/render/default_focus_render_delegate.h"
#include "game/render/gl.h"
#include "game/ui/focus_util.h"
#include "game/ui/render_state.h"
#include "game/ui/view.h"

namespace {
class InvalidateViewTask : public Task {
 public:
  InvalidateViewTask(PlatformDelegate* platform_delegate, int id)
      : platform_delegate_(platform_delegate), id_(id) {}
  ~InvalidateViewTask() override {}

 private:
  // Task:
  void Execute() override { platform_delegate_->InvalidateView(id_); }

  PlatformDelegate* platform_delegate_;
  int id_;
};
}

SimpleGame::SimpleGame(PlatformDelegate* platform_delegate)
    : platform_delegate_(platform_delegate),
      focused_view_(nullptr),
      width_(0),
      height_(0),
      ui_shader_(new BasicTextureShader),
      focus_render_delegate_(new DefaultFocusRenderDelegate) {
  CHECK_THREAD(thread::Ui);
}

SimpleGame::~SimpleGame() {
  CHECK_THREAD(thread::Ui);
}

void SimpleGame::MoveFocusRight() {
  MoveFocus(kRight);
}

void SimpleGame::MoveFocusLeft() {
  MoveFocus(kLeft);
}

void SimpleGame::MoveFocusUp() {
  MoveFocus(kUp);
}

void SimpleGame::MoveFocusDown() {
  MoveFocus(kDown);
}

ui::View* SimpleGame::GetView() {
  CHECK_THREAD(thread::Ui);
  // Don't need to lock view_ on Ui thread.
  return view_.get();
}

ui::View* SimpleGame::GetViewAt(int x, int y) {
  CHECK_THREAD(thread::Ui);
  // Don't need to lock view_ on Ui thread.
  if (view_) {
    ui::View* view = view_->GetViewAt(x, y);
    if (view) {
      return view;
    }
  }
  return nullptr;
}

ui::View* SimpleGame::FindViewById(int id) {
  CHECK_THREAD(thread::Ui);
  // Don't need to lock view_ on Ui thread.
  if (view_) {
    return view_->FindViewById(id);
  }
  return nullptr;
}

bool SimpleGame::OnKeyDown(int key_code) {
  CHECK_THREAD(thread::Ui);
  return HandleKeyEvent(KeyEvent(KeyEvent::kKeyDown, "", key_code));
}

bool SimpleGame::OnKeyUp(int key_code) {
  CHECK_THREAD(thread::Ui);
  return HandleKeyEvent(KeyEvent(KeyEvent::kKeyUp, "", key_code));
}

void SimpleGame::OnTouchStart(int id, int x, int y, const Timestamp& time) {
  CHECK_THREAD(thread::Ui);
  // Don't need to lock view_ on Ui thread.
  auto ix = touches_.find(id);
  if (ix != touches_.end()) {
    LOG(WARNING) << "Touch with id " << id << " already exists";
    touches_.erase(ix);
  }

  std::unique_ptr<TouchEvent> touch(new TouchEvent(id));
  TouchEvent* touch_ptr = touch.get();
  touch->SetState(TouchEvent::kTouchStart);
  touch->AddPoint(x, y, time);
  touches_[id] = std::move(touch);

  SendTouchEvents(touch_ptr);
}

void SimpleGame::OnTouchMove(int id, int x, int y, const Timestamp& time) {
  CHECK_THREAD(thread::Ui);
  // Don't need to lock view_ on Ui thread.
  auto ix = touches_.find(id);
  if (ix == touches_.end()) {
    OnTouchStart(id, x, y, time);
    return;
  }

  TouchEvent* touch = ix->second.get();
  touch->SetState(TouchEvent::kTouchMove);
  touch->AddPoint(x, y, time);

  SendTouchEvents(touch);
}

void SimpleGame::OnTouchEnd(int id, int x, int y, const Timestamp& time) {
  CHECK_THREAD(thread::Ui);
  // Don't need to lock view_ on Ui thread.
  auto ix = touches_.find(id);
  if (ix == touches_.end()) {
    LOG(WARNING) << "Touch with id " << id << " missing";
    return;
  }

  TouchEvent* touch = ix->second.get();
  touch->SetState(TouchEvent::kTouchEnd);
  touch->AddPoint(x, y, time);

  SendTouchEvents(touch);

  touches_.erase(id);
}

void SimpleGame::OnRenderInit() {
  CHECK_THREAD(thread::Render);
  current_time_ = Timestamp::Now();
  ui_shader_->Load();
  focus_render_delegate_->Init(current_time_);

  // Set up some good defaults.
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void SimpleGame::OnResize(int width, int height) {
  CHECK_THREAD(thread::Render);
  width_ = width;
  height_ = height;

  {
    AutoLock lock(&view_lock_);
    if (view_)
      view_->RequestLayout();
  }

  glViewport(0, 0, width_, height_);
  ui_projection_matrix_.OrthoProjection(0, width_, height_, 0, -1000, 1000);
  focus_render_delegate_->OnSize(width_, height_);
}

void SimpleGame::OnRender() {
  CHECK_THREAD(thread::Render);
  current_time_ = Timestamp::Now();
  glClearColor(0, 0, 0, 1);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // Reset the shader.
  glUseProgram(0);

  AutoLock lock(&view_lock_);
  if (view_ && view_->visible()) {
    math::Rect bounds = math::Rect::MakeXYWH(0, 0, width_, height_);
    if (view_->need_layout()) {
      view_->Layout(bounds);
      OnLayoutView(view_.get());
    }

    ui_shader_->Use();

    // Setup the uniform matricies
    ui_shader_->SetMVPMatrix(ui_projection_matrix_);

    ui::RenderState render_state(current_time_, ui_shader_.get(), bounds);
    view_->Render(&render_state);

    if (focused_view_) {
      focus_render_delegate_->Render(&render_state, focused_view_);
    }
  }
}

// protected:
void SimpleGame::SetView(std::unique_ptr<ui::View> view) {
  CHECK_THREAD(thread::Ui);
  {
    AutoLock lock(&view_lock_);
    SetFocus(nullptr);
    view_ = std::move(view);
    if (view_)
      view_->SetRootView(this);
  }

  platform_delegate_->InvalidateRootView();

  if (view_)
    platform_delegate_->HandleViewChanged(view_->id());
}

// private:
void SimpleGame::SendTouchEvents(TouchEvent* current_event) {
  if (current_event->captured()) {
    current_event->SendCapturedEvent();
    return;
  }

  std::vector<TouchEvent*> touches;
  int current_index = -1;
  for (auto& ix : touches_) {
    if (ix.second.get() == current_event) {
      current_index = (int)touches.size();
    }
    if (!ix.second->captured())
      touches.push_back(ix.second.get());
  }
  if (current_index == -1) {
    LOG(ERROR) << "Missing touch when sending events";
    return;
  }

  const math::Point& current_location =
      current_event->touches().back().position;
  // Don't need to lock view_ on Ui thread.
  if (view_ && view_->Contains(current_location) &&
      view_->OnTouchEvent(touches, current_index)) {
    return;
  }
}

bool SimpleGame::HandleKeyEvent(const KeyEvent& event) {
  if (focused_view_) {
    if (focused_view_->OnKeyEvent(event) || HandleKeyEventForFocus(event)) {
      return true;
    }
  }
  // Don't need to lock view_ on Ui thread.
  if (view_ && view_->OnKeyEvent(event)) {
    return true;
  }

  return HandleKeyEventForFocus(event);
}

bool SimpleGame::HandleKeyEventForFocus(const KeyEvent& event) {
  switch (event.key_code()) {
    case VKEY_RIGHT:
      if (event.type() == KeyEvent::kKeyDown) {
        MoveFocus(kRight);
      }
      return true;

    case VKEY_LEFT:
      if (event.type() == KeyEvent::kKeyDown) {
        MoveFocus(kLeft);
      }
      return true;

    case VKEY_UP:
      if (event.type() == KeyEvent::kKeyDown) {
        MoveFocus(kUp);
      }
      return true;

    case VKEY_DOWN:
      if (event.type() == KeyEvent::kKeyDown) {
        MoveFocus(kDown);
      }
      return true;
  }
  return false;
}

void SimpleGame::MoveFocus(FocusDirection direction) {
  if (!view_)
    return;

  if (focused_view_ && focused_view_->visible()) {
    switch (direction) {
      case kRight:
        SetFocus(ui::FindNextFocusRight(focused_view_));
        break;
      case kLeft:
        SetFocus(ui::FindNextFocusLeft(focused_view_));
        break;
      case kUp:
        SetFocus(ui::FindNextFocusUp(focused_view_));
        break;
      case kDown:
        SetFocus(ui::FindNextFocusDown(focused_view_));
        break;
    }
  } else {
    SetFocus(ui::FindFirstFocus(view_.get()));
  }
}

void SimpleGame::SetFocus(ui::View* view) {
  if (focused_view_)
    focused_view_->SetFocused(false);

  if (view) {
    DCHECK(view->IsFocusable());
    view->SetFocused(true);
  }

  focused_view_ = view;
}

// ui::RootView:
void SimpleGame::PostUiTask(std::unique_ptr<Task> task) {
  CHECK_THREAD(thread::Ui);
  platform_delegate_->PostNativeUiTask(std::move(task), TimeInterval());
}

void SimpleGame::PostUiTaskDelayed(std::unique_ptr<Task> task,
                                   const TimeInterval& delay) {
  CHECK_THREAD(thread::Ui);
  platform_delegate_->PostNativeUiTask(std::move(task), delay);
}

bool SimpleGame::CaptureMouse(InputListener* listener) {
  // No mouse.
  return false;
}

void SimpleGame::ReleaseMouse(InputListener* listener) {
  // No mouse.
}

void SimpleGame::OnRemoveView(ui::View* view) {
  CHECK_THREAD(thread::Render);
  AutoLock lock(&view_lock_);
  if (focused_view_ == view) {
    SetFocus(nullptr);
  }
}

void SimpleGame::OnLayoutView(ui::View* view) {
  CHECK_THREAD(thread::Render);
  platform_delegate_->PostNativeUiTask(
      std::make_unique<InvalidateViewTask>(platform_delegate_, view->id()),
      TimeInterval());
}

void SimpleGame::OnTextChanged(ui::View* view) {
  platform_delegate_->HandleTextChanged(view->id());
}

void SimpleGame::AccessibilityAnnounce(const std::string& text) {
  platform_delegate_->AccessibilityAnnounce(text);
}
