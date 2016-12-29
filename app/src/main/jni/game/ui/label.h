////
// label.h
////

#pragma once

#include "base/macros.h"
#include "game/ui/image.h"

#include <string>

namespace ui {

class Label : public Image {
 public:
  Label();
  ~Label() override;
  DISALLOW_COPY_AND_ASSIGN(Label);

  void SetText(const std::string& text);
  std::string GetText();
  void SetTextColor(float red, float green, float blue, float alpha);
  void SetTextHAlign(HAlign align);
  void SetTextVAlign(VAlign align);
  void SetFontSize(int size);

 protected:
  // View:
  void GetAccessibilityInfo(ui::AccessibilityInfo* info) override;
  void LayoutChildren() override;
  void RenderInternal(RenderState* render_state) override;

 private:
  std::unique_ptr<UiTexture> text_image_;

  Mutex text_lock_;
  std::string text_;
  float text_color_[4];
  int font_size_;
  HAlign text_halign_;
  VAlign text_valign_;

  bool dirty_;
};

}  // namespace ui
