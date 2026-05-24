#ifdef VECTMA_USE_SKIA
#include "renderer/SkiaRenderPipeline.hpp"
#include "core/RectNode.hpp"
#include "core/EllipseNode.hpp"
#include "core/PathNode.hpp"
#include "core/TextNode.hpp"
#include "core/ImageNode.hpp"
#include "core/CompoundShapeNode.hpp"
#include "core/modifiers/DashGeneratorModifier.hpp"
#include "include/core/SkColor.h"
#include "include/core/SkMatrix.h"
#include "include/core/SkData.h"
#include "include/core/SkImage.h"
#include "include/effects/SkRuntimeEffect.h"
#include "modules/skparagraph/include/ParagraphBuilder.h"
#include "modules/skparagraph/include/ParagraphStyle.h"
#include "modules/skparagraph/include/FontCollection.h"
#include "include/core/SkFontMgr.h"

namespace vectma {

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

static SkPathOp MapOp(BooleanOpType op) {
    switch (op) {
        case BooleanOpType::Union:     return kUnion_SkPathOp;
        case BooleanOpType::Subtract:  return kDifference_SkPathOp;
        case BooleanOpType::Intersect: return kIntersect_SkPathOp;
        case BooleanOpType::Exclude:   return kXOR_SkPathOp;
        default:                       return kUnion_SkPathOp;
    }
}

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
    if (effect) m_brushEffect = effect;
}

void SkiaRenderPipeline::beginFrame() { m_globalOpacity = 1.0f; m_overrideStack.clear(); }
void SkiaRenderPipeline::endFrame() {}

void SkiaRenderPipeline::pushTransform(const GTransform& transform) {
    m_canvas->save();
    m_canvas->concat(SkMatrix::MakeAll(transform.a, transform.c, transform.e, transform.b, transform.d, transform.f, 0, 0, 1));
}
void SkiaRenderPipeline::popTransform() { m_canvas->restore(); }

void SkiaRenderPipeline::pushOverrideContext(const OverrideMap* overrides) { m_overrideStack.push_back(overrides); }
void SkiaRenderPipeline::popOverrideContext() { if (!m_overrideStack.empty()) m_overrideStack.pop_back(); }

