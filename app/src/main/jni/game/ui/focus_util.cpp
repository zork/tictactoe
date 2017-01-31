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
typedef void (*FocusWrapFunc)(const View&,
                              const math::Rect&,
                              std::vector<math::Rect>*);

// Move focus right.
bool FocusAcceptRight(const math::Rect& bounds, const View& view) {
  return view.bounds().x() > bounds.x() &&
         view.bounds().bottom() > bounds.y() &&
         view.bounds().y() < bounds.bottom();
}

bool FocusOrderRight(const View& left, const View& right) {
  return (left.bounds().x() < right.bounds().x()) ||
         (left.bounds().x() == right.bounds().x() &&
          left.bounds().y() < right.bounds().y());
}

void FocusWrapRight(const View& parent,
                    const math::Rect& bounds,
                    std::vector<math::Rect>* wrapped_bounds) {
  // First, wrap to views below the current view.
  if (parent.bounds().bottom() > bounds.bottom()) {
    wrapped_bounds->push_back(
        math::Rect::MakeXYRB(parent.bounds().x() - 1, bounds.bottom(),
                             parent.bounds().x(), parent.bounds().bottom()));
  }

  // Next, wrap to views above the current view.
  if (parent.bounds().y() < bounds.y()) {
    wrapped_bounds->push_back(
        math::Rect::MakeXYRB(parent.bounds().x() - 1, parent.bounds().y(),
                             parent.bounds().x(), bounds.y()));
  }

  // Finally, wrap to views aligned with the current view.
  wrapped_bounds->push_back(
      math::Rect::MakeXYRB(parent.bounds().x() - 1, bounds.y(),
                           parent.bounds().x(), bounds.bottom()));
}

// Move focus left.
bool FocusAcceptLeft(const math::Rect& bounds, const View& view) {
  return view.bounds().x() < bounds.x() &&
         view.bounds().bottom() > bounds.y() &&
         view.bounds().y() < bounds.bottom();
}

bool FocusOrderLeft(const View& left, const View& right) {
  return (left.bounds().x() > right.bounds().x()) ||
         (left.bounds().x() == right.bounds().x() &&
          left.bounds().y() > right.bounds().y());
}

void FocusWrapLeft(const View& parent,
                   const math::Rect& bounds,
                   std::vector<math::Rect>* wrapped_bounds) {
  // First, wrap to views above the current view.
  if (parent.bounds().y() < bounds.y()) {
    wrapped_bounds->push_back(
        math::Rect::MakeXYRB(parent.bounds().right(), parent.bounds().y(),
                             parent.bounds().right() + 1, bounds.y()));
  }

  // Next, wrap to views below the current view.
  if (parent.bounds().bottom() > bounds.bottom()) {
    wrapped_bounds->push_back(math::Rect::MakeXYRB(
        parent.bounds().right(), bounds.bottom(), parent.bounds().right() + 1,
        parent.bounds().bottom()));
  }

  // Finally, wrap to views aligned with the current view.
  wrapped_bounds->push_back(
      math::Rect::MakeXYRB(parent.bounds().right(), bounds.y(),
                           parent.bounds().right() + 1, bounds.bottom()));
}

// Move focus up.
bool FocusAcceptUp(const math::Rect& bounds, const View& view) {
  return view.bounds().x() < bounds.right() &&
         view.bounds().right() > bounds.x() && view.bounds().y() < bounds.y();
}

bool FocusOrderUp(const View& left, const View& right) {
  return (left.bounds().y() > right.bounds().y()) ||
         (left.bounds().y() == right.bounds().y() &&
          left.bounds().x() > right.bounds().x());
}

