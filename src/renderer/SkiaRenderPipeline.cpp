#include "renderer/SkiaRenderPipeline.hpp"
#include "core/RectNode.hpp"
#include "core/EllipseNode.hpp"
#include "core/PathNode.hpp"
#include "core/TextNode.hpp"
#include "core/ImageNode.hpp"
#include "core/ImageEngine.hpp"

#ifdef VECTMA_USE_SKIA
#include "include/core/SkCanvas.h"
#include "include/core/SkPath.h"
#include "include/core/SkPaint.h"
#include "include/core/SkShader.h"
#include "include/effects/SkRuntimeEffect.h"
#include "include/core/SkImage.h"
#include "include/core/SkData.h"
#include "include/core/SkBlendMode.h"
#include "modules/skparagraph/include/ParagraphBuilder.h"
#include "modules/skparagraph/include/ParagraphStyle.h"
#include "modules/skparagraph/include/FontCollection.h"
#include "include/core/SkFontMgr.h"

namespace vectma {

SkiaRenderPipeline::SkiaRenderPipeline(SkCanvas* canvas) : m_canvas(canvas) {
    static const char* sksl = R"(
        uniform float2 u_pos;
        uniform float u_radius;
        uniform half4 u_color;

        half4 main(float2 fragcoord) {
            float d = distance(fragcoord, u_pos);
            return u_color * smoothstep(u_radius, u_radius * 0.8, d);
        }
    )";

    auto [effect, error] = SkRuntimeEffect::MakeForShader(SkString(sksl));
    if (effect) {
        m_brushEffect = effect;
    } else {
        std::cerr << "SkSL Error: " << error.c_str() << std::endl;
    }
}

void SkiaRenderPipeline::beginFrame() {
    m_globalOpacity = 1.0f;
}

void SkiaRenderPipeline::endFrame() {
}

SkPath SkiaRenderPipeline::TranslateToSkPath(const PathData& nativePath) {
    SkPath path;
    for (const auto& contour : nativePath.contours) {
        if (contour.anchors.empty()) continue;
        const auto& anchors = contour.anchors;
        path.moveTo(anchors[0].position.x, anchors[0].position.y);
        for (size_t i = 0; i < anchors.size() - 1; ++i) {
            path.cubicTo(anchors[i].handleOut.x, anchors[i].handleOut.y,
                         anchors[i+1].handleIn.x, anchors[i+1].handleIn.y,
                         anchors[i+1].position.x, anchors[i+1].position.y);
        }
        if (contour.isClosed && anchors.size() > 1) {
            path.cubicTo(anchors.back().handleOut.x, anchors.back().handleOut.y,
                         anchors.front().handleIn.x, anchors.front().handleIn.y,
                         anchors.front().position.x, anchors.front().position.y);
            path.close();
        }
    }
    return path;
}

static SkBlendMode MapBlendMode(BlendMode mode) {
    switch (mode) {
        case BlendMode::Normal:     return SkBlendMode::kSrcOver;
        case BlendMode::Multiply:   return SkBlendMode::kMultiply;
        case BlendMode::Screen:     return SkBlendMode::kScreen;
        case BlendMode::Overlay:    return SkBlendMode::kOverlay;
        case BlendMode::Darken:     return SkBlendMode::kDarken;
        case BlendMode::Lighten:    return SkBlendMode::kLighten;
        case BlendMode::ColorDodge: return SkBlendMode::kColorDodge;
        case BlendMode::ColorBurn:  return SkBlendMode::kColorBurn;
        case BlendMode::HardLight:  return SkBlendMode::kHardLight;
        case BlendMode::SoftLight:  return SkBlendMode::kSoftLight;
        case BlendMode::Difference: return SkBlendMode::kDifference;
        case BlendMode::Exclusion:  return SkBlendMode::kExclusion;
        default:                    return SkBlendMode::kSrcOver;
    }
}

void SkiaRenderPipeline::drawRect(const RectNode& node, FillType fillType, const GradientConfig& gradConfig, StrokeAlignment strokeAlign) {
    (void)fillType; (void)gradConfig; (void)strokeAlign;
    SkPaint paint;
    paint.setColor(node.getFillColor().toSkColor());
    paint.setAlphaf(m_globalOpacity * node.getOpacity());
    paint.setBlendMode(MapBlendMode(node.getBlendMode()));
    m_canvas->drawRect(SkRect::MakeXYWH(node.getX(), node.getY(), node.getWidth(), node.getHeight()), paint);
}

void SkiaRenderPipeline::drawEllipse(const EllipseNode& node, FillType fillType, const GradientConfig& gradConfig, StrokeAlignment strokeAlign) {
    (void)fillType; (void)gradConfig; (void)strokeAlign;
    SkPaint paint;
    paint.setColor(node.getFillColor().toSkColor());
    paint.setAlphaf(m_globalOpacity * node.getOpacity());
    paint.setBlendMode(MapBlendMode(node.getBlendMode()));
    m_canvas->drawOval(SkRect::MakeXYWH(node.getX() - node.getRadiusX(), node.getY() - node.getRadiusY(),
                                      node.getRadiusX() * 2, node.getRadiusY() * 2), paint);
}

