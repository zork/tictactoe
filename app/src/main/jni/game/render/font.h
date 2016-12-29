////
// font.h
////

#include "base/platform.h"
#include "game/ui/view.h"

#include <string>

class Bitmap;
class File;

namespace font {
#if OS_CHROME || OS_OSX || OS_WIN
void SetDefaultFont(File* font_data);
#elif OS_IOS
void SetDefaultFont(const char* name);
#endif

void DrawText(Bitmap* image,
              const std::string& text,
              int font_size,
              float color[],
              ui::View::HAlign halign,
              ui::View::VAlign valign);
}  // namespace font
