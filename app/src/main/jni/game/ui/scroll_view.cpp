////
// scroll_view.cpp
////

#include "game/ui/scroll_view.h"

#include "base/logging.h"
#include "base/math/math.h"
#include "game/input/mouse_event.h"
#include "game/render/gl.h"
#include "game/ui/render_state.h"

#include <algorithm>

namespace ui {

ScrollView::ScrollView() {
  AddGesture(std::make_unique<SlideGesture>(this));
}

ScrollView::~ScrollView() {}

math::Point ScrollView::GetScrollPosition() {
  AutoLock lock(&scroll_lock_);
  return scroll_position_;
}

void ScrollView::SetScrollPosition(const math::Point& scroll_position) {
  {
    AutoLock lock(&scroll_lock_);
    scroll_position_ = scroll_position;
  }
  RequestLayout();
}

// private:
// InputListener:
bool ScrollView::OnMouseEvent(const MouseEvent& event) {
  if (event.type() == MouseEvent::kMouseScroll) {
    {
      AutoLock lock(&scroll_lock_);
      scroll_position_ -= event.delta();
    }
    RequestLayout();
    return true;
  } else {
    return View::OnMouseEvent(event);
  }
}

// SlideGesture::Listener:
void ScrollView::OnSlide(SlideGesture* gesture, TouchEvent::State state) {
  {
    AutoLock lock(&scroll_lock_);
    scroll_position_ -= math::Point(gesture->delta_x(), gesture->delta_y());
  }
  RequestLayout();
}

// View:
void ScrollView::LayoutChildren() {
  AutoLock lock(&children_lock_);
  if (children_.empty())
    return;

  int width;
  int height;
  children_[0]->Measure(&width, &height);
  int max_x = std::max(width - bounds().width(), 0);
  int max_y = std::max(height - bounds().height(), 0);

  math::Point offset;
  {
    AutoLock lock(&scroll_lock_);
    offset.SetX(math::Clamp((int)scroll_position_.x(), 0, max_x));
    offset.SetY(math::Clamp((int)scroll_position_.y(), 0, max_y));
    scroll_position_ = offset;
  }

  math::Rect client_bounds = math::Rect::MakeXYWH(
      bounds().x() - offset.x(), bounds().y() - offset.y(), width, height);

  for (const auto& child : children_) {
    if (child->visible()) {
      child->Layout(client_bounds);
    }
  }
}

void ScrollView::RenderInternal(RenderState* render_state) {
  glEnable(GL_SCISSOR_TEST);
  glScissor(bounds().x(),
            (render_state->bounds().height() - (bounds().bottom())),
            bounds().width(), bounds().height());

  AutoLock lock(&children_lock_);
  for (const auto& child : children_) {
    if (child->visible() && bounds().Intersects(child->bounds())) {
      child->Render(render_state);
    }
  }

  glDisable(GL_SCISSOR_TEST);
}

}  // namespace ui
