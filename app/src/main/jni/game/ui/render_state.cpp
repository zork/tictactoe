////
// render_state.cpp
////

#include "game/ui/render_state.h"

#include "base/math/vector.h"
#include "game/render/basic_texture_shader.h"

namespace ui {

RenderState::RenderState(const Timestamp& frame_time,
                         BasicTextureShader* shader,
                         const math::Rect& bounds)
    : frame_time_(frame_time), shader_(shader), bounds_(bounds) {
  PushUiColor(1, 1, 1, 1);
}

RenderState::~RenderState() {}

void RenderState::PushUiColor(float red, float green, float blue, float alpha) {
  color_stack_.push_back(Vector4(red, green, blue, alpha));
  shader_->SetColor(color_stack_.back().value());
}

void RenderState::PopUiColor() {
  color_stack_.pop_back();
  shader_->SetColor(color_stack_.back().value());
}

}  // namespace ui
