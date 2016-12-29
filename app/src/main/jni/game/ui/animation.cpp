////
// animation.cpp
////

#include "game/ui/animation.h"

#include "base/image/bitmap.h"
#include "game/ui/render_state.h"
#include "game/ui/ui_texture.h"

namespace ui {

Animation::Animation()
    : animation_period_(TimeInterval::FromSeconds(1)), animation_loop_(true) {}

Animation::~Animation() {}

void Animation::AddFrame(const std::string& filename) {
  auto texture = std::make_unique<UiTexture>();
  texture->SetImage(filename);
  AddFrameInternal(std::move(texture));
}

void Animation::AddFrame(std::unique_ptr<Bitmap> image) {
  auto texture = std::make_unique<UiTexture>();
  texture->SetImage(std::move(image));
  AddFrameInternal(std::move(texture));
}

void Animation::ClearFrames() {
  AutoLock lock(&frames_lock_);
  frames_.clear();
}

// private:
void Animation::AddFrameInternal(std::unique_ptr<UiTexture> image) {
  {
    AutoLock lock(&frames_lock_);
    frames_.push_back(std::move(image));
  }
  RequestLayout();
}

// View:
void Animation::Measure(int* width, int* height) {
  AutoLock lock(&frames_lock_);
  if (layout_fill()) {
    *width = -1;
    *height = -1;
  } else {
    if (layout_width() >= 0) {
      *width = layout_width();
    } else if (!frames_.empty()) {
      *width = frames_[0]->width();
    } else {
      *width = 0;
    }

    if (layout_height() >= 0) {
      *height = layout_height();
    } else if (!frames_.empty()) {
      *height = frames_[0]->height();
    } else {
      *height = 0;
    }
  }
}

void Animation::RenderInternal(RenderState* render_state) {
  if (animation_start_time_ == Timestamp())
    animation_start_time_ = render_state->frame_time();
  AutoLock lock(&frames_lock_);
  if (!frames_.empty()) {
    size_t frame_index =
        (render_state->frame_time() - animation_start_time_).Seconds() *
        frames_.size() / animation_period_.Seconds();
    if (animation_loop_) {
      frame_index = frame_index % frames_.size();
    } else {
      if (frame_index >= frames_.size())
        frame_index = frames_.size() - 1;
    }

    DrawImage(render_state, frames_[frame_index].get(), bounds());
  }
  RenderChildren(render_state);
}

}  // namespace ui
