////
// platform_delegate_android.cpp
////

#include "platform/android/platform_delegate_android.h"

#include "base/file/file_manager_android.h"
#include "base/thread/thread_util.h"
#include "game/ui/view.h"
#include "platform/android/android.h"

enum {
  TYPE_WINDOW_STATE_CHANGED = 0x00000020,
};

PlatformDelegateAndroid::PlatformDelegateAndroid(jobject controller,
                                                 jobject asset_manager,
                                                 jstring data_dir) {
  JNIEnv* env = android::GetJNIEnv();
  // Keep a reference to the Java Controller.
  controller_ = env->NewGlobalRef(controller);

  // Create the file manager.
  const char* data_dir_str = env->GetStringUTFChars(data_dir, NULL);
  file_manager_ =
      std::make_unique<FileManagerAndroid>(asset_manager, data_dir_str);
  env->ReleaseStringUTFChars(data_dir, data_dir_str);

  // Create the game.
  game_ = SimpleGame::Create(this);
}

PlatformDelegateAndroid::~PlatformDelegateAndroid() {
  JNIEnv* env = android::GetJNIEnv();
  // Release the reference to the Java Controller.
  env->DeleteGlobalRef(controller_);
}

// private:
// PlatformDelegate:
void PlatformDelegateAndroid::InvalidateRootView() {
  CHECK_THREAD(thread::Ui);
  JNIEnv* env = android::GetJNIEnv();
  env->CallVoidMethod(controller_,
                      env->GetMethodID(env->GetObjectClass(controller_),
                                       "invalidateRoot", "()V"));
}

void PlatformDelegateAndroid::InvalidateView(int id) {
  CHECK_THREAD(thread::Ui);
  JNIEnv* env = android::GetJNIEnv();
  env->CallVoidMethod(controller_,
                      env->GetMethodID(env->GetObjectClass(controller_),
                                       "invalidateVirtualView", "(I)V"),
                      id);
}

void PlatformDelegateAndroid::HandleViewChanged(int id) {
  CHECK_THREAD(thread::Ui);
  JNIEnv* env = android::GetJNIEnv();

  std::string text;
  ui::View* view = game_->FindViewById(id);
  if (view) {
    // If the view has a title, use it.
    text = view->title();
  }

  env->CallVoidMethod(
      controller_, env->GetMethodID(env->GetObjectClass(controller_),
                                    "sendAccessibilityEvent",
                                    "(ILjava/lang/CharSequence;)V"),
      TYPE_WINDOW_STATE_CHANGED, env->NewStringUTF(text.c_str()));
}

void PlatformDelegateAndroid::HandleTextChanged(int view_id) {
  CHECK_THREAD(thread::Ui);
  const int CONTENT_CHANGE_TYPE_TEXT = 0x0002;
  JNIEnv* env = android::GetJNIEnv();

  env->CallVoidMethod(controller_,
                      env->GetMethodID(env->GetObjectClass(controller_),
                                       "invalidateVirtualView", "(II)V"),
                      view_id, CONTENT_CHANGE_TYPE_TEXT);
}

void PlatformDelegateAndroid::AccessibilityAnnounce(const std::string& text) {
  JNIEnv* env = android::GetJNIEnv();
  env->CallVoidMethod(
      controller_,
      env->GetMethodID(env->GetObjectClass(controller_),
                       "accessibilityAnnounce", "(Ljava/lang/CharSequence;)V"),
      env->NewStringUTF(text.c_str()));
}

void PlatformDelegateAndroid::PostNativeUiTask(std::unique_ptr<Task> task,
                                               const TimeInterval& delay) {
  JNIEnv* env = android::GetJNIEnv();

  jclass controller_class = env->GetObjectClass(controller_);
  jmethodID post_ui_task_method =
      env->GetMethodID(controller_class, "postUiTask", "(JJ)V");

  env->CallVoidMethod(controller_, post_ui_task_method, (jlong)task.release(),
                      (jlong)delay.Milliseconds());
}
