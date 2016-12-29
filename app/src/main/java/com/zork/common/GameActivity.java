package com.zork.common;

import android.app.Activity;
import android.opengl.GLSurfaceView;
import android.os.Bundle;
import android.support.v4.view.ViewCompat;
import android.view.KeyEvent;
import android.view.MotionEvent;
import android.view.Window;
import android.view.WindowManager;
import javax.microedition.khronos.egl.EGLConfig;
import javax.microedition.khronos.opengles.GL10;

public class GameActivity extends Activity {
  @Override
  public void onCreate(Bundle savedInstanceState) {
    super.onCreate(savedInstanceState);

    requestWindowFeature(Window.FEATURE_NO_TITLE);

    getWindow()
        .setFlags(
            WindowManager.LayoutParams.FLAG_FULLSCREEN, WindowManager.LayoutParams.FLAG_FULLSCREEN);
    mView =
        new GLSurfaceView(this) {
          @Override
          public boolean dispatchHoverEvent(MotionEvent event) {
            return mController.dispatchHoverEvent(event) || super.dispatchHoverEvent(event);
          }

          @Override
          public boolean onKeyDown(int keyCode, KeyEvent event) {
            return mController.onKeyDown(keyCode) || super.onKeyDown(keyCode, event);
          }

          @Override
          public boolean onKeyUp(int keyCode, KeyEvent event) {
            return mController.onKeyUp(keyCode) || super.onKeyUp(keyCode, event);
          }

          @Override
          public boolean onTouchEvent(final MotionEvent event) {
            return mController.onTouchEvent(event);
          }
        };

    mView.setEGLContextClientVersion(2);
    mView.setPreserveEGLContextOnPause(true);
    mView.setWillNotDraw(false);
    mView.setRenderer(mRenderer);
    setContentView(mView);

    mController = new Controller(this, mView);
    ViewCompat.setAccessibilityDelegate(mView, mController);
    mController.onCreate();
  }

  @Override
  protected void onPause() {
    mView.onPause();
    mController.onPause();
    super.onPause();
  }

  @Override
  protected void onResume() {
    super.onResume();
    mController.onResume();
    mView.onResume();
  }

  @Override
  protected void onDestroy() {
    mController.onDestroy();
    super.onDestroy();
  }

  @Override
  public void onBackPressed() {
    if (!mController.onBackPressed()) {
      super.onBackPressed();
    }
  }

  private final GLSurfaceView.Renderer mRenderer =
      new GLSurfaceView.Renderer() {
        @Override
        public void onSurfaceCreated(GL10 gl, EGLConfig config) {
          mController.RenderInit();
        }

        @Override
        public void onSurfaceChanged(GL10 gl, int w, int h) {
          mController.Resize(w, h);
        }

        @Override
        public void onDrawFrame(GL10 gl) {
          mController.Render();
        }
      };

  private GLSurfaceView mView;

  private Controller mController;
}
