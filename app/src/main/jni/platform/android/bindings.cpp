////
// bindings.cpp
////

#include "base/thread/task.h"
#include "base/thread/thread_util.h"
#include "game/input/keycodes.h"
#include "game/ui/accessibility_action.h"
#include "game/ui/view.h"
#include "platform/android/android.h"
#include "platform/android/platform_delegate_android.h"

#include <jni.h>

namespace {
const int ROOT_ID = -1;

std::unique_ptr<PlatformDelegateAndroid> g_platform_delegate;
}

#define JNI_FUNC(name) Java_com_zork_common_Controller_##name

extern "C" {

// Activity
void JNI_FUNC(nativeCreate)(JNIEnv* env,
                            jclass,
                            jobject controller,
                            jobject asset_manager,
                            jstring data_dir) {
  // Initialize UI thread.
  InitThread(thread::Ui);
  android::SetThreadJNIEnv(env);

  g_platform_delegate = std::make_unique<PlatformDelegateAndroid>(
      controller, asset_manager, data_dir);
  g_platform_delegate->game()->OnCreate();
}

void JNI_FUNC(nativePause)(JNIEnv* env, jclass) {
  g_platform_delegate->game()->OnPause();
}

void JNI_FUNC(nativeResume)(JNIEnv* env, jclass) {
  g_platform_delegate->game()->OnResume();
}

void JNI_FUNC(nativeDestroy)(JNIEnv* env, jclass) {
  g_platform_delegate->game()->OnDestroy();
  g_platform_delegate.reset();
}

void JNI_FUNC(nativeRunUiTask)(JNIEnv* env, jclass, jlong task_ptr) {
  CHECK_THREAD(thread::Ui);
  Task* task = reinterpret_cast<Task*>(task_ptr);
  task->Execute();
  delete task;
}

// View
jboolean JNI_FUNC(nativeOnKeyDown)(JNIEnv* env, jclass, jint key_code) {
  return g_platform_delegate->game()->OnKeyDown(key_code);
}

jboolean JNI_FUNC(nativeOnKeyUp)(JNIEnv* env, jclass, jint key_code) {
  return g_platform_delegate->game()->OnKeyUp(key_code);
}

void JNI_FUNC(nativeTouchStart)(JNIEnv* env, jclass, jint id, jint x, jint y) {
  Timestamp time = Timestamp::Now();
  g_platform_delegate->game()->OnTouchStart(id, x, y, time);
}

void JNI_FUNC(nativeTouchMove)(JNIEnv* env, jclass, jint id, jint x, jint y) {
  Timestamp time = Timestamp::Now();
  g_platform_delegate->game()->OnTouchMove(id, x, y, time);
}

void JNI_FUNC(nativeTouchEnd)(JNIEnv* env, jclass, jint id, jint x, jint y) {
  Timestamp time = Timestamp::Now();
  g_platform_delegate->game()->OnTouchEnd(id, x, y, time);
}

jboolean JNI_FUNC(nativeOnBackPressed)(JNIEnv* env, jclass) {
  bool handled = g_platform_delegate->game()->OnKeyDown(VKEY_BACK);
  return g_platform_delegate->game()->OnKeyUp(VKEY_BACK) || handled;
}

// Renderer
void JNI_FUNC(nativeRenderInit)(JNIEnv* env, jclass) {
  InitThread(thread::Render);
  android::SetThreadJNIEnv(env);
  g_platform_delegate->game()->OnRenderInit();
}

void JNI_FUNC(nativeResize)(JNIEnv* env, jclass, jint width, jint height) {
  g_platform_delegate->game()->OnResize(width, height);
}

void JNI_FUNC(nativeRender)(JNIEnv* env, jclass) {
  g_platform_delegate->game()->OnRender();
}

// Accessibility
jint JNI_FUNC(nativeGetVirtualViewAt)(JNIEnv* env, jclass, jfloat x, jfloat y) {
  ui::View* view = g_platform_delegate->game()->GetViewAt(x, y);
  if (view) {
    return view->id();
  }
  return ROOT_ID;
}

void JNI_FUNC(nativeGetVisibleVirtualViews)(
    JNIEnv* env,
    jclass,
    /* List<Integer> */ jobject virtual_view_ids) {
  ui::View* view = g_platform_delegate->game()->GetView();
  if (view) {
    jclass list_class = env->GetObjectClass(virtual_view_ids);
    jmethodID add_method =
        env->GetMethodID(list_class, "add", "(Ljava/lang/Object;)Z");
    env->CallBooleanMethod(virtual_view_ids, add_method,
                           android::Integer(env, view->id()));
  }
}

void JNI_FUNC(nativeOnPopulateEvent)(JNIEnv* env,
                                     jclass,
                                     jint virtual_view_id,
                                     /* AccessibilityEvent */ jobject event) {
  jclass event_class = env->GetObjectClass(event);

  // Set an empty content description.
  jmethodID set_content_description_method = env->GetMethodID(
      event_class, "setContentDescription", "(Ljava/lang/CharSequence;)V");
  env->CallVoidMethod(event, set_content_description_method,
                      env->NewStringUTF(""));
}

void JNI_FUNC(nativeOnPopulateNode)(
    JNIEnv* env,
    jclass,
    jint virtual_view_id,
    /* AccessibilityNodeInfoCompat */ jobject node) {
  // Get the accessibility info.
  ui::AccessibilityInfo info;
  ui::View* view = g_platform_delegate->game()->FindViewById(virtual_view_id);
  if (view) {
    view->GetAccessibilityInfo(&info);
  }

  // Fill in the Java object.
  android::SetAccessibilityNode(env, info, node);
}

jint JNI_FUNC(nativeGetVirtualViewParent)(JNIEnv* env,
                                          jclass,
                                          jint virtual_view_id) {
  ui::View* view = g_platform_delegate->game()->FindViewById(virtual_view_id);
  if (view && view->parent()) {
    return view->parent()->id();
  }
  // If the view has no parent or doesn't exist, the parent is the container.
  return ROOT_ID;
}

void JNI_FUNC(nativeGetVirtualViewChildren)(
    JNIEnv* env,
    jclass,
    jint virtual_view_id,
    /* List<Integer> */ jobject virtual_view_ids) {
  ui::View* view = g_platform_delegate->game()->FindViewById(virtual_view_id);
  if (view) {
    std::vector<int> child_ids;
    view->GetVisibleChildIds(&child_ids);

    jclass list_class = env->GetObjectClass(virtual_view_ids);
    jmethodID add_method =
        env->GetMethodID(list_class, "add", "(Ljava/lang/Object;)Z");
    for (const auto& id : child_ids) {
      env->CallBooleanMethod(virtual_view_ids, add_method,
                             android::Integer(env, id));
    }
  }
}

jboolean JNI_FUNC(nativeOnPerformAction)(JNIEnv* env,
                                         jclass,
                                         jint virtual_view_id,
                                         jint action_id,
                                         /* Bundle */ jobject arguments) {
  ui::AccessibilityAction action;
  if (android::GetAccessibilityAction(env, action_id, arguments, &action)) {
    ui::View* view = g_platform_delegate->game()->FindViewById(virtual_view_id);
    if (view) {
      view->SendAccessibilityAction(action);
    }
    return true;
  }
  return false;
}
}  // extern "C"
