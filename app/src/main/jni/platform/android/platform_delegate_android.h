////
// platform_delegate_android.h
////

#pragma once

#include "game/core/platform_delegate.h"
#include "game/simple_game.h"

#include <jni.h>

class FileManager;
class SimpleGame;

class PlatformDelegateAndroid : public PlatformDelegate {
 public:
  PlatformDelegateAndroid(jobject controller,
                          jobject asset_manager,
                          jstring data_dir);

  ~PlatformDelegateAndroid() override;

  SimpleGame* game() { return game_.get(); }

 private:
  // PlatformDelegate:
  void InvalidateRootView() override;
  void InvalidateView(int id) override;
  void HandleViewChanged(int id) override;
  void HandleTextChanged(int view_id) override;
  void AccessibilityAnnounce(const std::string& text) override;
  void PostNativeUiTask(std::unique_ptr<Task> task,
                        const TimeInterval& delay) override;

  jobject controller_;
  std::unique_ptr<FileManager> file_manager_;
  std::unique_ptr<SimpleGame> game_;
};
