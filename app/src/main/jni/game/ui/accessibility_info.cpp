////
// accessibility_info.cpp
////

#include "game/ui/accessibility_info.h"

namespace ui {

CollectionInfo::CollectionInfo(int in_row_count, int in_column_count)
    : row_count(in_row_count), column_count(in_column_count) {}
CollectionInfo::~CollectionInfo() {}

CollectionItemInfo::CollectionItemInfo(int in_row_index, int in_column_index)
    : row_index(in_row_index), column_index(in_column_index) {}
CollectionItemInfo::~CollectionItemInfo() {}

AccessibilityInfo::AccessibilityInfo()
    : role(ROLE_NONE), live(kAccessibilityLiveNone) {}

AccessibilityInfo::~AccessibilityInfo() {}

}  // namespace ui
