////
// grid.cpp
////

#include "game/ui/grid.h"

#include "base/logging.h"
#include "base/thread/thread_util.h"
#include "game/ui/accessibility_info.h"

#include <algorithm>

namespace ui {

Grid::Grid() : columns_(0), cell_width_(0), cell_height_(0) {
  SetIsRelativeLayout(true);
}

Grid::~Grid() {}

// private:
void Grid::MeasureChildren(std::vector<int>* widths,
                           std::vector<int>* heights) {
  DCHECK(children_lock_.IsLocked());
  if (children_.empty())
    return;

  int columns = NumColumns();
  int rows = NumRows();
  widths->resize(columns, cell_width_);
  heights->resize(rows, cell_height_);

  if (!cell_width_ && !cell_height_) {
    int index = 0;
    for (const auto& child : children_) {
      if (child->visible()) {
        int width = 0;
        int height = 0;
        child->Measure(&width, &height);
        int column = index % columns;
        int row = index / columns;
        if (!cell_width_)
          (*widths)[column] = std::max(widths->at(column), width);
        if (!cell_height_)
          (*heights)[row] = std::max(heights->at(row), height);
      }
      index++;
    }
  }
}

// View:
void Grid::GetAccessibilityInfo(AccessibilityInfo* info) {
  CHECK_THREAD(thread::Ui);
  // Children don't need to be locked on the UI thread.
  View::GetAccessibilityInfo(info);
  // If there's only one row or column, it's a list.
  if (columns_ == 0 || columns_ == 1) {
    info->role = ui::AccessibilityInfo::ROLE_LIST;
  } else {
    info->role = ui::AccessibilityInfo::ROLE_GRID;
  }

  info->collection_info =
      std::make_unique<CollectionInfo>(NumRows(), NumColumns());
}

void Grid::GetAccessibilityInfoForChild(View* child, AccessibilityInfo* info) {
  CHECK_THREAD(thread::Ui);
  // Children don't need to be locked on the UI thread.
  auto ix = std::find_if(children_.begin(), children_.end(),
                         [&child](const auto& p) { return p.get() == child; });
  DCHECK(ix != children_.end());

  int index = ix - children_.begin();
  int columns = NumColumns();
  info->collection_item_info =
      std::make_unique<CollectionItemInfo>(index / columns, index % columns);
}

void Grid::Measure(int* width, int* height) {
  AutoLock lock(&children_lock_);
  std::vector<int> widths;
  std::vector<int> heights;
  MeasureChildren(&widths, &heights);
  *width = 0;
  *height = 0;
  for (const auto& column_width : widths) {
    *width += column_width;
  }
  for (const auto& column_height : heights) {
    *height += column_height;
  }
}

void Grid::LayoutChildren() {
  AutoLock lock(&children_lock_);
  std::vector<int> widths;
  std::vector<int> heights;
  MeasureChildren(&widths, &heights);

  int columns = NumColumns();

  int x_offset = 0;
  int y_offset = 0;
  int row = 0;
  int column = 0;
  for (const auto& child : children_) {
    if (child->visible()) {
      child->Layout(math::Rect::MakeXYWH(bounds().x() + x_offset,
                                         bounds().y() + y_offset,
                                         widths[column], heights[row]));
    }
    if (++column >= columns) {
      x_offset = 0;
      y_offset += heights[row];
      row++;
      column = 0;
    } else {
      x_offset += widths[column];
    }
  }
}

}  // namespace ui
