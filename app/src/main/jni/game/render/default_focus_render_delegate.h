////
// default_focus_render_delegate.h
////

#pragma once

#include "base/macros.h"
#include "base/math/matrix.h"
#include "game/ui/focus_render_delegate.h"

#include <memory>

class IdentityShader;

class DefaultFocusRenderDelegate : public ui::FocusRenderDelegate {
 public:
  DefaultFocusRenderDelegate();
  ~DefaultFocusRenderDelegate() override;
  DISALLOW_COPY_AND_ASSIGN(DefaultFocusRenderDelegate);

 private:
  // ui::FocusRenderDelegate:
  void Init(const Timestamp& time) override;
  void OnSize(int width, int height) override;
  void Render(ui::RenderState* render_state, ui::View* view) override;

  std::unique_ptr<IdentityShader> shader_;
  Matrix ui_projection_matrix_;
};