void SkiaRenderPipeline::drawPath(const PathNode& node, FillType fillType, const GradientConfig& gradConfig, StrokeAlignment strokeAlign) {
    (void)fillType; (void)gradConfig; (void)strokeAlign;
    SkPath path = TranslateToSkPath(node.getCompiledPath());
    SkPaint paint;
    paint.setAntiAlias(true);
    paint.setColor(node.getFillColor().toSkColor());
    paint.setAlphaf(m_globalOpacity * node.getOpacity());
    paint.setBlendMode(MapBlendMode(node.getBlendMode()));
    m_canvas->drawPath(path, paint);
}

void SkiaRenderPipeline::drawText(const TextNode& node) {
    sk_sp<skia::textlayout::FontCollection> fontCollection = sk_make_sp<skia::textlayout::FontCollection>();
    fontCollection->setDefaultFontManager(SkFontMgr::RefDefault());

    skia::textlayout::ParagraphStyle paraStyle;
    skia::textlayout::TextStyle textStyle;
    textStyle.setFontSize(node.getFontSize());
    textStyle.setColor(node.getFillColor().toSkColor());

    auto builder = skia::textlayout::ParagraphBuilder::make(paraStyle, fontCollection);
    builder->pushStyle(textStyle);
    builder->addText(node.getText().c_str());
    builder->pop();

    auto paragraph = builder->Build();
    paragraph->layout(10000);
    paragraph->paint(m_canvas, node.getX(), node.getY());
}

void SkiaRenderPipeline::drawImage(const ImageNode& node) {
    sk_sp<SkData> data = SkData::MakeWithCopy(node.getRawData().data(), node.getRawData().size());
    if (!data) return;

    sk_sp<SkImage> image = SkImages::DeferredFromEncodedData(data);
    if (!image) return;

    SkPaint paint;
    paint.setAlphaf(m_globalOpacity * node.getOpacity());
    paint.setBlendMode(MapBlendMode(node.getBlendMode()));
    m_canvas->drawImageRect(image, SkRect::MakeXYWH(node.getX(), node.getY(), node.getWidth(), node.getHeight()), SkSamplingOptions(), &paint);
}

void SkiaRenderPipeline::drawBezierPath(const PathNode& node) {
    drawPath(node, FillType::Solid, {}, StrokeAlignment::Center);
}

void SkiaRenderPipeline::drawAnchorOverlay(const BezierAnchor& anchor, bool selected, int activeHandle) {
    (void)anchor; (void)selected; (void)activeHandle;
}

void SkiaRenderPipeline::renderNode(const CanvasNode& node) {
    node.render(*this);
}

std::vector<uint8_t> SkiaRenderPipeline::exportRaster(float scale) {
    (void)scale;
    return {};
}

void SkiaRenderPipeline::setStrokeStyle(const std::vector<float>& dashPattern, float offset) {
    (void)dashPattern; (void)offset;
}

void SkiaRenderPipeline::drawSnappingGuide(const Point2D& start, const Point2D& end) {
    SkPaint paint;
    paint.setColor(SK_ColorCYAN);
    paint.setStrokeWidth(1.0f);
    m_canvas->drawLine(start.x, start.y, end.x, end.y, paint);
}

void SkiaRenderPipeline::pushClipRect(const GRect& rect) {
    m_canvas->save();
    m_canvas->clipRect(SkRect::MakeXYWH(rect.x, rect.y, rect.width, rect.height));
}

void SkiaRenderPipeline::popClipRect() {
    m_canvas->restore();
}

void SkiaRenderPipeline::setGlobalOpacity(float opacity) {
    m_globalOpacity = opacity;
}

void SkiaRenderPipeline::drawBrushStroke(const std::vector<BrushPoint>& strokeBuffer, const BrushSettings& settings) {
    if (!m_brushEffect || strokeBuffer.empty()) return;

    for (const auto& pt : strokeBuffer) {
        SkRuntimeShaderBuilder builder(m_brushEffect);
        builder.uniform("u_pos") = SkV2{ (float)pt.position.x, (float)pt.position.y };
        builder.uniform("u_radius") = settings.size * pt.pressure;
        builder.uniform("u_color") = SkV4{ (float)settings.color.r, (float)settings.color.g, (float)settings.color.b, (float)settings.color.a };

        SkPaint paint;
        paint.setShader(builder.makeShader());
        m_canvas->drawCircle(pt.position.x, pt.position.y, settings.size * pt.pressure, paint);
    }
}

} // namespace vectma
#endif
