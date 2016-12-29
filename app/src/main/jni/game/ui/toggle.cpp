////
// toggle.cpp
////

#include "game/ui/toggle.h"

#include "base/image/bitmap.h"
#include "base/thread/thread_util.h"
#include "game/ui/ui_texture.h"

namespace ui {

Toggle::Toggle() : toggle_listener_(nullptr), state_(true) {}

Toggle::~Toggle() {}

void Toggle::SetAltImage(const std::string& filename) {
  if (!alt_image_)
    alt_image_ = std::make_unique<UiTexture>();
  alt_image_->SetImage(filename);
}

void Toggle::SetAltImage(std::unique_ptr<Bitmap> image) {
  if (!alt_image_)
    alt_image_ = std::make_unique<UiTexture>();
  alt_image_->SetImage(std::move(image));
}

void Toggle::Click() {
  CHECK_THREAD(thread::Ui);
  state_ = !state_;
  if (toggle_listener_)
    toggle_listener_->OnToggle(this, state_);
  Button::Click();
}

// View:
void Toggle::RenderInternal(RenderState* render_state) {
  if (state_) {
    DrawImage(render_state, image(), bounds());
  } else {
    DrawImage(render_state, alt_image_.get(), bounds());
  }

  RenderChildren(render_state);
}

}  // namespace ui
