////
// focus_render_delegate.h
////

#pragma once

#include "base/time.h"

namespace ui {

class RenderState;
class View;

class FocusRenderDelegate {
 public:
  virtual ~FocusRenderDelegate() {}

  // Allocate rendering resources.  Called on the Render thread.
  virtual void Init(const Timestamp& time) {}

  // Set the size of the view.  Called on the Render thread.
  virtual void OnSize(int width, int height) {}

  // Render a single frame.  Called on the Render thread.
  virtual void Render(RenderState* render_state, View* view) = 0;
};

}  // namespace ui
