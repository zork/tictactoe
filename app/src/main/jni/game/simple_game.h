////
// simple_game.h
////

#pragma once

#include "base/macros.h"
#include "base/math/matrix.h"
#include "base/thread/mutex.h"
#include "base/time.h"
#include "game/ui/root_view.h"

#include <map>
#include <memory>

class BasicTextureShader;
class KeyEvent;
class PlatformDelegate;
class TouchEvent;

namespace ui {
class FocusRenderDelegate;
class View;
}

class SimpleGame : ui::RootView {
 public:
  static std::unique_ptr<SimpleGame> Create(
      PlatformDelegate* platform_delegate);

  SimpleGame(PlatformDelegate* platform_delegate);
  ~SimpleGame() override;
  DISALLOW_COPY_AND_ASSIGN(SimpleGame);

  // Move focus.  Called on any thread.
  void MoveFocusRight();
  void MoveFocusLeft();
  void MoveFocusUp();
  void MoveFocusDown();

  ui::View* GetView();
  ui::View* GetViewAt(int x, int y);
  ui::View* FindViewById(int id);

  // App lifetime events.
  virtual void OnCreate() {}
  virtual void OnPause() {}
  virtual void OnResume() {}
  virtual void OnDestroy() {}

  // Keyboard events.  Must be called on UI thread.
  bool OnKeyDown(int key_code);
  bool OnKeyUp(int key_code);

  // Touch events.  Must be called on UI thread.
  void OnTouchStart(int id, int x, int y, const Timestamp& time);
  void OnTouchMove(int id, int x, int y, const Timestamp& time);
  void OnTouchEnd(int id, int x, int y, const Timestamp& time);

  // Initialize render resources.  Must be called on the Render thread.
  void OnRenderInit();

  // Called when the dimensions of the root view changes.  Must be called on the
  // Render thread.
  void OnResize(int width, int height);

  // Render a single frame.  Must be called on the Render thread.
  void OnRender();

 protected:
  void SetView(std::unique_ptr<ui::View> view);

 private:
  enum FocusDirection {
    kRight,
    kLeft,
    kUp,
    kDown,
  };

  void SendTouchEvents(TouchEvent* current_event);
  bool HandleKeyEvent(const KeyEvent& event);
  bool HandleKeyEventForFocus(const KeyEvent& event);
  void MoveFocus(FocusDirection direction);
  void SetFocus(ui::View* view);

  // ui::RootView:
  void PostUiTask(std::unique_ptr<Task> task) override;
  void PostUiTaskDelayed(std::unique_ptr<Task> task,
                         const TimeInterval& delay) override;
  bool CaptureMouse(InputListener* listener) override;
  void ReleaseMouse(InputListener* listener) override;
  void OnRemoveView(ui::View* view) override;
  void OnLayoutView(ui::View* view) override;
  void OnTextChanged(ui::View* view) override;
  void AccessibilityAnnounce(const std::string& text) override;

  PlatformDelegate* platform_delegate_;

  Mutex view_lock_;
  std::unique_ptr<ui::View> view_;

  ui::View* focused_view_;
  int width_;
  int height_;
  Timestamp current_time_;
  std::unique_ptr<BasicTextureShader> ui_shader_;
  std::unique_ptr<ui::FocusRenderDelegate> focus_render_delegate_;

  Matrix ui_projection_matrix_;

  std::map<long, std::unique_ptr<TouchEvent>> touches_;
};
