commit f8186e60e4fc7132178b3735edaaabcff299293e
Author: huyk <yakun.hu@seraphic-corp.com>
Date:   Mon Jul 2 16:05:47 2018 +0800

    Feature #22198: dig video hole for hbbtv video broadcast
    
    Change-Id: Ibe9f01bf3aeb40b96ee0984eaeb7cd5da6b91a6b
    Reviewed-on: http://gerrit.seraphic.com:8080/18283
    Tested-by: Jenkins <jenkins@seraphic-corp.com>
    Reviewed-by: Wang Cui <cui.wang@seraphic-corp.com>

diff --git a/src/cc/BUILD.gn b/src/cc/BUILD.gn
old mode 100644
new mode 100755
index 854111b..0118f01
--- a/src/cc/BUILD.gn
+++ b/src/cc/BUILD.gn
@@ -173,6 +173,10 @@ component("cc") {
     "layers/solid_color_scrollbar_layer.h",
     "layers/solid_color_scrollbar_layer_impl.cc",
     "layers/solid_color_scrollbar_layer_impl.h",
+    "layers/sraf_video_hole_layer.cc",
+    "layers/sraf_video_hole_layer.h",
+    "layers/sraf_video_hole_layer_impl.cc",
+    "layers/sraf_video_hole_layer_impl.h",
     "layers/surface_layer.cc",
     "layers/surface_layer.h",
     "layers/surface_layer_impl.cc",
@@ -338,6 +342,8 @@ component("cc") {
     "quads/shared_quad_state.h",
     "quads/solid_color_draw_quad.cc",
     "quads/solid_color_draw_quad.h",
+    "quads/sraf_video_hole_draw_quad.cc",
+    "quads/sraf_video_hole_draw_quad.h",
     "quads/stream_video_draw_quad.cc",
     "quads/stream_video_draw_quad.h",
     "quads/surface_draw_quad.cc",
diff --git a/src/cc/layers/layer_impl.h b/src/cc/layers/layer_impl.h
index 4e94980..6876c76 100644
--- a/src/cc/layers/layer_impl.h
+++ b/src/cc/layers/layer_impl.h
@@ -104,6 +104,10 @@ class CC_EXPORT LayerImpl : public LayerAnimationValueObserver,
 
   int id() const { return layer_id_; }
 
+#if !defined(SRAF_VIDEO_HOLE_DISABLE)
+  virtual bool isVideoHoleLayer() { return false; }
+#endif
+
   // LayerAnimationValueProvider implementation.
   gfx::ScrollOffset ScrollOffsetForAnimation() const override;
 
diff --git a/src/cc/layers/render_surface_impl.cc b/src/cc/layers/render_surface_impl.cc
index 560c75c..f6ec3bd 100644
--- a/src/cc/layers/render_surface_impl.cc
+++ b/src/cc/layers/render_surface_impl.cc
@@ -241,6 +241,12 @@ void RenderSurfaceImpl::AppendQuads(RenderPass* render_pass,
                mask_texture_size, owning_layer_->filters(),
                owning_layer_to_target_scale,
                owning_layer_->background_filters());
+
+#if !defined(SRAF_VIDEO_HOLE_DISABLE)
+  if (owning_layer_->isVideoHoleLayer()) {
+    quad->has_video_hole_draw_quad = true;
+  }
+#endif
 }
 
 }  // namespace cc
