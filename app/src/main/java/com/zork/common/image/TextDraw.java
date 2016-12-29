package com.zork.common.image;

import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.graphics.Paint;
import android.graphics.Typeface;
import android.util.Log;
import java.util.ArrayList;

public class TextDraw {
  public static Typeface g_default_font = null;

  public static void DrawText(
      String text,
      int font_size,
      float red,
      float green,
      float blue,
      float alpha,
      int width,
      int height,
      int halign,
      int valign,
      long image_ptr) {
    Bitmap bitmap_texture = Bitmap.createBitmap(width, height, Bitmap.Config.ARGB_8888);

    Canvas canvas = new Canvas(bitmap_texture);
    Paint paint = new Paint();

    paint.setAntiAlias(true);
    paint.setTextSize(font_size);
    if (g_default_font != null) paint.setTypeface(g_default_font);

    float widths[] = new float[text.length()];
    paint.getTextWidths(text, widths);

    int text_color =
        (((int) (0xFF * alpha) & 0xFF) << 24)
            + (((int) (0xFF * blue) & 0xFF) << 16)
            + (((int) (0xFF * green) & 0xFF) << 8)
            + (((int) (0xFF * red) & 0xFF));
    paint.setColor(text_color);

    ArrayList<Integer> line_starts = new ArrayList<Integer>();
    ArrayList<Integer> line_ends = new ArrayList<Integer>();
    ArrayList<Integer> line_widths = new ArrayList<Integer>();

    int max_lines = (int) (height / paint.getFontSpacing());

    int end = 0;
    while (line_starts.size() < max_lines && end < text.length()) {
      int start = end;
      int text_width = 0;
      int best = start;
      int best_width = 0;
      while (end < text.length()) {
        if (text.charAt(end) == '\n') {
          best = end;
          best_width = text_width;
          break;
        }
        if (text_width + widths[end] > width) {
          if (start == end) {
            end++;
          } else {
            end = best;
            text_width = best_width;
          }
          break;
        }

        if (Character.isWhitespace(text.charAt(end))) {
          best = end;
          best_width = text_width;
        }

        text_width += widths[end];
        end++;
      }

      line_starts.add(start);
      line_ends.add(end);
      line_widths.add(text_width);

      if (end < text.length() && Character.isWhitespace(text.charAt(end))) {
        end++;
      }
    }

    int drawn_lines = line_starts.size();
    if (drawn_lines * paint.getFontSpacing() > height) {
      drawn_lines = (int) (((float) height) / paint.getFontSpacing());
    }

    int y_offset = -(int) paint.ascent();
    if (valign == kVAlignBottom) {
      y_offset += height - (int) (paint.getFontSpacing() * drawn_lines);
    } else if (valign == kVAlignCenter) {
      y_offset += (height - (int) (paint.getFontSpacing() * drawn_lines)) / 2;
    } else {
      if (valign != kVAlignTop) {
        Log.e("TextDraw", "Bad valign: " + valign);
      }
    }

    for (int line = 0; line < drawn_lines; line++) {
      int x_offset = 0;
      if (halign == kHAlignRight) {
        x_offset = width - line_widths.get(line);
      } else if (halign == kHAlignCenter) {
        x_offset = (width - line_widths.get(line)) / 2;
      } else {
        if (halign != kHAlignLeft) {
          Log.e("TextDraw", "Bad halign: " + halign);
        }
      }
      canvas.drawText(
          text,
          line_starts.get(line),
          line_ends.get(line),
          x_offset,
          y_offset + line * paint.getFontSpacing(),
          paint);
    }

    int buffer[] = new int[width * height];
    bitmap_texture.getPixels(buffer, 0, width, 0, 0, width, height);
    nativeOnTextDone(buffer, image_ptr);
  }

  private static native void nativeOnTextDone(int[] image_data, long image_ptr);

  private static int kHAlignLeft = 0;
  private static int kHAlignRight = 1;
  private static int kHAlignCenter = 2;

  private static int kVAlignTop = 0;
  private static int kVAlignBottom = 1;
  private static int kVAlignCenter = 2;
}
