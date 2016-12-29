////
// focus_util.h
////

#pragma once

namespace ui {

class View;

View* FindFirstFocus(View* start);
View* FindNextFocusRight(View* start);
View* FindNextFocusLeft(View* start);
View* FindNextFocusUp(View* start);
View* FindNextFocusDown(View* start);

}  // namespace ui
