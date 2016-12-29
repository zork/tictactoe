////
// render_state.h
////

#pragma once

#include "base/math/rect.h"
#include "base/time.h"

#include <vector>

class BasicTextureShader;
class Vector4;

namespace ui {

class RenderState {
 public:
  RenderState(const Timestamp& frame_time,
              BasicTextureShader* shader,
              const math::Rect& bounds);
  ~RenderState();

  const Timestamp& frame_time() const { return frame_time_; }
  BasicTextureShader* shader() { return shader_; }
  const math::Rect& bounds() const { return bounds_; }

  void PushUiColor(float red, float green, float blue, float alpha);
  void PopUiColor();

 private:
  const Timestamp frame_time_;
  BasicTextureShader* shader_;
  math::Rect bounds_;
  std::vector<Vector4> color_stack_;
};

}  // namespace ui
