////
// view.cpp
////

#include "game/ui/view.h"

#include "base/memory/ptr_util.h"
#include "base/thread/thread_util.h"
#include "game/input/mouse_event.h"
#include "game/input/touch_event.h"
#include "game/render/basic_texture_shader.h"
#include "game/ui/accessibility_info.h"
#include "game/ui/render_state.h"
#include "game/ui/root_view.h"
#include "game/ui/ui_texture.h"

#include <algorithm>
#include <atomic>

namespace {
std::atomic<int> g_next_unique_id(1);
}

namespace ui {

// static
int View::AllocateUniqueId() {
  return g_next_unique_id++;
}

View::View()
    : id_(AllocateUniqueId()),
      tag_(0),
      is_relative_layout_(false),
      layout_x_(0),
      layout_y_(0),
      layout_width_(-1),
      layout_height_(-1),
      layout_halign_(kHAlignCenter),
      layout_valign_(kVAlignCenter),
      visible_(true),
      focused_(false),
      need_layout_(true),
      accessibility_live_(kAccessibilityLiveNone),
      root_view_(nullptr),
      parent_(nullptr) {}

View::~View() {}

void View::SetVisible(bool visible) {
  visible_ = visible;
  RequestLayout();
}

void View::SetAccessibilityLabel(const std::string& value) {
  {
    AutoLock lock(&accessibility_lock_);
    accessibility_label_ = value;
  }
  if (root_view())
    root_view()->OnTextChanged(this);
}

std::string View::GetAccessibilityLabel() {
  AutoLock lock(&accessibility_lock_);
  return accessibility_label_;
}

void View::GetVisibleChildIds(std::vector<int>* ids) {
  CHECK_THREAD(thread::Ui);
  // Don't need to lock |children_| on UI thread
  for (const auto& child : children_) {
    // TODO: accessibility_hidden_
    if (child->visible())
      ids->push_back(child->id_);
  }
}

void View::GetAccessibilityInfo(AccessibilityInfo* info) {
  CHECK_THREAD(thread::Ui);
  if (parent_) {
    math::Rect parent_bounds = parent_->bounds_;
    info->bounds_in_parent = math::Rect::MakeXYWH(
        bounds_.x() - parent_bounds.x(), bounds_.y() - parent_bounds.y(),
        bounds_.width(), bounds_.height());
  } else {
    info->bounds_in_parent = bounds_;
  }
  info->text = GetAccessibilityLabel();
  info->live = accessibility_live_;
  if (parent_) {
    parent_->GetAccessibilityInfoForChild(this, info);
  }
}

void View::SendAccessibilityAction(const AccessibilityAction& action) {}

View* View::GetViewAt(int x, int y) {
  CHECK_THREAD(thread::Ui);
  // TODO: accessibility_hidden_
  if (!visible() || !Contains(x, y))
    return nullptr;

  {
    // Don't need to lock |children_| on UI thread
    for (const auto& child : children_) {
      View* view = child->GetViewAt(x, y);
      if (view)
        return view;
    }
  }
  return this;
}

View* View::FindViewByName(const std::string& name) {
  if (name_ == name)
    return this;
  else
    return FindViewByNameInChildren(name);
}

View* View::FindViewById(int id) {
  if (id_ == id)
    return this;
  else
    return FindViewByIdInChildren(id);
}

void View::AddView(std::unique_ptr<View> view) {
  DCHECK(!root_view_ || thread::CurrentlyOn(thread::Ui));
  view->need_layout_ = true;
  view->parent_ = this;
  view->SetRootView(root_view_);
  // We don't need to lock |children_| here, because it's either the UI thread,
  // or this view isn't being accessed by multiple threads yet.
  children_.push_back(std::move(view));
  if (is_relative_layout_)
    need_layout_ = true;
}

std::unique_ptr<View> View::RemoveView() {
  CHECK_THREAD(thread::Ui);
  if (parent_)
    return parent_->RemoveView(this);

  LOG(ERROR) << "Attemping to remove view without parent: " << name_;
  return WrapUnique(this);
}

void View::RemoveAllViews() {
  CHECK_THREAD(thread::Ui);
  {
    std::vector<std::unique_ptr<View>> to_remove;
    {
      AutoLock lock(&children_lock_);
      to_remove.swap(children_);
    }
    for (const auto& child : to_remove) {
      child->parent_ = nullptr;
      child->OnRemove();
    }
  }
}

void View::RequestLayout() {
  need_layout_ = true;
  if (parent_ && parent_->is_relative_layout_) {
    parent_->RequestLayout();
  }
}

void View::Measure(int* width, int* height) {
  CHECK_THREAD(thread::Render);
  *width = layout_width_;
  *height = layout_height_;
}

void View::Layout(const math::Rect& parent_bounds) {
  CHECK_THREAD(thread::Render);
  need_layout_ = false;

  int width;
  int height;
  Measure(&width, &height);

  // width of -1 means fill width.
  width = width >= 0 ? width : parent_bounds.width();
  // height of -1 means fill height.
  height = height >= 0 ? height : parent_bounds.height();

  int x = 0;
  int y = 0;
  switch (layout_halign_) {
    case kHAlignLeft:
      // layout_x_ is an offset from the left side.
      x = parent_bounds.x() + layout_x_;
      break;
    case kHAlignRight:
      // layout_x_ is the offset of the right edge from the right bounds.
      x = parent_bounds.right() - width + layout_x_;
      break;
    case kHAlignCenter:
      // layout_x_ is the offset from the center.
      x = parent_bounds.x() + ((parent_bounds.width() - width) / 2) + layout_x_;
      break;
  }

  switch (layout_valign_) {
    case kVAlignTop:
      // layout_y_ is an offset from the top.
      y = parent_bounds.y() + layout_y_;
      break;
    case kVAlignBottom:
      // layout_y_ is the offset of the bottom edge from the bottom of the view.
      y = parent_bounds.bottom() - height + layout_y_;
      break;
    case kVAlignCenter:
      // layout_y_ is the offset from the center.
      y = parent_bounds.y() + ((parent_bounds.height() - height) / 2) +
          layout_y_;
      break;
  }

  bounds_ = math::Rect::MakeXYWH(x, y, width, height);

  LayoutChildren();
}

void View::Render(RenderState* render_state) {
  CHECK_THREAD(thread::Render);
  {
    AutoLock lock(&children_lock_);
    for (const auto& child : children_) {
      if (child->visible_ && child->need_layout_) {
        child->Layout(bounds_);
        root_view_->OnLayoutView(child.get());
      }
    }
  }
  RenderInternal(render_state);
}

void View::SetRootView(RootView* root_view) {
  root_view_ = root_view;
  AutoLock lock(&children_lock_);
  for (const auto& child : children_)
    child->SetRootView(root_view);
}

void View::SetFocused(bool focused) {
  focused_ = focused;
}

// InputListener:
bool View::OnMouseEvent(const MouseEvent& event) {
  CHECK_THREAD(thread::Ui);
  if (SendMouseEventToChildren(event))
    return true;
  else
    return SendMouseEventToGestures(event);
}

bool View::OnTouchEvent(std::vector<TouchEvent*>& touches, int index) {
  CHECK_THREAD(thread::Ui);
  if (SendTouchEventsToChildren(touches, index))
    return true;
  else
    return SendTouchEventsToGestures(touches, index);
}

// protected:
// static
void View::DrawImage(RenderState* render_state,
                     UiTexture* texture,
                     const math::Rect& bounds) {
  if (!texture)
    return;

  texture->UploadTexture();
  if (!texture->IsReady())
    return;

  texture->Bind();

  float x = bounds.x();
  float y = bounds.y();
  float right = bounds.right();
  float top = bounds.top();
  float vertices[] = {
      // (0, 1)
      x, top, 0,

      // (1, 1)
      right, top, 0,

      // (1, 0)
      right, y, 0,

      // (0, 0)
      x, y, 0,
  };

  float textures[] = {
      // (0, 1)
      0, texture->top(),

      // (1, 1)
      texture->right(), texture->top(),

      // (1, 0)
      texture->right(), 0,

      // (0, 0)
      0, 0,
  };

  uint32_t faces[] = {0, 1, 2, 0, 2, 3};
  render_state->shader()->DrawTriangles(vertices, arraysize(vertices), textures,
                                        arraysize(textures), faces,
                                        arraysize(faces));
}

void View::GetAccessibilityInfoForChild(View* child, AccessibilityInfo* info) {}

void View::LayoutChildren() {
  AutoLock lock(&children_lock_);
  for (const auto& child : children_) {
    if (child->visible_) {
      child->Layout(bounds_);
    }
  }
}

void View::RenderInternal(RenderState* render_state) {
  // By default, just render children.
  RenderChildren(render_state);
}

void View::RenderChildren(RenderState* render_state) {
  AutoLock lock(&children_lock_);
  for (const auto& child : children_) {
    if (child->visible()) {
      child->Render(render_state);
    }
  }
}

bool View::SendMouseEventToChildren(const MouseEvent& event) {
  CHECK_THREAD(thread::Ui);
  // Children don't need to be locked on the UI thread.
  for (auto ix = children_.rbegin(); ix != children_.rend(); ++ix) {
    if ((*ix)->visible() && (*ix)->Contains(event.position()) &&
        (*ix)->OnMouseEvent(event)) {
      return true;
    }
  }
  return false;
}

bool View::SendTouchEventsToChildren(std::vector<TouchEvent*>& touches,
                                     int index) {
  CHECK_THREAD(thread::Ui);
  // Children don't need to be locked on the UI thread.
  const math::Point& current_location =
      touches[index]->touches().back().position;
  for (auto ix = children_.rbegin(); ix != children_.rend(); ++ix) {
    if ((*ix)->visible() && (*ix)->Contains(current_location) &&
        (*ix)->OnTouchEvent(touches, index)) {
      return true;
    }
  }
  return false;
}

std::unique_ptr<View> View::RemoveView(View* view) {
  CHECK_THREAD(thread::Ui);
  view->parent_ = nullptr;
  view->OnRemove();

  std::unique_ptr<View> handle;
  {
    AutoLock lock(&children_lock_);
    auto ix = std::find_if(children_.begin(), children_.end(),
                           [&view](const auto& p) { return p.get() == view; });
    if (ix != children_.end()) {
      handle = std::move(*ix);
      children_.erase(ix);
    } else {
      LOG(ERROR) << "Attempt to remove view that's not a child";
    }
  }
  if (is_relative_layout_)
    need_layout_ = true;
  return handle;
}

// private:
View* View::FindViewByNameInChildren(const std::string& name) {
  AutoLock lock(&children_lock_);
  // Check each child first.
  auto ix = std::find_if(children_.begin(), children_.end(),
                         [&name](const auto& p) { return p->name() == name; });
  if (ix != children_.end())
    return ix->get();

  // Recurse into each child.
  for (const auto& child_view : children_) {
    View* view = child_view->FindViewByNameInChildren(name);
    if (view)
      return view;
  }

  return nullptr;
}

View* View::FindViewByIdInChildren(int id) {
  AutoLock lock(&children_lock_);
  // Check each child first.
  auto ix = std::find_if(children_.begin(), children_.end(),
                         [&id](const auto& p) { return p->id() == id; });
  if (ix != children_.end())
    return ix->get();

  // Recurse into each child.
  for (const auto& child_view : children_) {
    View* view = child_view->FindViewByIdInChildren(id);
    if (view)
      return view;
  }

  return nullptr;
}

void View::OnRemove() {
  CHECK_THREAD(thread::Ui);
  AutoLock lock(&children_lock_);
  if (root_view_) {
    root_view_->OnRemoveView(this);
    root_view_ = nullptr;
  }
  for (const auto& child : children_) {
    child->OnRemove();
  }
}

}  // namespace ui