void FocusWrapUp(const View& parent,
                 const math::Rect& bounds,
                 std::vector<math::Rect>* wrapped_bounds) {
  // First, wrap to views left of the current view.
  if (parent.bounds().x() < bounds.x()) {
    wrapped_bounds->push_back(
        math::Rect::MakeXYRB(parent.bounds().x(), parent.bounds().bottom(),
                             bounds.x(), parent.bounds().bottom() + 1));
  }

  // Next, wrap to views right of the current view.
  if (parent.bounds().right() > bounds.right()) {
    wrapped_bounds->push_back(math::Rect::MakeXYRB(
        bounds.right(), parent.bounds().bottom(), parent.bounds().right(),
        parent.bounds().bottom() + 1));
  }

  // Finally, wrap to views aligned with the current view.
  wrapped_bounds->push_back(
      math::Rect::MakeXYRB(bounds.x(), parent.bounds().bottom(), bounds.right(),
                           parent.bounds().bottom() + 1));
}

// Move focus down.
bool FocusAcceptDown(const math::Rect& bounds, const View& view) {
  return view.bounds().x() < bounds.right() &&
         view.bounds().right() > bounds.x() && view.bounds().y() > bounds.y();
}

bool FocusOrderDown(const View& left, const View& right) {
  return (left.bounds().y() < right.bounds().y()) ||
         (left.bounds().y() == right.bounds().y() &&
          left.bounds().x() < right.bounds().x());
}

void FocusWrapDown(const View& parent,
                   const math::Rect& bounds,
                   std::vector<math::Rect>* wrapped_bounds) {
  // First, wrap to views right of the current view.
  if (parent.bounds().right() > bounds.right()) {
    wrapped_bounds->push_back(
        math::Rect::MakeXYRB(bounds.right(), parent.bounds().y() - 1,
                             parent.bounds().right(), parent.bounds().y()));
  }

  // Next, wrap to views left of the current view.
  if (parent.bounds().x() < bounds.x()) {
    wrapped_bounds->push_back(
        math::Rect::MakeXYRB(parent.bounds().x(), parent.bounds().y() - 1,
                             bounds.x(), parent.bounds().y()));
  }

  // Finally, wrap to views aligned with the current view.
  wrapped_bounds->push_back(
      math::Rect::MakeXYRB(bounds.x(), parent.bounds().y() - 1, bounds.right(),
                           parent.bounds().y()));
}

View* FocusSearch(FocusAcceptFunc accept,
                  FocusOrderFunc compare,
                  FocusWrapFunc wrap,
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
        return FocusSearch(accept, compare, wrap, view->parent(), bounds, true);
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
      View* focus_view =
          FocusSearch(accept, compare, wrap, view, bounds, false);
      if (focus_view)
        return focus_view;
    }
  }

  if (follow_parent) {
    if (view->parent()) {
      // Check through parent views for a match.
      return FocusSearch(accept, compare, wrap, view->parent(), bounds, true);
    } else {
      // Wrap bounds, and check for a new match.
      std::vector<math::Rect> wrapped_bounds;
      wrap(*view, bounds, &wrapped_bounds);
      for (const auto& new_bounds : wrapped_bounds) {
        View* focus_view =
            FocusSearch(accept, compare, wrap, view, new_bounds, false);
        if (focus_view)
          return focus_view;
      }
    }
  }

  // No next view.
  return nullptr;
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
  return FocusSearch(FocusAcceptRight, FocusOrderRight, FocusWrapRight, start,
                     start->bounds(), true);
}

View* FindNextFocusLeft(View* start) {
  CHECK_THREAD(thread::Ui);
  return FocusSearch(FocusAcceptLeft, FocusOrderLeft, FocusWrapLeft, start,
                     start->bounds(), true);
}

View* FindNextFocusUp(View* start) {
  CHECK_THREAD(thread::Ui);
  return FocusSearch(FocusAcceptUp, FocusOrderUp, FocusWrapUp, start,
                     start->bounds(), true);
}

View* FindNextFocusDown(View* start) {
  CHECK_THREAD(thread::Ui);
  return FocusSearch(FocusAcceptDown, FocusOrderDown, FocusWrapDown, start,
                     start->bounds(), true);
}

}  // namespace ui