diff --git a/src/cc/layers/sraf_video_hole_layer.cc b/src/cc/layers/sraf_video_hole_layer.cc
new file mode 100755
index 0000000..d4e2518
--- /dev/null
+++ b/src/cc/layers/sraf_video_hole_layer.cc
@@ -0,0 +1,27 @@
+//
+
+#include "cc/layers/sraf_video_hole_layer.h"
+
+#include "base/logging.h"
+#include "cc/layers/sraf_video_hole_layer_impl.h"
+
+namespace cc {
+
+scoped_refptr<SrafVideoHoleLayer> SrafVideoHoleLayer::Create(
+    const LayerSettings& settings) {
+  return make_scoped_refptr(new SrafVideoHoleLayer(settings));
+}
+
+SrafVideoHoleLayer::SrafVideoHoleLayer(const LayerSettings& settings)
+    : Layer(settings) {
+  SetForceRenderSurface(true);
+}
+
+SrafVideoHoleLayer::~SrafVideoHoleLayer() {}
+
+scoped_ptr<LayerImpl> SrafVideoHoleLayer::CreateLayerImpl(
+    LayerTreeImpl* tree_impl) {
+  return SrafVideoHoleLayerImpl::Create(tree_impl, id());
+}
+
+} //namespace cc
diff --git a/src/cc/layers/sraf_video_hole_layer.h b/src/cc/layers/sraf_video_hole_layer.h
new file mode 100755
index 0000000..5dd07e8
--- /dev/null
+++ b/src/cc/layers/sraf_video_hole_layer.h
@@ -0,0 +1,25 @@
+//
+
+#ifndef CC_LAYERS_SRAF_VIDEO_HOLE_LAYER_H_
+#define CC_LAYERS_SRAF_VIDEO_HOLE_LAYER_H_
+
+#include "cc/layers/layer.h"
+
+namespace cc {
+
+class CC_EXPORT SrafVideoHoleLayer : public Layer {
+ public:
+  static scoped_refptr<SrafVideoHoleLayer> Create(const LayerSettings& settings);
+
+  scoped_ptr<LayerImpl> CreateLayerImpl(LayerTreeImpl* tree_impl) override;
+
+ private:
+  SrafVideoHoleLayer(const LayerSettings& settings);
+  ~SrafVideoHoleLayer() override;
+
+  DISALLOW_COPY_AND_ASSIGN(SrafVideoHoleLayer);
+};
+
+} //namespace cc
+
+#endif  // CC_LAYERS_SRAF_VIDEO_HOLE_LAYER_H_
diff --git a/src/cc/layers/sraf_video_hole_layer_impl.cc b/src/cc/layers/sraf_video_hole_layer_impl.cc
new file mode 100755
index 0000000..2e84297
--- /dev/null
+++ b/src/cc/layers/sraf_video_hole_layer_impl.cc
@@ -0,0 +1,83 @@
+//
+
+#include "cc/layers/sraf_video_hole_layer_impl.h"
+
+#include "base/logging.h"
+#include "cc/trees/layer_tree_impl.h"
+#include "cc/trees/proxy.h"
+#include "cc/quads/sraf_video_hole_draw_quad.h"
+
+namespace cc {
+
+//static
+scoped_ptr<SrafVideoHoleLayerImpl> SrafVideoHoleLayerImpl::Create(
+    LayerTreeImpl* tree_impl,
+    int id) {
+  DCHECK(tree_impl->proxy()->IsImplThread());
+  DCHECK(tree_impl->proxy()->IsMainThreadBlocked());
+  return make_scoped_ptr(
+      new SrafVideoHoleLayerImpl(tree_impl, id));
+}
+
+SrafVideoHoleLayerImpl::SrafVideoHoleLayerImpl(LayerTreeImpl* tree_impl,
+    int id)
+    : LayerImpl(tree_impl, id) {
+}
+
+SrafVideoHoleLayerImpl::~SrafVideoHoleLayerImpl() {
+}
+
+scoped_ptr<LayerImpl> SrafVideoHoleLayerImpl::CreateLayerImpl(
+    LayerTreeImpl* tree_impl) {
+  return make_scoped_ptr(new SrafVideoHoleLayerImpl(tree_impl, id()));
+}
+
+void SrafVideoHoleLayerImpl::AppendQuads(
+    RenderPass* render_pass,
+    AppendQuadsData* append_quads_data) {
+  gfx::Transform transform = draw_transform();
+  gfx::Size rotated_size = bounds();
+
+  SharedQuadState* shared_quad_state =
+      render_pass->CreateAndAppendSharedQuadState();
+  shared_quad_state->SetAll(transform,
+                            rotated_size,
+                            visible_layer_rect(),
+                            clip_rect(),
+                            is_clipped(),
+                            draw_opacity(),
+                            blend_mode(),
+                            sorting_context_id());
+
+  gfx::Rect quad_rect(rotated_size);
+  gfx::Rect opaque_rect(contents_opaque() ? quad_rect : gfx::Rect());
+
+  Occlusion occlusion_in_video_space =
+      draw_properties()
+          .occlusion_in_content_space.GetOcclusionWithGivenDrawTransform(
+              transform);
+  gfx::Rect visible_quad_rect =
+      occlusion_in_video_space.GetUnoccludedContentRect(quad_rect);
+  if (visible_quad_rect.IsEmpty())
+    return;
+
+  gfx::Rect video_hole_rect = update_rect();
+
+  SrafVideoHoleDrawQuad* sraf_video_hole_draw_quad =
+      render_pass->CreateAndAppendDrawQuad<SrafVideoHoleDrawQuad>();
+
+  sraf_video_hole_draw_quad->SetAll(shared_quad_state,
+                                    quad_rect,
+                                    opaque_rect,
+                                    visible_quad_rect,
+                                    video_hole_rect,
+                                    true,
+                                    SK_ColorTRANSPARENT,
+                                    true);
+}
+
+const char* SrafVideoHoleLayerImpl::LayerTypeAsString() const {
+  return "cc::SrafVideoHoleLayerImpl";
+}
+
+} //namespace cc
diff --git a/src/cc/layers/sraf_video_hole_layer_impl.h b/src/cc/layers/sraf_video_hole_layer_impl.h
new file mode 100755
index 0000000..5762705
--- /dev/null
+++ b/src/cc/layers/sraf_video_hole_layer_impl.h
@@ -0,0 +1,35 @@
+//
+
+#ifndef CC_LAYERS_SRAF_VIDEO_HOLE_LAYER_IMPL_H_
+#define CC_LAYERS_SRAF_VIDEO_HOLE_LAYER_IMPL_H_
+
+#include "cc/layers/layer_impl.h"
+
+namespace cc {
+
+class CC_EXPORT SrafVideoHoleLayerImpl : public LayerImpl {
+ public:
+  static scoped_ptr<SrafVideoHoleLayerImpl> Create(LayerTreeImpl* tree_impl,
+                                                   int id);
+
+  ~SrafVideoHoleLayerImpl() override;
+
+  //LayerImpl implementation.
+  bool isVideoHoleLayer() override { return true; }
+
+  scoped_ptr<LayerImpl> CreateLayerImpl(LayerTreeImpl* tree_impl) override;
+
+  void AppendQuads(RenderPass* render_pass,
+                   AppendQuadsData* append_quads_data) override;
+
+ private:
+  SrafVideoHoleLayerImpl(LayerTreeImpl* tree_impl, int id);
+
+  const char* LayerTypeAsString() const override;
+
+  DISALLOW_COPY_AND_ASSIGN(SrafVideoHoleLayerImpl);
+};
+
+} //namespace cc
+
+#endif  // CC_LAYERS_SRAF_VIDEO_HOLE_LAYER_IMPL_H_
diff --git a/src/cc/output/gl_renderer.cc b/src/cc/output/gl_renderer.cc
index 536ab50..55e35bf 100644
--- a/src/cc/output/gl_renderer.cc
+++ b/src/cc/output/gl_renderer.cc
@@ -338,6 +338,10 @@ GLRenderer::GLRenderer(RendererClient* client,
       highp_threshold_cache_(0),
       use_sync_query_(false),
       on_demand_tile_raster_resource_id_(0),
+#if !defined(SRAF_VIDEO_HOLE_DISABLE)
+      has_video_hole_(false),
+      drew_video_render_pass_(false),
+#endif
       bound_geometry_(NO_BINDING) {
   DCHECK(gl_);
   DCHECK(context_support_);
@@ -533,6 +537,11 @@ void GLRenderer::DoDrawQuad(DrawingFrame* frame,
       DrawSolidColorQuad(frame, SolidColorDrawQuad::MaterialCast(quad),
                          clip_region);
       break;
+#if !defined(SRAF_VIDEO_HOLE_DISABLE)
+    case DrawQuad::SRAF_VIDEO_HOLE:
+      DrawVideoHoleQuad(frame, SrafVideoHoleDrawQuad::MaterialCast(quad));
+      break;
+#endif
     case DrawQuad::STREAM_VIDEO_CONTENT:
       DrawStreamVideoQuad(frame, StreamVideoDrawQuad::MaterialCast(quad),
                           clip_region);
@@ -1201,8 +1210,33 @@ void GLRenderer::DrawRenderPassQuad(DrawingFrame* frame,
 
   SetShaderOpacity(quad->shared_quad_state->opacity, locations.alpha);
   SetShaderQuadF(surface_quad, locations.quad);
+#if !defined(SRAF_VIDEO_HOLE_DISABLE)
+  if(has_video_hole_) {
+    if (quad->has_video_hole_draw_quad) {
+      drew_video_render_pass_ = true;
+      gl_->BlendFunc(GL_ONE, GL_ZERO);
+      DrawQuadGeometry(frame, quad->shared_quad_state->quad_to_target_transform,
+                       gfx::RectF(quad->rect), locations.matrix);
+      gl_->BlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
+    } else {
+      if (drew_video_render_pass_ && (frame->current_render_pass == frame->root_render_pass)) {
+        gl_->BlendFunc(GL_ONE, GL_ZERO);
+        DrawQuadGeometry(frame, quad->shared_quad_state->quad_to_target_transform,
+                         gfx::RectF(quad->rect), locations.matrix);
+        gl_->BlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
+      } else {
+        DrawQuadGeometry(frame, quad->shared_quad_state->quad_to_target_transform,
+                         gfx::RectF(quad->rect), locations.matrix);
+      }
+    }
+  } else {
+    DrawQuadGeometry(frame, quad->shared_quad_state->quad_to_target_transform,
+                     gfx::RectF(quad->rect), locations.matrix);
+  }
+#else
   DrawQuadGeometry(frame, quad->shared_quad_state->quad_to_target_transform,
                    gfx::RectF(quad->rect), locations.matrix);
+#endif
 
   // Flush the compositor context before the filter bitmap goes out of
   // scope, so the draw gets processed before the filter texture gets deleted.
@@ -1614,6 +1648,42 @@ void GLRenderer::DrawSolidColorQuad(const DrawingFrame* frame,
   }
 }
 
+#if !defined(SRAF_VIDEO_HOLE_DISABLE)
+void GLRenderer::DrawVideoHoleQuad(const DrawingFrame* frame,
+                                   const SrafVideoHoleDrawQuad* quad) {
+  if (quad->video_hole_rect.width() == 0 || quad->video_hole_rect.height() == 0) {
+    return;
+  }
+
+  has_video_hole_ = true;
+
+  gfx::Rect video_hole_rect = quad->video_hole_rect;
+  if (quad->shared_quad_state->is_clipped)
+    video_hole_rect.Intersect(quad->shared_quad_state->clip_rect);
+
+  if (video_hole_rect.y() <= current_surface_size_.height()) {
+    gl_->Disable(GL_SCISSOR_TEST);
+    gl_->Enable(GL_SCISSOR_TEST);
+    int offset_height = 0;
+    if (video_hole_rect.y() < 0) {
+      offset_height = video_hole_rect.height() + video_hole_rect.y();
+    } else {
+      offset_height = video_hole_rect.height();
+    }
+    video_hole_rect.set_y(current_surface_size_.height() - video_hole_rect.bottom());
+    gl_->Scissor(video_hole_rect.x(),
+                 video_hole_rect.y(),
+                 video_hole_rect.width(),
+                 offset_height);
+    gl_->ClearColor(0, 0, 0, 0);
+    gl_->Clear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
+    gl_->Disable(GL_SCISSOR_TEST);
+    gl_->Flush();
+    RestoreGLScissor();
+  }
+}
+#endif
+
 struct TileProgramUniforms {
   unsigned program;
   unsigned matrix_location;
@@ -2445,6 +2515,11 @@ void GLRenderer::FinishDrawingFrame(DrawingFrame* frame) {
   blend_shadow_ = false;
 
   ScheduleOverlays(frame);
+
+#if !defined(SRAF_VIDEO_HOLE_DISABLE)
+  has_video_hole_ = false;
+  drew_video_render_pass_ = false;
+#endif
 }
 
 void GLRenderer::FinishDrawingQuadList() {
@@ -3462,6 +3537,18 @@ void GLRenderer::ReinitializeGLState() {
   RestoreGLState();
 }
 
+#if !defined(SRAF_VIDEO_HOLE_DISABLE)
+void GLRenderer::RestoreGLScissor() {
+  if (is_scissor_enabled_) {
+    gl_->Enable(GL_SCISSOR_TEST);
+    gl_->Scissor(scissor_rect_.x(),
+                 scissor_rect_.y(),
+                 scissor_rect_.width(),
+                 scissor_rect_.height());
+  }
+}
+#endif
+
 void GLRenderer::RestoreGLState() {
   // This restores the current GLRenderer state to the GL context.
   bound_geometry_ = NO_BINDING;
diff --git a/src/cc/output/gl_renderer.h b/src/cc/output/gl_renderer.h
index a090324..cbb1983 100644
--- a/src/cc/output/gl_renderer.h
+++ b/src/cc/output/gl_renderer.h
@@ -17,6 +17,9 @@
 #include "cc/quads/io_surface_draw_quad.h"
 #include "cc/quads/render_pass_draw_quad.h"
 #include "cc/quads/solid_color_draw_quad.h"
+#if !defined(SRAF_VIDEO_HOLE_DISABLE)
+#include "cc/quads/sraf_video_hole_draw_quad.h"
+#endif
 #include "cc/quads/tile_draw_quad.h"
 #include "cc/quads/yuv_video_draw_quad.h"
 #include "ui/gfx/geometry/quad_f.h"
@@ -181,6 +184,11 @@ class CC_EXPORT GLRenderer : public DirectRenderer {
   void DrawSolidColorQuad(const DrawingFrame* frame,
                           const SolidColorDrawQuad* quad,
                           const gfx::QuadF* clip_region);
+#if !defined(SRAF_VIDEO_HOLE_DISABLE)
+  void DrawVideoHoleQuad(const DrawingFrame* frame,
+                         const SrafVideoHoleDrawQuad* quad);
+  void RestoreGLScissor();
+#endif
   void DrawStreamVideoQuad(const DrawingFrame* frame,
                            const StreamVideoDrawQuad* quad,
                            const gfx::QuadF* clip_region);
@@ -513,6 +521,10 @@ class CC_EXPORT GLRenderer : public DirectRenderer {
 
   SkBitmap on_demand_tile_raster_bitmap_;
   ResourceId on_demand_tile_raster_resource_id_;
+#if !defined(SRAF_VIDEO_HOLE_DISABLE)
+  bool has_video_hole_;
+  bool drew_video_render_pass_;
+#endif
   BoundGeometry bound_geometry_;
   DISALLOW_COPY_AND_ASSIGN(GLRenderer);
 };
diff --git a/src/cc/output/software_renderer.cc b/src/cc/output/software_renderer.cc
index ea980a8..583a7bd 100644
--- a/src/cc/output/software_renderer.cc
+++ b/src/cc/output/software_renderer.cc
@@ -315,6 +315,11 @@ void SoftwareRenderer::DoDrawQuad(DrawingFrame* frame,
     case DrawQuad::SOLID_COLOR:
       DrawSolidColorQuad(frame, SolidColorDrawQuad::MaterialCast(quad));
       break;
+#if !defined(SRAF_VIDEO_HOLE_DISABLE)
+    case DrawQuad::SRAF_VIDEO_HOLE:
+      DrawVideoHoleQuad(frame, SrafVideoHoleDrawQuad::MaterialCast(quad));
+      break;
+#endif
     case DrawQuad::TEXTURE_CONTENT:
       DrawTextureQuad(frame, TextureDrawQuad::MaterialCast(quad));
       break;
@@ -403,6 +408,19 @@ void SoftwareRenderer::DrawSolidColorQuad(const DrawingFrame* frame,
                             current_paint_);
 }
 
+#if !defined(SRAF_VIDEO_HOLE_DISABLE)
+void SoftwareRenderer::DrawVideoHoleQuad(const DrawingFrame* frame,
+                                         const SrafVideoHoleDrawQuad* quad) {
+  current_paint_.setXfermodeMode(SkXfermode::kSrc_Mode);
+  gfx::RectF visible_quad_vertex_rect = MathUtil::ScaleRectProportional(
+      QuadVertexRect(), gfx::RectF(quad->rect), gfx::RectF(quad->visible_rect));
+  current_paint_.setColor(quad->color);
+  current_paint_.setAlpha(0);
+  current_canvas_->drawRect(gfx::RectFToSkRect(visible_quad_vertex_rect),
+                            current_paint_);
+}
+#endif
+
 void SoftwareRenderer::DrawTextureQuad(const DrawingFrame* frame,
                                        const TextureDrawQuad* quad) {
   if (!IsSoftwareResource(quad->resource_id())) {
diff --git a/src/cc/output/software_renderer.h b/src/cc/output/software_renderer.h
index 339bf6f..1097091 100644
--- a/src/cc/output/software_renderer.h
+++ b/src/cc/output/software_renderer.h
@@ -9,6 +9,9 @@
 #include "cc/base/cc_export.h"
 #include "cc/output/compositor_frame.h"
 #include "cc/output/direct_renderer.h"
+#if !defined(SRAF_VIDEO_HOLE_DISABLE)
+#include "cc/quads/sraf_video_hole_draw_quad.h"
+#endif
 
 namespace cc {
 
@@ -85,6 +88,10 @@ class CC_EXPORT SoftwareRenderer : public DirectRenderer {
                           const RenderPassDrawQuad* quad);
   void DrawSolidColorQuad(const DrawingFrame* frame,
                           const SolidColorDrawQuad* quad);
+#if !defined(SRAF_VIDEO_HOLE_DISABLE)
+  void DrawVideoHoleQuad(const DrawingFrame* frame,
+                         const SrafVideoHoleDrawQuad* quad);
+#endif
   void DrawTextureQuad(const DrawingFrame* frame,
                        const TextureDrawQuad* quad);
   void DrawTileQuad(const DrawingFrame* frame,
diff --git a/src/cc/quads/draw_quad.h b/src/cc/quads/draw_quad.h
index 10b0e40..48a2fe7 100644
--- a/src/cc/quads/draw_quad.h
+++ b/src/cc/quads/draw_quad.h
@@ -40,6 +40,9 @@ class CC_EXPORT DrawQuad {
     PICTURE_CONTENT,
     RENDER_PASS,
     SOLID_COLOR,
+#if !defined(SRAF_VIDEO_HOLE_DISABLE)
+    SRAF_VIDEO_HOLE,
+#endif
     STREAM_VIDEO_CONTENT,
     SURFACE_CONTENT,
     TEXTURE_CONTENT,
diff --git a/src/cc/quads/render_pass.cc b/src/cc/quads/render_pass.cc
index dc79bb8..add864b 100644
--- a/src/cc/quads/render_pass.cc
+++ b/src/cc/quads/render_pass.cc
@@ -20,6 +20,9 @@
 #include "cc/quads/render_pass_draw_quad.h"
 #include "cc/quads/shared_quad_state.h"
 #include "cc/quads/solid_color_draw_quad.h"
+#if !defined(SRAF_VIDEO_HOLE_DISABLE)
+#include "cc/quads/sraf_video_hole_draw_quad.h"
+#endif
 #include "cc/quads/stream_video_draw_quad.h"
 #include "cc/quads/surface_draw_quad.h"
 #include "cc/quads/texture_draw_quad.h"
@@ -240,6 +243,11 @@ DrawQuad* RenderPass::CopyFromAndAppendDrawQuad(
     case DrawQuad::SOLID_COLOR:
       CopyFromAndAppendTypedDrawQuad<SolidColorDrawQuad>(quad);
       break;
+#if !defined(SRAF_VIDEO_HOLE_DISABLE)
+    case DrawQuad::SRAF_VIDEO_HOLE:
+      CopyFromAndAppendTypedDrawQuad<SrafVideoHoleDrawQuad>(quad);
+      break;
+#endif
     case DrawQuad::TILED_CONTENT:
       CopyFromAndAppendTypedDrawQuad<TileDrawQuad>(quad);
       break;
diff --git a/src/cc/quads/render_pass_draw_quad.cc b/src/cc/quads/render_pass_draw_quad.cc
index a0070d6..ec5e2c9 100644
--- a/src/cc/quads/render_pass_draw_quad.cc
+++ b/src/cc/quads/render_pass_draw_quad.cc
@@ -13,6 +13,9 @@
 namespace cc {
 
 RenderPassDrawQuad::RenderPassDrawQuad() {
+#if !defined(SRAF_VIDEO_HOLE_DISABLE)
+  has_video_hole_draw_quad = false;
+#endif
 }
 
 RenderPassDrawQuad::~RenderPassDrawQuad() {
diff --git a/src/cc/quads/render_pass_draw_quad.h b/src/cc/quads/render_pass_draw_quad.h
index c0d6b52..45a6efa 100644
--- a/src/cc/quads/render_pass_draw_quad.h
+++ b/src/cc/quads/render_pass_draw_quad.h
@@ -61,6 +61,10 @@ class CC_EXPORT RenderPassDrawQuad : public DrawQuad {
   // background of the render pass, from behind it.
   FilterOperations background_filters;
 
+#if !defined(SRAF_VIDEO_HOLE_DISABLE)
+  bool has_video_hole_draw_quad;
+#endif
+
   // Helper function to generate the normalized uv rect.
   gfx::RectF MaskUVRect() const;
 
diff --git a/src/cc/quads/sraf_video_hole_draw_quad.cc b/src/cc/quads/sraf_video_hole_draw_quad.cc
new file mode 100755
index 0000000..2898d11
--- /dev/null
+++ b/src/cc/quads/sraf_video_hole_draw_quad.cc
@@ -0,0 +1,57 @@
+//
+
+#include "cc/quads/sraf_video_hole_draw_quad.h"
+
+#include "base/logging.h"
+#include "base/trace_event/trace_event_argument.h"
+#include "base/values.h"
+#include "cc/base/math_util.h"
+
+namespace cc {
+
+SrafVideoHoleDrawQuad::SrafVideoHoleDrawQuad()
+    : color(0), force_anti_aliasing_off(false) {}
+
+void SrafVideoHoleDrawQuad::SetNew(const SharedQuadState* shared_quad_state,
+                                   const gfx::Rect& rect,
+                                   const gfx::Rect& visible_rect,
+                                   const gfx::Rect& video_hole_rect,
+                                   SkColor color,
+                                   bool force_anti_aliasing_off) {
+  gfx::Rect opaque_rect = SkColorGetA(color) == 255 ? rect : gfx::Rect();
+  bool needs_blending = false;
+  DrawQuad::SetAll(shared_quad_state, DrawQuad::SRAF_VIDEO_HOLE, rect, opaque_rect,
+                   visible_rect, needs_blending);
+  this->color = color;
+  this->force_anti_aliasing_off = force_anti_aliasing_off;
+  this->video_hole_rect = video_hole_rect;
+}
+
+void SrafVideoHoleDrawQuad::SetAll(const SharedQuadState* shared_quad_state,
+                                const gfx::Rect& rect,
+                                const gfx::Rect& opaque_rect,
+                                const gfx::Rect& visible_rect,
+                                const gfx::Rect& video_hole_rect,
+                                bool needs_blending,
+                                SkColor color,
+                                bool force_anti_aliasing_off) {
+  DrawQuad::SetAll(shared_quad_state, DrawQuad::SRAF_VIDEO_HOLE, rect, opaque_rect,
+                   visible_rect, needs_blending);
+  this->color = color;
+  this->force_anti_aliasing_off = force_anti_aliasing_off;
+  this->video_hole_rect = video_hole_rect;
+}
+
+const SrafVideoHoleDrawQuad* SrafVideoHoleDrawQuad::MaterialCast(
+    const DrawQuad* quad) {
+  DCHECK(quad->material == DrawQuad::SRAF_VIDEO_HOLE);
+  return static_cast<const SrafVideoHoleDrawQuad*>(quad);
+}
+
+void SrafVideoHoleDrawQuad::ExtendValue(base::trace_event::TracedValue* value) const {
+  value->SetInteger("color", color);
+  value->SetBoolean("force_anti_aliasing_off", force_anti_aliasing_off);
+  MathUtil::AddToTracedValue("video_hole_rect", video_hole_rect, value);
+}
+
+}  // namespace cc
diff --git a/src/cc/quads/sraf_video_hole_draw_quad.h b/src/cc/quads/sraf_video_hole_draw_quad.h
new file mode 100755
index 0000000..fcbbe9a
--- /dev/null
+++ b/src/cc/quads/sraf_video_hole_draw_quad.h
@@ -0,0 +1,45 @@
+//
+
+#ifndef CC_QUADS_SRAF_VIDEO_HOLE_DRAW_QUAD_H_
+#define CC_QUADS_SRAF_VIDEO_HOLE_DRAW_QUAD_H_
+
+#include "base/memory/scoped_ptr.h"
+#include "cc/base/cc_export.h"
+#include "cc/quads/draw_quad.h"
+#include "third_party/skia/include/core/SkColor.h"
+
+namespace cc {
+
+class CC_EXPORT SrafVideoHoleDrawQuad : public DrawQuad {
+ public:
+  SrafVideoHoleDrawQuad();
+
+  void SetNew(const SharedQuadState* shared_quad_state,
+              const gfx::Rect& rect,
+              const gfx::Rect& visible_rect,
+              const gfx::Rect& video_hole_rect,
+              SkColor color,
+              bool force_anti_aliasing_off);
+
+  void SetAll(const SharedQuadState* shared_quad_state,
+              const gfx::Rect& rect,
+              const gfx::Rect& opaque_rect,
+              const gfx::Rect& visible_rect,
+              const gfx::Rect& video_hole_rect,
+              bool needs_blending,
+              SkColor color,
+              bool force_anti_aliasing_off);
+
+  SkColor color;
+  bool force_anti_aliasing_off;
+  gfx::Rect video_hole_rect;
+
+  static const SrafVideoHoleDrawQuad* MaterialCast(const DrawQuad*);
+
+ private:
+  void ExtendValue(base::trace_event::TracedValue* value) const override;
+};
+
+}  // namespace cc
+
+#endif  // CC_QUADS_SRAF_VIDEO_HOLE_DRAW_QUAD_H_
diff --git a/src/content/common/cc_messages.cc b/src/content/common/cc_messages.cc
index f05a253..7163d3d 100644
--- a/src/content/common/cc_messages.cc
+++ b/src/content/common/cc_messages.cc
@@ -335,6 +335,11 @@ void ParamTraits<cc::RenderPass>::Write(
       case cc::DrawQuad::SOLID_COLOR:
         WriteParam(m, *cc::SolidColorDrawQuad::MaterialCast(quad));
         break;
+#if !defined(SRAF_VIDEO_HOLE_DISABLE)
+      case cc::DrawQuad::SRAF_VIDEO_HOLE:
+        WriteParam(m, *cc::SrafVideoHoleDrawQuad::MaterialCast(quad));
+        break;
+#endif
       case cc::DrawQuad::SURFACE_CONTENT:
         WriteParam(m, *cc::SurfaceDrawQuad::MaterialCast(quad));
         break;
@@ -455,6 +460,11 @@ bool ParamTraits<cc::RenderPass>::Read(const Message* m,
       case cc::DrawQuad::SOLID_COLOR:
         draw_quad = ReadDrawQuad<cc::SolidColorDrawQuad>(m, iter, p);
         break;
+#if !defined(SRAF_VIDEO_HOLE_DISABLE)
+      case cc::DrawQuad::SRAF_VIDEO_HOLE:
+        draw_quad = ReadDrawQuad<cc::SrafVideoHoleDrawQuad>(m, iter, p);
+        break;
+#endif
       case cc::DrawQuad::TILED_CONTENT:
         draw_quad = ReadDrawQuad<cc::TileDrawQuad>(m, iter, p);
         break;
@@ -545,6 +555,11 @@ void ParamTraits<cc::RenderPass>::Log(
       case cc::DrawQuad::SOLID_COLOR:
         LogParam(*cc::SolidColorDrawQuad::MaterialCast(quad), l);
         break;
+#if !defined(SRAF_VIDEO_HOLE_DISABLE)
+      case cc::DrawQuad::SRAF_VIDEO_HOLE:
+        LogParam(*cc::SrafVideoHoleDrawQuad::MaterialCast(quad), l);
+        break;
+#endif
       case cc::DrawQuad::SURFACE_CONTENT:
         LogParam(*cc::SurfaceDrawQuad::MaterialCast(quad), l);
         break;
diff --git a/src/content/common/cc_messages.h b/src/content/common/cc_messages.h
index d6da602..dfc7890 100644
--- a/src/content/common/cc_messages.h
+++ b/src/content/common/cc_messages.h
@@ -17,6 +17,9 @@
 #include "cc/quads/render_pass_draw_quad.h"
 #include "cc/quads/shared_quad_state.h"
 #include "cc/quads/solid_color_draw_quad.h"
+#if !defined(SRAF_VIDEO_HOLE_DISABLE)
+#include "cc/quads/sraf_video_hole_draw_quad.h"
+#endif
 #include "cc/quads/stream_video_draw_quad.h"
 #include "cc/quads/surface_draw_quad.h"
 #include "cc/quads/texture_draw_quad.h"
@@ -198,6 +201,9 @@ IPC_STRUCT_TRAITS_BEGIN(cc::RenderPassDrawQuad)
   IPC_STRUCT_TRAITS_MEMBER(filters)
   IPC_STRUCT_TRAITS_MEMBER(filters_scale)
   IPC_STRUCT_TRAITS_MEMBER(background_filters)
+#if !defined(SRAF_VIDEO_HOLE_DISABLE)
+  IPC_STRUCT_TRAITS_MEMBER(has_video_hole_draw_quad)
+#endif
 IPC_STRUCT_TRAITS_END()
 
 IPC_STRUCT_TRAITS_BEGIN(cc::SolidColorDrawQuad)
@@ -206,6 +212,15 @@ IPC_STRUCT_TRAITS_BEGIN(cc::SolidColorDrawQuad)
   IPC_STRUCT_TRAITS_MEMBER(force_anti_aliasing_off)
 IPC_STRUCT_TRAITS_END()
 
+#if !defined(SRAF_VIDEO_HOLE_DISABLE)
+IPC_STRUCT_TRAITS_BEGIN(cc::SrafVideoHoleDrawQuad)
+  IPC_STRUCT_TRAITS_PARENT(cc::DrawQuad)
+  IPC_STRUCT_TRAITS_MEMBER(color)
+  IPC_STRUCT_TRAITS_MEMBER(force_anti_aliasing_off)
+  IPC_STRUCT_TRAITS_MEMBER(video_hole_rect)
+IPC_STRUCT_TRAITS_END()
+#endif
+
 IPC_STRUCT_TRAITS_BEGIN(cc::StreamVideoDrawQuad)
   IPC_STRUCT_TRAITS_PARENT(cc::DrawQuad)
   IPC_STRUCT_TRAITS_MEMBER(overlay_resources)
diff --git a/src/content/renderer/npapi/webplugin_impl.cc b/src/content/renderer/npapi/webplugin_impl.cc
index f02d0d7..39d20dc 100644
--- a/src/content/renderer/npapi/webplugin_impl.cc
+++ b/src/content/renderer/npapi/webplugin_impl.cc
@@ -18,6 +18,9 @@
 #include "base/thread_task_runner_handle.h"
 #include "cc/blink/web_layer_impl.h"
 #include "cc/layers/io_surface_layer.h"
+#if !defined(SRAF_VIDEO_HOLE_DISABLE)
+#include "cc/layers/sraf_video_hole_layer.h"
+#endif
 #include "content/child/appcache/web_application_cache_host_impl.h"
 #include "content/child/multipart_response_delegate.h"
 #include "content/child/npapi/plugin_host.h"
@@ -277,10 +280,24 @@ bool WebPluginImpl::initialize(WebPluginContainer* container) {
 
   delegate_ = plugin_delegate;
 
+#if !defined(SRAF_VIDEO_HOLE_DISABLE)
+  if (!sraf_web_layer_.get() && container_ && ("video/broadcast" == mime_type_)) {
+    sraf_web_layer_.reset(new cc_blink::WebLayerImpl(
+        cc::SrafVideoHoleLayer::Create(cc_blink::WebLayerImpl::LayerSettings())));
+    container_->setWebLayer(sraf_web_layer_.get());
+  }
+#endif
+
   return true;
 }
 
 void WebPluginImpl::destroy() {
+#if !defined(SRAF_VIDEO_HOLE_DISABLE)
+  if (container_) {
+    container_->setWebLayer(NULL);
+  }
+  sraf_web_layer_.reset();
+#endif
   SetContainer(NULL);
   base::MessageLoop::current()->DeleteSoon(FROM_HERE, this);
 }
@@ -393,6 +410,11 @@ void WebPluginImpl::updateGeometry(const WebRect& window_rect,
   geometry_ = new_geometry;
 #endif  // OS_WIN
   first_geometry_update_ = false;
+
+#if !defined(SRAF_VIDEO_HOLE_DISABLE)
+  if (sraf_web_layer_)
+    sraf_web_layer_->invalidateRect(gfx::Rect(window_rect));
+#endif
 }
 
 void WebPluginImpl::updateFocus(bool focused, blink::WebFocusType focus_type) {
diff --git a/src/content/renderer/npapi/webplugin_impl.h b/src/content/renderer/npapi/webplugin_impl.h
old mode 100644
new mode 100755
index 517157f..70d4a4e
--- a/src/content/renderer/npapi/webplugin_impl.h
+++ b/src/content/renderer/npapi/webplugin_impl.h
@@ -280,6 +280,9 @@ class WebPluginImpl : public WebPlugin,
   scoped_refptr<cc::IOSurfaceLayer> io_surface_layer_;
   scoped_ptr<blink::WebLayer> web_layer_;
 #endif
+#if !defined(SRAF_VIDEO_HOLE_DISABLE)
+  scoped_ptr<blink::WebLayer> sraf_web_layer_;
+#endif
   bool accepts_input_events_;
   RenderFrameImpl* render_frame_;
   base::WeakPtr<RenderViewImpl> render_view_;