template <typename T>
T SkiaRenderPipeline_Resolve(const std::vector<const OverrideMap*>& stack, NodeId id, const std::string& prop, T val) {
    std::string key = std::to_string(id.timestamp) + "." + prop;
    for (auto it = stack.rbegin(); it != stack.rend(); ++it) {
        auto found = (*it)->find(key);
        if (found != (*it)->end()) {
            if (auto pval = std::get_if<T>(&found->second)) return *pval;
        }
    }
    return val;
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

void SkiaRenderPipeline::drawRect(const RectNode& node, FillType, const GradientConfig&, StrokeAlignment) {
    GColor fill = SkiaRenderPipeline_Resolve(m_overrideStack, node.getId(), "fill_color", node.getFillColor());
    float opacity = SkiaRenderPipeline_Resolve(m_overrideStack, node.getId(), "opacity", node.getOpacity());

    SkPaint paint;
    paint.setColor(fill.toSkColor());
    paint.setAlphaf(m_globalOpacity * opacity);
    paint.setBlendMode(MapBlendMode(node.getBlendMode()));
    m_canvas->drawRect(SkRect::MakeXYWH(node.getX(), node.getY(), node.getW(), node.getH()), paint);
}

void SkiaRenderPipeline::drawEllipse(const EllipseNode& node, FillType, const GradientConfig&, StrokeAlignment) {
    GColor fill = SkiaRenderPipeline_Resolve(m_overrideStack, node.getId(), "fill_color", node.getFillColor());
    float opacity = SkiaRenderPipeline_Resolve(m_overrideStack, node.getId(), "opacity", node.getOpacity());

    SkPaint paint;
    paint.setColor(fill.toSkColor());
    paint.setAlphaf(m_globalOpacity * opacity);
    paint.setBlendMode(MapBlendMode(node.getBlendMode()));
    m_canvas->drawOval(SkRect::MakeXYWH(node.getCX() - node.getRX(), node.getCY() - node.getRY(),
                                      node.getRX() * 2, node.getRY() * 2), paint);
}

void SkiaRenderPipeline::drawPath(const PathNode& node, FillType, const GradientConfig&, StrokeAlignment) {
    GColor fill = SkiaRenderPipeline_Resolve(m_overrideStack, node.getId(), "fill_color", node.getFillColor());
    float opacity = SkiaRenderPipeline_Resolve(m_overrideStack, node.getId(), "opacity", node.getOpacity());

    SkPath path = TranslateToSkPath(node.getCompiledPath());
    SkPaint paint;
    paint.setAntiAlias(true);
    paint.setColor(fill.toSkColor());
    paint.setAlphaf(m_globalOpacity * opacity);
    paint.setBlendMode(MapBlendMode(node.getBlendMode()));
    m_canvas->drawPath(path, paint);
}

void SkiaRenderPipeline::drawText(const TextNode& node) {
    std::string text = SkiaRenderPipeline_Resolve(m_overrideStack, node.getId(), "text", node.getText());
    GColor fill = SkiaRenderPipeline_Resolve(m_overrideStack, node.getId(), "fill_color", node.getFillColor());

    sk_sp<skia::textlayout::FontCollection> fontCollection = sk_make_sp<skia::textlayout::FontCollection>();
    fontCollection->setDefaultFontManager(SkFontMgr::RefDefault());
    skia::textlayout::ParagraphStyle paraStyle;
    skia::textlayout::TextStyle textStyle;
    textStyle.setFontSize(node.getFontSize());
    textStyle.setColor(fill.toSkColor());
    auto builder = skia::textlayout::ParagraphBuilder::make(paraStyle, fontCollection);
    builder->pushStyle(textStyle);
    builder->addText(text.c_str());
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

void SkiaRenderPipeline::drawCompoundShape(const CompoundShapeNode& node) {
    SkPath result;
    bool first = true;
    for (const auto& child : node.getChildren()) {
        if (!child->isVisible()) continue;
        SkPath childPath;
        if (auto* pathNode = dynamic_cast<const PathNode*>(child.get())) {
            childPath = TranslateToSkPath(pathNode->getCompiledPath());
        } else if (auto* rect = dynamic_cast<const RectNode*>(child.get())) {
            childPath.addRect(SkRect::MakeXYWH(rect->getX(), rect->getY(), rect->getW(), rect->getH()));
        }
        if (first) { result = childPath; first = false; }
        else { SkPath next; Op(result, childPath, MapOp(node.getOpType()), &next); result = next; }
    }
    SkPaint paint;
    paint.setAntiAlias(true);
    paint.setColor(node.getFillColor().toSkColor());
    paint.setAlphaf(m_globalOpacity * node.getOpacity());
    paint.setBlendMode(MapBlendMode(node.getBlendMode()));
    m_canvas->drawPath(result, paint);
}

void SkiaRenderPipeline::drawBezierPath(const PathNode& node) {
    drawPath(node, FillType::Solid, {}, StrokeAlignment::Center);
}

void SkiaRenderPipeline::drawAnchorOverlay(const BezierAnchor& anchor, bool selected, int activeHandle) {
    (void)anchor; (void)selected; (void)activeHandle;
}

void SkiaRenderPipeline::renderNode(const CanvasNode& node) { node.render(*this); }
std::vector<uint8_t> SkiaRenderPipeline::exportRaster(float) { return {}; }
void SkiaRenderPipeline::setStrokeStyle(const std::vector<float>&, float) {}
void SkiaRenderPipeline::drawSnappingGuide(const Point2D& start, const Point2D& end) {
    SkPaint paint; paint.setColor(SK_ColorCYAN); m_canvas->drawLine(start.x, start.y, end.x, end.y, paint);
}
void SkiaRenderPipeline::pushClipRect(const GRect& rect) { m_canvas->save(); m_canvas->clipRect(SkRect::MakeXYWH(rect.x, rect.y, rect.width, rect.height)); }
void SkiaRenderPipeline::popClipRect() { m_canvas->restore(); }
void SkiaRenderPipeline::setGlobalOpacity(float opacity) { m_globalOpacity = opacity; }

void SkiaRenderPipeline::drawBrushStroke(const std::vector<BrushPoint>& strokeBuffer, const BrushSettings& settings) {
    if (!m_brushEffect || strokeBuffer.empty()) return;
    for (const auto& pt : strokeBuffer) {
        SkRuntimeShaderBuilder builder(m_brushEffect);
        builder.uniform("u_pos") = SkV2{ (float)pt.position.x, (float)pt.position.y };
        builder.uniform("u_radius") = settings.size * pt.pressure;
        builder.uniform("u_color") = SkV4{ (float)settings.color.r/255.0f, (float)settings.color.g/255.0f, (float)settings.color.b/255.0f, (float)settings.color.a/255.0f };
        SkPaint paint; paint.setShader(builder.makeShader());
        m_canvas->drawCircle(pt.position.x, pt.position.y, settings.size * pt.pressure, paint);
    }
}

} // namespace vectma
#endif
