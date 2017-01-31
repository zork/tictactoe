////
// default_focus_render_delegate.cpp
////

#include "game/render/default_focus_render_delegate.h"

#include "base/math/rect.h"
#include "game/render/identity_shader.h"
#include "game/ui/view.h"

DefaultFocusRenderDelegate::DefaultFocusRenderDelegate()
    : shader_(new IdentityShader) {}

DefaultFocusRenderDelegate::~DefaultFocusRenderDelegate() {}

// private:
// ui::FocusRenderDelegate:
void DefaultFocusRenderDelegate::Init(const Timestamp& time) {
  shader_->Load();
}

void DefaultFocusRenderDelegate::OnSize(int width, int height) {
  ui_projection_matrix_.OrthoProjection(0, width, height, 0, -1000, 1000);
}

void DefaultFocusRenderDelegate::Render(ui::RenderState* render_state,
                                        ui::View* view) {
  if (!view->visible())
    return;

  shader_->Use();
  shader_->SetMVPMatrix(ui_projection_matrix_);
  math::Rect bounds = view->bounds();

  float x = bounds.x() + 0.5;
  float y = bounds.y() + 0.5;
  float right = bounds.right() + 0.5;
  float top = bounds.top() + 0.5;
  float vertices[] = {
      // (0, 1)
      x - 1, top + 1, 0,
      // (1, 1)
      right + 1, top + 1, 0,
      // (1, 0)
      right + 1, y - 1, 0,
      // (0, 0)
      x - 1, y - 1, 0,

      // (0, 1)
      x - 2, top + 2, 0,
      // (1, 1)
      right + 2, top + 2, 0,
      // (1, 0)
      right + 2, y - 2, 0,
      // (0, 0)
      x - 2, y - 2, 0,
  };

  // clang-format off
  float colors[] = {
      1, 1, 1, 1,
      1, 1, 1, 1,
      1, 1, 1, 1,
      1, 1, 1, 1,

      1, 1, 1, 1,
      1, 1, 1, 1,
      1, 1, 1, 1,
      1, 1, 1, 1,
  };
  // clang-format on

  uint32_t faces[] = {0, 1, 2, 3, 0, 4, 5, 6, 7, 4};
  shader_->DrawPrimitives(vertices, arraysize(vertices), colors,
                          arraysize(colors), faces, arraysize(faces),
                          kLineStrip);
}
