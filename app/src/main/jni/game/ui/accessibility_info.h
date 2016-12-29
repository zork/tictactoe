////
// accessibility_info.h
////

#pragma once

#include "base/macros.h"
#include "base/math/rect.h"

#include <memory>
#include <string>

namespace ui {

enum AccessibilityLive {
  kAccessibilityLiveNone,
  kAccessibilityLivePolite,
  kAccessibilityLiveAssertive,
};

const int kCollectionInfoNoItems = -1;
const int kCollectionItemNoIndex = -1;

struct CollectionInfo {
  CollectionInfo(int in_row_count, int in_column_count);
  ~CollectionInfo();
  DISALLOW_COPY_AND_ASSIGN(CollectionInfo);

  int row_count;
  int column_count;
};

struct CollectionItemInfo {
  CollectionItemInfo(int in_row_index, int in_column_index);
  ~CollectionItemInfo();
  DISALLOW_COPY_AND_ASSIGN(CollectionItemInfo);

  int row_index;
  int column_index;
};

struct AccessibilityInfo {
  enum Role {
    ROLE_NONE,
    ROLE_BUTTON,
    ROLE_GRID,
    ROLE_LABEL,
    ROLE_LIST,
    ROLE_SLIDER,
  };
  AccessibilityInfo();
  ~AccessibilityInfo();
  DISALLOW_COPY_AND_ASSIGN(AccessibilityInfo);

  Role role;
  math::Rect bounds_in_parent;
  std::string text;
  AccessibilityLive live;

  std::unique_ptr<CollectionInfo> collection_info;
  std::unique_ptr<CollectionItemInfo> collection_item_info;
};

}  // namespace ui
