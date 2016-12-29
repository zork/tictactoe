////
// slider.cpp
////

#include "game/ui/slider.h"

#include "base/image/bitmap.h"
#include "base/logging.h"
#include "base/math/math.h"
#include "game/input/mouse_event.h"
#include "game/input/touch_event.h"
#include "game/render/basic_texture_shader.h"
#include "game/ui/render_state.h"
#include "game/ui/root_view.h"
#include "game/ui/ui_texture.h"

namespace ui {

Slider::Slider()
    : listener_(nullptr),
      mouse_captured_(false),
      captured_touch_(nullptr),
      min_(0),
      max_(1),
      value_(1),
      left_cap_(0),
      right_cap_(0),
      thumb_image_(new UiTexture),
      min_image_(new UiTexture),
      max_image_(new UiTexture) {}

Slider::~Slider() {
  if (mouse_captured_)
    root_view()->ReleaseMouse(this);
  if (captured_touch_)
    captured_touch_->ReleaseCapture();
}

void Slider::SetThumbImage(const std::string& filename) {
  thumb_image_->SetImage(filename);
  UpdateLayoutHeightFromImage(thumb_image_.get());
}

void Slider::SetThumbImage(std::unique_ptr<Bitmap> image) {
  thumb_image_->SetImage(std::move(image));
  UpdateLayoutHeightFromImage(thumb_image_.get());
}

void Slider::SetMinImage(const std::string& filename) {
  min_image_->SetImage(filename);
  UpdateLayoutHeightFromImage(min_image_.get());
}

void Slider::SetMinImage(std::unique_ptr<Bitmap> image) {
  min_image_->SetImage(std::move(image));
  UpdateLayoutHeightFromImage(min_image_.get());
}

void Slider::SetMaxImage(const std::string& filename) {
  max_image_->SetImage(filename);
  UpdateLayoutHeightFromImage(max_image_.get());
}

void Slider::SetMaxImage(std::unique_ptr<Bitmap> image) {
  max_image_->SetImage(std::move(image));
  UpdateLayoutHeightFromImage(max_image_.get());
}

// private:
void Slider::UpdateLayoutHeightFromImage(UiTexture* texture) {
  if (layout_height() < texture->height()) {
    SetLayoutHeight(texture->height());
    RequestLayout();
  }
}

void Slider::DrawMinImage(RenderState* render_state, int mid_point) {
  min_image_->UploadTexture();
  if (!min_image_->IsReady())
    return;

  min_image_->Bind();

  float texture_top = min_image_->top();
  float y = bounds().y() + (bounds().height() - min_image_->height()) / 2;
  float bottom = y + min_image_->height();

  // Draw the end cap
  if (left_cap_) {
    float texture_right = min_image_->right() * left_cap_ / min_image_->width();
    float x = bounds().x();
    float right = bounds().x() + left_cap_;
    float vertices[] = {
        x, bottom, 0, right, bottom, 0, right, y, 0, x, y, 0,
    };

    float textures[] = {
        0, texture_top, texture_right, texture_top, texture_right, 0, 0, 0,
    };

    uint32_t faces[] = {0, 1, 2, 0, 2, 3};
    render_state->shader()->DrawTriangles(vertices, arraysize(vertices),
                                          textures, arraysize(textures), faces,
                                          arraysize(faces));
  }

  // Draw the bar
  {
    float texture_right =
        min_image_->right() * (left_cap_ + 1) / min_image_->width();
    float x = bounds().x() + left_cap_;
    float right = mid_point;
    float vertices[] = {
        x, bottom, 0, right, bottom, 0, right, y, 0, x, y, 0,
    };

    float textures[] = {
        texture_right, texture_top, texture_right, texture_top,
        texture_right, 0,           texture_right, 0,
    };

    uint32_t faces[] = {0, 1, 2, 0, 2, 3};
    render_state->shader()->DrawTriangles(vertices, arraysize(vertices),
                                          textures, arraysize(textures), faces,
                                          arraysize(faces));
  }
}

void Slider::DrawMaxImage(RenderState* render_state, int mid_point) {
  max_image_->UploadTexture();
  if (!max_image_->IsReady())
    return;

  max_image_->Bind();

  float texture_top = max_image_->top();
  float y = bounds().y() + (bounds().height() - min_image_->height()) / 2;
  float bottom = y + min_image_->height();

  // Draw the end cap
  if (right_cap_) {
    float texture_left = max_image_->right() *
                         (max_image_->width() - right_cap_) /
                         max_image_->width();
    float texture_right = max_image_->right();

    float right = bounds().right();
    float x = right - right_cap_;
    float vertices[] = {
        x, bottom, 0, right, bottom, 0, right, y, 0, x, y, 0,
    };

    float textures[] = {
        texture_left,  texture_top, texture_right, texture_top,
        texture_right, 0,           texture_left,  0,
    };

    uint32_t faces[] = {0, 1, 2, 0, 2, 3};
    render_state->shader()->DrawTriangles(vertices, arraysize(vertices),
                                          textures, arraysize(textures), faces,
                                          arraysize(faces));
  }

  // Draw the bar
  {
    float texture_left = max_image_->right() *
                         (max_image_->width() - right_cap_ - 1) /
                         max_image_->width();
    float x = mid_point;
    float right = bounds().right() - right_cap_;
    float vertices[] = {
        x, bottom, 0, right, bottom, 0, right, y, 0, x, y, 0,
    };

    float textures[] = {
        texture_left, texture_top, texture_left, texture_top,
        texture_left, 0,           texture_left, 0,
    };

    uint32_t faces[] = {0, 1, 2, 0, 2, 3};
    render_state->shader()->DrawTriangles(vertices, arraysize(vertices),
                                          textures, arraysize(textures), faces,
                                          arraysize(faces));
  }
}

void Slider::DrawThumbImage(RenderState* render_state, int mid_point) {
  if (!thumb_image_->IsSet())
    return;
  int y_offset = (bounds().height() - thumb_image_->height()) / 2;
  DrawImage(render_state, thumb_image_.get(),
            math::Rect::MakeXYWH(mid_point - thumb_image_->width() / 2,
                                 bounds().y() + y_offset, thumb_image_->width(),
                                 thumb_image_->height()));
}

// View:
void Slider::RenderInternal(RenderState* render_state) {
  float value = value_;
  value /= (max_ - min_);

  int mid_point = bounds().x() + left_cap_ + SliderWidth() * value;
  DrawMinImage(render_state, mid_point);
  DrawMaxImage(render_state, mid_point);
  DrawThumbImage(render_state, mid_point);

  RenderChildren(render_state);
}

// InputListener:
bool Slider::OnMouseEvent(const MouseEvent& event) {
  if (event.type() == MouseEvent::kMouseDown && !mouse_captured_) {
    mouse_captured_ = root_view()->CaptureMouse(this);
  }

  if (mouse_captured_) {
    if (event.type() == MouseEvent::kMouseLeave) {
      root_view()->ReleaseMouse(this);
      mouse_captured_ = false;
      return true;
    } else if (event.type() == MouseEvent::kMouseEnter) {
      return false;
    }

    float value =
        (event.position().x() - bounds().x() - left_cap_) / SliderWidth();
    value = math::Clamp(value, 0.0f, 1.0f);

    SetValue(min_ + (max_ - min_) * value);
    if (listener_)
      listener_->OnChange(this);

    if (event.type() == MouseEvent::kMouseUp) {
      root_view()->ReleaseMouse(this);
      mouse_captured_ = false;
    }
    return true;
  }

  // Children of sliders don't get events.
  return false;
}

bool Slider::OnTouchEvent(std::vector<TouchEvent*>& touches, int index) {
  TouchEvent* current_touch = touches[index];

  if (!captured_touch_) {
    if (current_touch->state() == TouchEvent::kTouchStart) {
      captured_touch_ = current_touch;
      current_touch->Capture(this);
    }
  }

  if (current_touch == captured_touch_) {
    const math::Point& position = current_touch->touches().back().position;

    float value = (position.x() - bounds().x() - left_cap_) / SliderWidth();
    value = math::Clamp(value, 0.0f, 1.0f);

    SetValue(min_ + (max_ - min_) * value);
    if (listener_)
      listener_->OnChange(this);

    if (current_touch->state() == TouchEvent::kTouchEnd) {
      captured_touch_->ReleaseCapture();
      captured_touch_ = nullptr;
    }
    return true;
  }

  // Children of sliders don't get events.
  return false;
}

void Slider::OnTouchDelete(TouchEvent* touch) {
  if (touch == captured_touch_) {
    captured_touch_->ReleaseCapture();
    captured_touch_ = nullptr;
  }
}

}  // namespace ui
