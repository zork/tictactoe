////
// label.cpp
////

#include "game/ui/label.h"

#include "base/image/bitmap.h"
#include "game/render/font.h"
#include "game/ui/accessibility_info.h"
#include "game/ui/render_state.h"
#include "game/ui/root_view.h"
#include "game/ui/ui_texture.h"

namespace {
const int kDefaultFontSize = 34;
}

namespace ui {

Label::Label()
    : font_size_(kDefaultFontSize),
      text_halign_(kHAlignLeft),
      text_valign_(kVAlignTop),
      dirty_(false) {
  text_color_[0] = 1;
  text_color_[1] = 1;
  text_color_[2] = 1;
  text_color_[3] = 1;
}

Label::~Label() {}

void Label::SetText(const std::string& text) {
  {
    AutoLock lock(&text_lock_);
    text_ = text;
  }
  dirty_ = true;
  if (root_view())
    root_view()->OnTextChanged(this);
}

std::string Label::GetText() {
  AutoLock lock(&text_lock_);
  return text_;
}

void Label::SetTextColor(float red, float green, float blue, float alpha) {
  text_color_[0] = red;
  text_color_[1] = green;
  text_color_[2] = blue;
  text_color_[3] = alpha;
  dirty_ = true;
}

void Label::SetTextHAlign(HAlign align) {
  text_halign_ = align;
  dirty_ = true;
}

void Label::SetTextVAlign(VAlign align) {
  text_valign_ = align;
  dirty_ = true;
}

void Label::SetFontSize(int size) {
  font_size_ = size;
  dirty_ = true;
}

// protected:
// View:
void Label::GetAccessibilityInfo(ui::AccessibilityInfo* info) {
  View::GetAccessibilityInfo(info);
  info->role = ui::AccessibilityInfo::ROLE_LABEL;
  // Don't override accessibility_label
  if (info->text.empty()) {
    AutoLock lock(&text_lock_);
    info->text = text_;
  }
}

void Label::LayoutChildren() {
  dirty_ = true;
  Image::LayoutChildren();
}

void Label::RenderInternal(RenderState* render_state) {
  if (dirty_) {
    dirty_ = false;
    std::string text = GetText();
    if (!text.empty()) {
      if (!text_image_)
        text_image_ = std::make_unique<UiTexture>();

      int image_width = bounds().width();
      int image_height = bounds().height();
      auto image = std::make_unique<Bitmap>(image_width, image_height);
      font::DrawText(image.get(), text, font_size_, text_color_, text_halign_,
                     text_valign_);

      text_image_->SetImage(std::move(image));
    } else {
      text_image_.reset();
    }
  }

  DrawImage(render_state, image(), bounds());
  DrawImage(render_state, text_image_.get(), bounds());
  RenderChildren(render_state);
}

}  // namespace ui
