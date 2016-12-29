////
// view.h
////

#pragma once

#include "base/macros.h"
#include "base/math/point.h"
#include "base/math/rect.h"
#include "base/thread/mutex.h"
#include "game/input/input_listener.h"
#include "game/ui/accessibility_info.h"

#include <string>

namespace ui {

struct AccessibilityAction;
class RenderState;
class RootView;
class UiTexture;

class View : public InputListener {
 public:
  enum HAlign {
    kHAlignLeft = 0,
    kHAlignRight = 1,
    kHAlignCenter = 2,
  };
  enum VAlign {
    kVAlignTop = 0,
    kVAlignBottom = 1,
    kVAlignCenter = 2,
  };
  static int AllocateUniqueId();

  View();
  ~View() override;
  DISALLOW_COPY_AND_ASSIGN(View);

  // View information.
  int id() const { return id_; }

  void SetName(const std::string& name) { name_ = name; }
  const std::string& name() const { return name_; }

  void SetTitle(const std::string& title) { title_ = title; }
  const std::string& title() const { return title_; }

  void SetTag(long tag) { tag_ = tag; }
  long tag() const { return tag_; }

  void SetLayoutX(int x) { layout_x_ = x; }
  void SetLayoutY(int y) { layout_y_ = y; }

  void SetLayoutWidth(int width) { layout_width_ = width; }
  int layout_width() const { return layout_width_; }

  void SetLayoutHeight(int height) { layout_height_ = height; }
  int layout_height() const { return layout_height_; }

  void SetLayoutHAlign(HAlign align) { layout_halign_ = align; }
  void SetLayoutVAlign(VAlign align) { layout_valign_ = align; }
  bool need_layout() { return need_layout_; }

  void SetVisible(bool visible);
  bool visible() const { return visible_; }

  bool focused() const { return focused_; }

  const math::Rect& bounds() const { return bounds_; }

  void SetAccessibilityLabel(const std::string& value);
  std::string GetAccessibilityLabel();

  void SetAccessibilityLive(AccessibilityLive live) {
    accessibility_live_ = live;
  }

  // Accessibility accessors.
  void GetVisibleChildIds(std::vector<int>* ids);
  virtual void GetAccessibilityInfo(AccessibilityInfo* info);
  virtual void SendAccessibilityAction(const AccessibilityAction& action);

  // Hit checking.
  View* GetViewAt(int x, int y);
  bool Contains(const math::Point& p) const {
    return bounds_.Contains(p.x(), p.y());
  }
  bool Contains(int x, int y) const { return bounds_.Contains(x, y); }

  // Hierarchy inspection.  Only use on the UI thread, do not modify.
  View* parent() const { return parent_; }
  const std::vector<std::unique_ptr<View>>& children() const {
    return children_;
  }

  // View management
  View* FindViewByName(const std::string& name);
  View* FindViewById(int id);
  void AddView(std::unique_ptr<View> view);
  std::unique_ptr<View> RemoveView();
  void RemoveAllViews();
  void RequestLayout();

  // Focus management.
  virtual bool IsFocusable() const { return false; }

  // Should only be called by RootView.
  virtual void Measure(int* width, int* height);
  void Layout(const math::Rect& parent_bounds);
  void Render(RenderState* render_state);
  void SetRootView(RootView* root_view);
  void SetFocused(bool focused);

  // InputListener:
  bool OnMouseEvent(const MouseEvent& event) override;
  bool OnTouchEvent(std::vector<TouchEvent*>& touches, int index) override;

 protected:
  RootView* root_view() { return root_view_; }

  void SetIsRelativeLayout(bool value) { is_relative_layout_ = value; }

  static void DrawImage(RenderState* render_state,
                        UiTexture* texture,
                        const math::Rect& bounds);

  virtual void GetAccessibilityInfoForChild(View* child,
                                            AccessibilityInfo* info);
  virtual void LayoutChildren();
  virtual void RenderInternal(RenderState* render_state);

  void RenderChildren(RenderState* render_state);
  bool SendMouseEventToChildren(const MouseEvent& event);
  bool SendTouchEventsToChildren(std::vector<TouchEvent*>& touches, int index);
  std::unique_ptr<View> RemoveView(View* view);

 private:
  View* FindViewByNameInChildren(const std::string& name);
  View* FindViewByIdInChildren(int id);
  void OnRemove();

  const int id_;
  std::string name_;
  std::string title_;
  long tag_;

  bool is_relative_layout_;
  int layout_x_;
  int layout_y_;
  int layout_width_;
  int layout_height_;
  HAlign layout_halign_;
  VAlign layout_valign_;

  bool visible_;
  bool focused_;
  math::Rect bounds_;
  bool need_layout_;

  Mutex accessibility_lock_;
  std::string accessibility_label_;
  AccessibilityLive accessibility_live_;

  RootView* root_view_;
  View* parent_;

 protected:
  Mutex children_lock_;
  std::vector<std::unique_ptr<View>> children_;
};

}  // namespace ui
