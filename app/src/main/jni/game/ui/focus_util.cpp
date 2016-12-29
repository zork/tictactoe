////
// focus_util.cpp
////

#include "game/ui/focus_util.h"

#include "base/math/rect.h"
#include "base/thread/thread_util.h"
#include "game/ui/view.h"

#include <algorithm>
#include <list>

namespace ui {

namespace {
typedef bool (*FocusAcceptFunc)(const math::Rect&, const View&);
typedef bool (*FocusOrderFunc)(const View&, const View&);

// Move focus right.
bool FocusAcceptRight(const math::Rect& bounds, const View& view) {
  return view.bounds().x() > bounds.x() &&
         view.bounds().bottom() > bounds.y() &&
         view.bounds().y() < bounds.bottom();
}

bool FocusOrderRight(const View& left, const View& right) {
  if (left.bounds().x() < right.bounds().x()) {
    return true;
    /* TODO
    } else if (left.bounds().x() == right.bounds().x()) {
      if (left.bounds().y() < right.bounds().y()) {
        return true;
      } else if (left.bounds().y() == right.bounds().y()) {
        return left.bounds().right() < right.bounds().right() ||
               (left.bounds().right() == right.bounds().right() &&
                left.bounds().bottom() < right.bounds().bottom());
      }
    */
  }
  return false;
}

// Move focus left.
bool FocusAcceptLeft(const math::Rect& bounds, const View& view) {
  return view.bounds().right() < bounds.right() &&
         view.bounds().bottom() > bounds.y() &&
         view.bounds().y() < bounds.bottom();
}

bool FocusOrderLeft(const View& left, const View& right) {
  if (left.bounds().right() > right.bounds().right()) {
    return true;
    /* TODO
    } else if (left.bounds().right() == right.bounds().right()) {
      if (left.bounds().bottom() > right.bounds().bottom()) {
        return true;
      } else if (left.bounds().bottom() == right.bounds().bottom()) {
        return left.bounds().x() > right.bounds().x() ||
               (left.bounds().x() == right.bounds().x() &&
                left.bounds().top() > right.bounds().top());
      }
    */
  }
  return false;
}

// Move focus up.
bool FocusAcceptUp(const math::Rect& bounds, const View& view) {
  return view.bounds().x() < bounds.right() &&
         view.bounds().right() > bounds.x() &&
         view.bounds().bottom() < bounds.bottom();
}

bool FocusOrderUp(const View& left, const View& right) {
  if (left.bounds().bottom() > right.bounds().bottom()) {
    return true;
  }
  return false;
}

// Move focus down.
bool FocusAcceptDown(const math::Rect& bounds, const View& view) {
  return view.bounds().x() < bounds.right() &&
         view.bounds().right() > bounds.x() && view.bounds().y() > bounds.y();
}

bool FocusOrderDown(const View& left, const View& right) {
  if (left.bounds().y() < right.bounds().y()) {
    return true;
  }
  return false;
}

View* FocusSearch(FocusAcceptFunc accept,
                  FocusOrderFunc compare,
                  View* view,
                  const math::Rect& bounds,
                  bool follow_parent) {
  // Children don't need to be locked on the UI thread.
  CHECK_THREAD(thread::Ui);

  // Ignore non-visible views.
  if (!view->visible())
    return nullptr;

  if (view->IsFocusable()) {
    if (follow_parent && view->parent()) {
      // If we're still walking the tree, and this is focusable, then we should
      // climb the tree.
      if (view->parent()) {
        return FocusSearch(accept, compare, view->parent(), bounds, true);
      }
    } else {
      // If this is a leaf of the search or the top view and this element is
      // focusable, it's a match.
      return view;
    }
  }

  // Order the children, discarding any that fail the comparison with the
  // bounds.
  std::list<View*> views;
  for (const auto& child : view->children()) {
    if (child->visible() && accept(bounds, *child)) {
      // Add the child if it's in the search area.
      auto ix = std::find_if(
          views.begin(), views.end(),
          [&child, &compare](const auto& p) { return compare(*child, *p); });
      views.insert(ix, child.get());
    }
  }

  for (const auto& view : views) {
    if (view->IsFocusable()) {
      // If a view in the accepted children is focusable, it's a match.
      return view;
    } else {
      // Check the children of the view for a match, but do not climb the
      // tree.
      View* focus_view = FocusSearch(accept, compare, view, bounds, false);
      if (focus_view)
        return focus_view;
    }
  }

  if (follow_parent) {
    if (view->parent()) {
      return FocusSearch(accept, compare, view->parent(), bounds, true);
    } else {
      // TODO: Handle wrapping.
      return nullptr;
    }
  } else {
    return nullptr;
  }
}

}  // namespace

View* FindFirstFocus(View* start) {
  CHECK_THREAD(thread::Ui);
  if (!start->visible()) {
    return nullptr;
  }

  if (start->IsFocusable()) {
    return start;
  } else {
    // Children don't need to be locked on the UI thread.
    for (const auto& child : start->children()) {
      View* view = FindFirstFocus(child.get());
      if (view) {
        return view;
      }
    }
  }
  return nullptr;
}

View* FindNextFocusRight(View* start) {
  CHECK_THREAD(thread::Ui);
  return FocusSearch(FocusAcceptRight, FocusOrderRight, start, start->bounds(),
                     true);
}

View* FindNextFocusLeft(View* start) {
  CHECK_THREAD(thread::Ui);
  return FocusSearch(FocusAcceptLeft, FocusOrderLeft, start, start->bounds(),
                     true);
}

View* FindNextFocusUp(View* start) {
  CHECK_THREAD(thread::Ui);
  return FocusSearch(FocusAcceptUp, FocusOrderUp, start, start->bounds(), true);
}

View* FindNextFocusDown(View* start) {
  CHECK_THREAD(thread::Ui);
  return FocusSearch(FocusAcceptDown, FocusOrderDown, start, start->bounds(),
                     true);
}

}  // namespace ui
