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

  float x = bounds.x();
  float y = bounds.y();
  float right = bounds.right();
  float top = bounds.top();
  float vertices[] = {
      // (0, 1)
      x, top, 0,

      // (1, 1)
      right, top, 0,

      // (1, 0)
      right, y, 0,

      // (0, 0)
      x, y, 0,
  };

  // clang-format off
  float colors[] = {
    1, 1, 1, .5,
    1, 1, 1, .5,
    1, 1, 1, .5,
    1, 1, 1, .5,
  };
  // clang-format on

  uint32_t faces[] = {0, 1, 2, 0, 2, 3};
  shader_->DrawTriangles(vertices, arraysize(vertices), colors,
                         arraysize(colors), faces, arraysize(faces));
}
