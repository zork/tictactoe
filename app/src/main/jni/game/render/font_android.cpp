////
// font_android.cpp
////

#include "game/render/font.h"

#include "base/image/bitmap.h"
#include "platform/android/android.h"

namespace font {
void DrawText(Bitmap* image,
              const std::string& text,
              int font_size,
              float color[],
              ui::View::HAlign halign,
              ui::View::VAlign valign) {
  JNIEnv* env = android::GetJNIEnv();
  jclass text_draw_class =
      env->FindClass("com/zork/common/image/TextDraw");
  jmethodID draw_text = env->GetStaticMethodID(
      text_draw_class, "DrawText", "(Ljava/lang/String;IFFFFIIIIJ)V");
  env->CallStaticVoidMethod(
      text_draw_class, draw_text, env->NewStringUTF(text.c_str()), font_size,
      color[0], color[1], color[2], color[3], image->width(), image->height(),
      halign, valign, (jlong)image->image_data());
}
}  // namespace font

extern "C" {

void Java_com_zork_common_image_TextDraw_nativeOnTextDone(
    JNIEnv* env,
    jclass,
    jintArray image_data,
    jlong image_ptr) {
  jint* image = (jint*)image_ptr;
  jsize size = env->GetArrayLength(image_data);
  env->GetIntArrayRegion(image_data, 0, size, image);
}
}
