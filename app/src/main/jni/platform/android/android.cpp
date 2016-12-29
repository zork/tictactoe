////
// android.cpp
////

#include "platform/android/android.h"

#include "base/logging.h"
#include "base/math/rect.h"
#include "base/thread/thread_local.h"
#include "game/ui/accessibility_action.h"
#include "game/ui/accessibility_info.h"

namespace {
ThreadLocalPtr<JNIEnv> t_jni_envs;

const char* kCollectionInfoClass =
    "android/support/v4/view/accessibility/"
    "AccessibilityNodeInfoCompat$CollectionInfoCompat";
const char* kCollectionItemInfoClass =
    "android/support/v4/view/accessibility/"
    "AccessibilityNodeInfoCompat$CollectionItemInfoCompat";
const char* kIntegerClass = "java/lang/Integer";
const char* kRectClass = "android/graphics/Rect";

const char* kButtonClassName = "android.widget.Button";
const char* kGridClassName = "android.widget.GridView";
const char* kLabelClassName = "android.widget.TextView";
const char* kListClassName = "android.widget.AbsListView";
const char* kSliderClassName = "android.widget.SeekBar";

enum {
  LIVE_REGION_NONE = 0x0000,
  LIVE_REGION_POLITE = 0x0001,
  LIVE_REGION_ASSERTIVE = 0x0002,
};

bool IsClickable(const ui::AccessibilityInfo& info) {
  return info.role == ui::AccessibilityInfo::ROLE_BUTTON;
}

bool IsFocusable(const ui::AccessibilityInfo& info) {
  switch (info.role) {
    case ui::AccessibilityInfo::ROLE_BUTTON:
    case ui::AccessibilityInfo::ROLE_LABEL:
    case ui::AccessibilityInfo::ROLE_SLIDER:
      return true;

    default:
      return false;
  }
}
}

