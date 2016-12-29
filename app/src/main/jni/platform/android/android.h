////
// android.h
////

#pragma once

#include <jni.h>

namespace math {
class Rect;
}

namespace ui {
struct AccessibilityAction;
struct AccessibilityInfo;
}

namespace android {
const int ACTION_FOCUS = 0x00000001;
const int ACTION_CLICK = 0x00000010;

JNIEnv* GetJNIEnv();
void SetThreadJNIEnv(JNIEnv* env);

jobject Integer(JNIEnv* env, int value);
jobject Rect(JNIEnv* env, const math::Rect& value);

void SetAccessibilityNode(JNIEnv* env,
                          const ui::AccessibilityInfo& info,
                          /* AccessibilityNodeInfoCompat */ jobject node);
bool GetAccessibilityAction(JNIEnv* env,
                            jint action_id,
                            /* Bundle */ jobject args,
                            ui::AccessibilityAction* action);

}  // namespace android
