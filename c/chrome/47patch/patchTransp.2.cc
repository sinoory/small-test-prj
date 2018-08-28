commit dd2a05f8b55cb4ed51eaf76238983791a3b10921
Author: huyk <yakun.hu@seraphic-corp.com>
Date:   Fri Jun 1 19:23:30 2018 +0800

    Feature #21851: support transparent background
    on Android 7.1.1 platform
    
    Change-Id: I95ffb779cd6c9b912114d2dd1b03147dbc9f4d01
    Reviewed-on: http://gerrit.seraphic.com:8080/17640
    Tested-by: Jenkins <jenkins@seraphic-corp.com>
    Reviewed-by: Wang Cui <cui.wang@seraphic-corp.com>

diff --git a/src/content/browser/android/content_view_render_view.cc b/src/content/browser/android/content_view_render_view.cc
index 8b1ea2b..114b691 100644
--- a/src/content/browser/android/content_view_render_view.cc
+++ b/src/content/browser/android/content_view_render_view.cc
@@ -107,8 +107,15 @@ void ContentViewRenderView::OnSwapBuffersCompleted(int pending_swap_buffers) {
 }
 
 void ContentViewRenderView::InitCompositor() {
+#if defined(SRAF_ENABLE_TRANSPARENT_BACKGROUND)
+  if (!compositor_) {
+    compositor_.reset(Compositor::Create(this, root_window_));
+    compositor_->SetHasTransparentBackground(true);
+  }
+#else
   if (!compositor_)
     compositor_.reset(Compositor::Create(this, root_window_));
+#endif
 }
 
 jlong ContentViewRenderView::GetUIResourceProvider(JNIEnv* env,
diff --git a/src/content/public/android/java/src/org/chromium/content/browser/ContentViewRenderView.java b/src/content/public/android/java/src/org/chromium/content/browser/ContentViewRenderView.java
index 388ab66..bda5d72 100644
--- a/src/content/public/android/java/src/org/chromium/content/browser/ContentViewRenderView.java
+++ b/src/content/public/android/java/src/org/chromium/content/browser/ContentViewRenderView.java
@@ -47,7 +47,7 @@ public class ContentViewRenderView extends FrameLayout {
         mSurfaceView.setZOrderMediaOverlay(true);
 
         //SRAF_ENABLE_TRANSPARENT_BACKGROUND
-        mSurfaceView.getHolder().setFormat(PixelFormat.TRANSLUCENT);
+        mSurfaceView.getHolder().setFormat(PixelFormat.TRANSPARENT);
         setSurfaceViewBackgroundColor(Color.TRANSPARENT);
 
         addView(mSurfaceView,
diff --git a/src/third_party/WebKit/Source/web/WebViewImpl.cpp b/src/third_party/WebKit/Source/web/WebViewImpl.cpp
index 495851f..b708684 100644
--- a/src/third_party/WebKit/Source/web/WebViewImpl.cpp
+++ b/src/third_party/WebKit/Source/web/WebViewImpl.cpp
@@ -430,11 +430,7 @@ WebViewImpl::WebViewImpl(WebViewClient* client)
     , m_operationsAllowed(WebDragOperationNone)
     , m_dragOperation(WebDragOperationNone)
     , m_devToolsEmulator(nullptr)
-#if defined(SRAF_ENABLE_TRANSPARENT_BACKGROUND)
-    , m_isTransparent(true)
-#else
     , m_isTransparent(false)
-#endif
     , m_tabsToLinks(false)
     , m_layerTreeView(0)
     , m_rootLayer(0)
