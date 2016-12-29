////
// root_view.h
////

#pragma once

#include "base/time.h"

#include <memory>

class InputListener;
class Task;

namespace ui {

class View;

class RootView {
 public:
  virtual ~RootView() {}
  // Called on UI thread.
  virtual void PostUiTask(std::unique_ptr<Task> task) = 0;
  virtual void PostUiTaskDelayed(std::unique_ptr<Task> task,
                                 const TimeInterval& delay) = 0;
  virtual bool CaptureMouse(InputListener* listener) = 0;
  virtual void ReleaseMouse(InputListener* listener) = 0;

  // Called on Render thread.
  virtual void OnRemoveView(ui::View* view) = 0;
  virtual void OnLayoutView(ui::View* view) = 0;

  // Called on any thread.
  virtual void OnTextChanged(ui::View* view) = 0;
  virtual void AccessibilityAnnounce(const std::string& text) = 0;
};

}  // namespace ui