namespace android {

JNIEnv* GetJNIEnv() {
  return t_jni_envs.Get();
}

void SetThreadJNIEnv(JNIEnv* env) {
  t_jni_envs.Set(env);
}

jobject Integer(JNIEnv* env, int value) {
  jclass clazz = env->FindClass(kIntegerClass);
  jmethodID constructor = env->GetMethodID(clazz, "<init>", "(I)V");
  return env->NewObject(clazz, constructor, value);
}

jobject Rect(JNIEnv* env, const math::Rect& value) {
  jclass clazz = env->FindClass(kRectClass);
  jmethodID constructor = env->GetMethodID(clazz, "<init>", "(IIII)V");
  return env->NewObject(clazz, constructor, value.x(), value.y(), value.right(),
                        value.top());
}

jobject CollectionInfo(JNIEnv* env, const ui::CollectionInfo& value) {
  jclass clazz = env->FindClass(kCollectionInfoClass);
  jmethodID obtain = env->GetStaticMethodID(
      clazz, "obtain",
      "(IIZI)Landroid/support/v4/view/accessibility/"
      "AccessibilityNodeInfoCompat$CollectionInfoCompat;");
  return env->CallStaticObjectMethod(clazz, obtain, value.row_count,
                                     value.column_count, false, 0);
}

jobject CollectionItemInfo(JNIEnv* env, const ui::CollectionItemInfo& value) {
  jclass clazz = env->FindClass(kCollectionItemInfoClass);
  jmethodID obtain = env->GetStaticMethodID(
      clazz, "obtain",
      "(IIIIZZ)Landroid/support/v4/view/accessibility/"
      "AccessibilityNodeInfoCompat$CollectionItemInfoCompat;");
  return env->CallStaticObjectMethod(clazz, obtain, value.row_index, 1,
                                     value.column_index, 1, false, false);
}

math::Rect SafeBounds(const math::Rect& input) {
  int width = input.width() <= 0 ? 1 : input.width();
  int height = input.height() <= 0 ? 1 : input.height();
  return math::Rect::MakeXYWH(input.x(), input.y(), width, height);
}

void SetAccessibilityNode(JNIEnv* env,
                          const ui::AccessibilityInfo& info,
                          /* AccessibilityNodeInfoCompat */ jobject node) {
  jclass node_class = env->GetObjectClass(node);
  jmethodID add_action_method =
      env->GetMethodID(node_class, "addAction", "(I)V");

  // Populate the java object
  // Bounds.
  jmethodID set_bounds_in_parent_method = env->GetMethodID(
      node_class, "setBoundsInParent", "(Landroid/graphics/Rect;)V");
  env->CallVoidMethod(node, set_bounds_in_parent_method,
                      android::Rect(env, SafeBounds(info.bounds_in_parent)));

  // Text.
  jmethodID set_text_method =
      env->GetMethodID(node_class, "setText", "(Ljava/lang/CharSequence;)V");
  env->CallVoidMethod(node, set_text_method,
                      env->NewStringUTF(info.text.c_str()));

  // Clickable
  if (IsClickable(info)) {
    jmethodID method = env->GetMethodID(node_class, "setClickable", "(Z)V");
    env->CallVoidMethod(node, method, true);
    env->CallVoidMethod(node, add_action_method, ACTION_CLICK);
  }

  // Focusable
  if (IsFocusable(info)) {
    jmethodID method = env->GetMethodID(node_class, "setFocusable", "(Z)V");
    env->CallVoidMethod(node, method, true);
    env->CallVoidMethod(node, add_action_method, ACTION_FOCUS);
  }

  // Live region
  if (info.live != ui::kAccessibilityLiveNone) {
    int live = LIVE_REGION_NONE;
    switch (info.live) {
      case ui::kAccessibilityLiveNone:
        live = LIVE_REGION_NONE;
        break;
      case ui::kAccessibilityLivePolite:
        live = LIVE_REGION_POLITE;
        break;
      case ui::kAccessibilityLiveAssertive:
        live = LIVE_REGION_ASSERTIVE;
        break;
    }
    jmethodID method = env->GetMethodID(node_class, "setLiveRegion", "(I)V");
    env->CallVoidMethod(node, method, live);
  }

  // Classname
  std::string classname;
  switch (info.role) {
    case ui::AccessibilityInfo::ROLE_NONE:
      break;
    case ui::AccessibilityInfo::ROLE_BUTTON:
      classname = kButtonClassName;
      break;
    case ui::AccessibilityInfo::ROLE_GRID:
      classname = kGridClassName;
      break;
    case ui::AccessibilityInfo::ROLE_LABEL:
      classname = kLabelClassName;
      break;
    case ui::AccessibilityInfo::ROLE_LIST:
      classname = kListClassName;
      break;
    case ui::AccessibilityInfo::ROLE_SLIDER:
      classname = kSliderClassName;
      break;
  }
  if (!classname.empty()) {
    jmethodID method = env->GetMethodID(node_class, "setClassName",
                                        "(Ljava/lang/CharSequence;)V");
    env->CallVoidMethod(node, method, env->NewStringUTF(classname.c_str()));
  }

  // Collection info
  if (info.collection_info) {
    jmethodID method = env->GetMethodID(node_class, "setCollectionInfo",
                                        "(Ljava/lang/Object;)V");
    env->CallVoidMethod(node, method,
                        CollectionInfo(env, *info.collection_info));
  }

  if (info.collection_item_info) {
    jmethodID method = env->GetMethodID(node_class, "setCollectionItemInfo",
                                        "(Ljava/lang/Object;)V");
    env->CallVoidMethod(node, method,
                        CollectionItemInfo(env, *info.collection_item_info));
  }
}

bool GetAccessibilityAction(JNIEnv* env,
                            jint action_id,
                            /* Bundle */ jobject args,
                            ui::AccessibilityAction* action) {
  if (action_id == ACTION_CLICK) {
    action->action = ui::AccessibilityAction::ACTION_CLICK;
    return true;
  }
  return false;
}

}  // namespace android
