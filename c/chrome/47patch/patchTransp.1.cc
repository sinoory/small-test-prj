commit 89e08c2ef0bb16f82a285c022d9fdee70e0d5c1e
Author: Beirong Huang <beirong.huang@seraphic-corp.com>
Date:   Mon Feb 5 17:23:28 2018 +0800

    Bug #19963: make background transparent and add home url
    
    Change-Id: Ided57928ceb7f8c29f7ad8dc084d752132fb3c53
    Reviewed-on: http://gerrit.seraphic.com:8080/15018
    Tested-by: Jenkins <jenkins@seraphic-corp.com>
    Reviewed-by: Wang Cui <cui.wang@seraphic-corp.com>

diff --git a/src/cc/trees/layer_tree_impl.cc b/src/cc/trees/layer_tree_impl.cc
index 90efa38..8aa0a06 100644
--- a/src/cc/trees/layer_tree_impl.cc
+++ b/src/cc/trees/layer_tree_impl.cc
@@ -47,7 +47,11 @@ LayerTreeImpl::LayerTreeImpl(
       source_frame_number_(-1),
       hud_layer_(0),
       background_color_(0),
+#if defined(SRAF_ENABLE_TRANSPARENT_BACKGROUND)
+      has_transparent_background_(true),
+#else
       has_transparent_background_(false),
+#endif
       currently_scrolling_layer_id_(Layer::INVALID_ID),
       overscroll_elasticity_layer_id_(Layer::INVALID_ID),
       page_scale_layer_id_(Layer::INVALID_ID),
diff --git a/src/cc/trees/layer_tree_impl.h b/src/cc/trees/layer_tree_impl.h
index ed4413a..da46dc8 100644
--- a/src/cc/trees/layer_tree_impl.h
+++ b/src/cc/trees/layer_tree_impl.h
@@ -170,8 +170,11 @@ class CC_EXPORT LayerTreeImpl {
   void ApplySentScrollAndScaleDeltasFromAbortedCommit();
 
   SkColor background_color() const { return background_color_; }
+#if defined(SRAF_ENABLE_TRANSPARENT_BACKGROUND)
+  void set_background_color(SkColor color) { background_color_ = 0; }
+#else
   void set_background_color(SkColor color) { background_color_ = color; }
-
+#endif
   bool has_transparent_background() const {
     return has_transparent_background_;
   }
diff --git a/src/content/browser/android/content_view_core_impl.cc b/src/content/browser/android/content_view_core_impl.cc
index 273d73e..52ddd6d 100644
--- a/src/content/browser/android/content_view_core_impl.cc
+++ b/src/content/browser/android/content_view_core_impl.cc
@@ -354,10 +354,14 @@ ScopedJavaLocalRef<jobject> ContentViewCoreImpl::GetJavaObject() {
 }
 
 jint ContentViewCoreImpl::GetBackgroundColor(JNIEnv* env, jobject obj) {
+#if defined(SRAF_ENABLE_TRANSPARENT_BACKGROUND)
+  return SK_ColorTRANSPARENT;
+#else
   RenderWidgetHostViewAndroid* rwhva = GetRenderWidgetHostViewAndroid();
   if (!rwhva)
     return SK_ColorWHITE;
   return rwhva->GetCachedBackgroundColor();
+#endif
 }
 
 void ContentViewCoreImpl::PauseOrResumeGeolocation(bool should_pause) {
diff --git a/src/content/public/android/java/src/org/chromium/content/browser/ContentViewRenderView.java b/src/content/public/android/java/src/org/chromium/content/browser/ContentViewRenderView.java
index 0381044..388ab66 100644
--- a/src/content/public/android/java/src/org/chromium/content/browser/ContentViewRenderView.java
+++ b/src/content/public/android/java/src/org/chromium/content/browser/ContentViewRenderView.java
@@ -46,7 +46,10 @@ public class ContentViewRenderView extends FrameLayout {
         mSurfaceView = createSurfaceView(getContext());
         mSurfaceView.setZOrderMediaOverlay(true);
 
-        setSurfaceViewBackgroundColor(Color.WHITE);
+        //SRAF_ENABLE_TRANSPARENT_BACKGROUND
+        mSurfaceView.getHolder().setFormat(PixelFormat.TRANSLUCENT);
+        setSurfaceViewBackgroundColor(Color.TRANSPARENT);
+
         addView(mSurfaceView,
                 new FrameLayout.LayoutParams(
                         FrameLayout.LayoutParams.MATCH_PARENT,
diff --git a/src/content/shell/android/java/res/layout/shell_view.xml b/src/content/shell/android/java/res/layout/shell_view.xml
index 37f702a..49ef311 100644
--- a/src/content/shell/android/java/res/layout/shell_view.xml
+++ b/src/content/shell/android/java/res/layout/shell_view.xml
@@ -5,7 +5,7 @@
      Use of this source code is governed by a BSD-style license that can be
      found in the LICENSE file.
  -->
-
+<!-- SRAF_ENABLE_TRANSPARENT_BACKGROUND -->
 <org.chromium.content_shell.Shell
     xmlns:android="http://schemas.android.com/apk/res/android"
     android:id="@+id/container"
@@ -13,8 +13,8 @@
     android:layout_height="match_parent"
     android:orientation="vertical" >
     <LinearLayout android:id="@+id/toolbar"
-        android:layout_width="match_parent"
-        android:layout_height="wrap_content"
+        android:layout_width="0dp"
+        android:layout_height="0dp"
         android:orientation="horizontal"
         android:background="@drawable/progress">
         <ImageButton android:id="@+id/stop_reload_button"
@@ -51,4 +51,4 @@
         android:layout_width="match_parent"
         android:layout_height="0dp"
         android:layout_weight="1" />
-</org.chromium.content_shell.Shell>
\ No newline at end of file
+</org.chromium.content_shell.Shell>
diff --git a/src/content/shell/android/java/src/org/chromium/content_shell/ShellManager.java b/src/content/shell/android/java/src/org/chromium/content_shell/ShellManager.java
index 278291c..5360a43 100644
--- a/src/content/shell/android/java/src/org/chromium/content_shell/ShellManager.java
+++ b/src/content/shell/android/java/src/org/chromium/content_shell/ShellManager.java
@@ -27,8 +27,8 @@ import org.chromium.ui.base.WindowAndroid;
  */
 @JNINamespace("content")
 public class ShellManager extends FrameLayout {
-
-    public static final String DEFAULT_SHELL_URL = "http://www.google.com";
+    //sraf start page
+    public static final String DEFAULT_SHELL_URL = "http://192.168.18.10/~seraphic%5chuangbr/";
     private static boolean sStartup = true;
     private WindowAndroid mWindow;
     private Shell mActiveShell;
diff --git a/src/content/shell/android/shell_apk/AndroidManifest.xml.jinja2 b/src/content/shell/android/shell_apk/AndroidManifest.xml.jinja2
index 8a71044..d2e65c6 100644
--- a/src/content/shell/android/shell_apk/AndroidManifest.xml.jinja2
+++ b/src/content/shell/android/shell_apk/AndroidManifest.xml.jinja2
@@ -19,13 +19,14 @@
     <uses-permission android:name="android.permission.VIBRATE"/>
     <uses-permission android:name="android.permission.WAKE_LOCK"/>
     <uses-permission android:name="android.permission.WRITE_EXTERNAL_STORAGE"/>
-
+    <!-- SRAF_ENABLE_TRANSPARENT_BACKGROUND -->
     <application android:name="ContentShellApplication"
             android:icon="@mipmap/app_icon"
+            android:theme="@android:style/Theme.Translucent.NoTitleBar"
             android:label="Content Shell">
         <activity android:name="ContentShellActivity"
                   android:launchMode="singleTask"
-                  android:theme="@android:style/Theme.Holo.Light.NoActionBar"
+                  android:theme="@android:style/Theme.Translucent.NoTitleBar"
                   android:configChanges="orientation|keyboardHidden|keyboard|screenSize"
                   android:windowSoftInputMode="adjustResize"
                   android:hardwareAccelerated="true">
diff --git a/src/third_party/WebKit/Source/web/WebViewImpl.cpp b/src/third_party/WebKit/Source/web/WebViewImpl.cpp
index b708684..495851f 100644
--- a/src/third_party/WebKit/Source/web/WebViewImpl.cpp
+++ b/src/third_party/WebKit/Source/web/WebViewImpl.cpp
@@ -430,7 +430,11 @@ WebViewImpl::WebViewImpl(WebViewClient* client)
     , m_operationsAllowed(WebDragOperationNone)
     , m_dragOperation(WebDragOperationNone)
     , m_devToolsEmulator(nullptr)
+#if defined(SRAF_ENABLE_TRANSPARENT_BACKGROUND)
+    , m_isTransparent(true)
+#else
     , m_isTransparent(false)
+#endif
     , m_tabsToLinks(false)
     , m_layerTreeView(0)
     , m_rootLayer(0)
