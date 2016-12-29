////
// grid.h
////

#pragma once

#include "base/macros.h"
#include "game/ui/view.h"

#include <vector>

namespace ui {

class Grid : public View {
 public:
  Grid();
  ~Grid() override;
  DISALLOW_COPY_AND_ASSIGN(Grid);

  void SetColumns(int count) { columns_ = count; }
  void SetCellWidth(int cell_width) { cell_width_ = cell_width; }
  void SetCellHeight(int cell_height) { cell_height_ = cell_height; }

 private:
  int NumRows() const { return 1 + ((children_.size() - 1) / NumColumns()); }

  int NumColumns() const { return columns_ > 0 ? columns_ : children_.size(); }

  void MeasureChildren(std::vector<int>* widths, std::vector<int>* heights);

  // View:
  void GetAccessibilityInfo(AccessibilityInfo* info) override;
  void GetAccessibilityInfoForChild(View* child,
                                    AccessibilityInfo* info) override;
  void Measure(int* width, int* height) override;
  void LayoutChildren() override;

  int columns_;
  int cell_width_;
  int cell_height_;
};

}  // namespace ui
