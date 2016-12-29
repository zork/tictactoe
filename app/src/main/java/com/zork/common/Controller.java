package com.zork.common;

import android.app.Activity;
import android.content.Context;
import android.content.res.AssetManager;
import android.os.Bundle;
import android.os.Handler;
import android.support.v4.view.accessibility.AccessibilityNodeInfoCompat;
import android.support.v4.widget.ExploreByTouchHelper;
import android.view.MotionEvent;
import android.view.View;
import android.view.accessibility.AccessibilityEvent;
import android.view.accessibility.AccessibilityManager;
import java.util.LinkedList;
import java.util.List;

public class Controller extends ExploreByTouchHelper {
  public Controller(Activity activity, View view) {
    super(view);
    mActivity = activity;
    mAccessibilityManager =
        (AccessibilityManager) activity.getSystemService(Context.ACCESSIBILITY_SERVICE);
    mView = view;
  }

  // Delegated from Activity
  void onCreate() {
    nativeCreate(
        this, mActivity.getAssets(), mActivity.getExternalFilesDir(null).getAbsolutePath());
  }

  void onPause() {
    nativePause();
  }

  void onResume() {
    nativeResume();
  }

  void onDestroy() {
    nativeDestroy();
  }

  public void postUiTask(long task, long delay) {
    class NativeUiTask implements Runnable {
      private NativeUiTask(long task) {
        mTask = task;
      }

      public void run() {
        nativeRunUiTask(mTask);
      }

      private long mTask;
    }
    Handler main_handler = new Handler(mActivity.getMainLooper());
    main_handler.postDelayed(new NativeUiTask(task), delay);
  }

  // Delegated from View
  boolean onKeyDown(int keyCode) {
    return nativeOnKeyDown(keyCode);
  }

  boolean onKeyUp(int keyCode) {
    return nativeOnKeyUp(keyCode);
  }

  boolean onTouchEvent(final MotionEvent event) {
    int index = event.getActionIndex();
    switch (event.getActionMasked()) {
      case MotionEvent.ACTION_DOWN:
      case MotionEvent.ACTION_POINTER_DOWN:
        nativeTouchStart(
            event.getPointerId(index), (int) event.getX(index), (int) event.getY(index));
        break;

      case MotionEvent.ACTION_MOVE:
        for (int i = 0; i < event.getPointerCount(); ++i) {
          nativeTouchMove(event.getPointerId(i), (int) event.getX(i), (int) event.getY(i));
        }
        break;

      case MotionEvent.ACTION_UP:
      case MotionEvent.ACTION_POINTER_UP:
        nativeTouchEnd(event.getPointerId(index), (int) event.getX(index), (int) event.getY(index));
        break;
    }
    return true;
  }

  boolean onBackPressed() {
    return nativeOnBackPressed();
  }

  // Delegated from Renderer
  void RenderInit() {
    nativeRenderInit();
  }

  public void Resize(int w, int h) {
    nativeResize(w, h);
  }

  public void Render() {
    nativeRender();
  }

  @Override
  protected int getVirtualViewAt(float x, float y) {
    return nativeGetVirtualViewAt(x, y);
  }

  @Override
  protected void getVisibleVirtualViews(List<Integer> virtualViewIds) {
    nativeGetVisibleVirtualViews(virtualViewIds);
  }

  @Override
  protected void onPopulateEventForVirtualView(int virtualViewId, AccessibilityEvent event) {
    nativeOnPopulateEvent(virtualViewId, event);
  }

  @Override
  protected void onPopulateNodeForVirtualView(int virtualViewId, AccessibilityNodeInfoCompat node) {
    nativeOnPopulateNode(virtualViewId, node);
    List<Integer> virtualViewIds = new LinkedList<>();
    int parentId = nativeGetVirtualViewParent(virtualViewId);
    node.setParent(mView, parentId);
    nativeGetVirtualViewChildren(virtualViewId, virtualViewIds);
    for (Integer id : virtualViewIds) {
      node.addChild(mView, id);
    }
  }

  @Override
  protected boolean onPerformActionForVirtualView(int virtualViewId, int action, Bundle arguments) {
    return nativeOnPerformAction(virtualViewId, action, arguments);
  }

  void sendAccessibilityEvent(int type, CharSequence text) {
    if (mAccessibilityManager.isEnabled()) {
      AccessibilityEvent event = AccessibilityEvent.obtain();
      event.setEventType(type);
      event.setContentDescription(text);
      mAccessibilityManager.sendAccessibilityEvent(event);
    }
  }

  void accessibilityAnnounce(CharSequence text) {
    mView.announceForAccessibility(text);
  }

  private final Activity mActivity;
  private final View mView;
  private final AccessibilityManager mAccessibilityManager;

  // Activity
  private static native void nativeCreate(
      Controller controller, AssetManager assetManager, String dataDirectory);

  private static native void nativePause();

  private static native void nativeResume();

  private static native void nativeDestroy();

  private static native void nativeRunUiTask(long task);

  // View
  private static native boolean nativeOnKeyDown(int keyCode);

  private static native boolean nativeOnKeyUp(int keyCode);

  private static native void nativeTouchStart(int id, int x, int y);

  private static native void nativeTouchMove(int id, int x, int y);

  private static native void nativeTouchEnd(int id, int x, int y);

  private static native boolean nativeOnBackPressed();

  // Renderer
  private static native void nativeRenderInit();

  private static native void nativeResize(int w, int h);

  private static native void nativeRender();

  // Accessibility
  private static native int nativeGetVirtualViewAt(float x, float y);

  private static native void nativeGetVisibleVirtualViews(List<Integer> virtualViewIds);

  private static native int nativeGetVirtualViewParent(int virtualViewId);

  private static native void nativeGetVirtualViewChildren(
      int virtualViewId, List<Integer> virtualViewIds);

  private static native void nativeOnPopulateEvent(int virtualViewId, AccessibilityEvent event);

  private static native void nativeOnPopulateNode(
      int virtualViewId, AccessibilityNodeInfoCompat node);

  private static native boolean nativeOnPerformAction(
      int virtualViewId, int action, Bundle arguments);

  static {
    System.loadLibrary("game");
  }
}
