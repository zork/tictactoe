////
// animation.h
////

#pragma once

#include "base/macros.h"
#include "base/time.h"
#include "game/ui/image.h"

#include <memory>
#include <vector>

class Bitmap;

namespace ui {

class Animation : public Image {
 public:
  Animation();
  ~Animation() override;
  DISALLOW_COPY_AND_ASSIGN(Animation);

  void SetPeriod(const TimeInterval& period) { animation_period_ = period; }
  void SetLoop(bool loop) { animation_loop_ = loop; }
  void AddFrame(const std::string& filename);
  void AddFrame(std::unique_ptr<Bitmap> image);
  void ClearFrames();

 private:
  void AddFrameInternal(std::unique_ptr<UiTexture> image);

  // View:
  void Measure(int* width, int* height) override;
  void RenderInternal(RenderState* render_state) override;

  TimeInterval animation_period_;
  bool animation_loop_;
  Timestamp animation_start_time_;

  Mutex frames_lock_;
  std::vector<std::unique_ptr<UiTexture>> frames_;
};

}  // namespace ui
