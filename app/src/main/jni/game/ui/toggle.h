////
// toggle.h
////

#pragma once

#include "base/macros.h"
#include "game/ui/button.h"

#include <memory>
#include <string>

class Bitmap;

namespace ui {

class Toggle : public Button {
 public:
  class Listener {
   public:
    virtual ~Listener() {}
    virtual void OnToggle(Toggle* toggle, bool state) = 0;
  };

  Toggle();
  ~Toggle() override;
  DISALLOW_COPY_AND_ASSIGN(Toggle);

  void SetToggleListener(Listener* listener) { toggle_listener_ = listener; }
  void SetState(bool state) { state_ = state; }

  void SetAltImage(const std::string& filename);
  void SetAltImage(std::unique_ptr<Bitmap> image);

  void Click() override;

 private:
  // View:
  void RenderInternal(RenderState* render_state) override;

  Listener* toggle_listener_;

  bool state_;
  std::unique_ptr<UiTexture> alt_image_;
};

}  // namespace ui
