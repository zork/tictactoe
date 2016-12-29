////
// platform_delegate.h
////

#pragma once

#include "base/time.h"

#include <memory>
#include <string>

class Task;

class PlatformDelegate {
 public:
  virtual ~PlatformDelegate() {}

  // Accessibility
  // Top level view hierachry has changed.
  virtual void InvalidateRootView() {}

  // View's layout has changed.
  virtual void InvalidateView(int id) {}

  // Top level view has changed.
  virtual void HandleViewChanged(int id) {}

  // Text in a view has changed.
  virtual void HandleTextChanged(int view_id) {}

  // Announce the string to screen reader users.
  virtual void AccessibilityAnnounce(const std::string& text) {}

  // Post a task to the native message loop.
  virtual void PostNativeUiTask(std::unique_ptr<Task> task,
                                const TimeInterval& delay) = 0;
};
