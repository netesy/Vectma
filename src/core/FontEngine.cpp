#include "core/FontEngine.hpp"

#ifdef VECTMA_USE_WOFF2
#include <vendor/woff2/decode.h>
#include <vendor/brotli/decode.h>
#endif

#ifdef VECTMA_USE_SKIA
#include "include/core/SkStream.h"
#include "include/core/SkTypeface.h"
#include "modules/skparagraph/include/ParagraphBuilder.h"
#include "modules/skparagraph/include/ParagraphStyle.h"
#include "modules/skparagraph/include/FontCollection.h"

namespace vectma {

std::optional<TypefaceHandle> FontEngine::LoadFont(const std::vector<uint8_t>& fontData) {
    std::vector<uint8_t> ttfData;
    const uint8_t* dataPtr = fontData.data();
    size_t dataSize = fontData.size();

#ifdef VECTMA_USE_WOFF2
    // WOFF2 detection
    if (dataSize >= 4 && dataPtr[0] == 'w' && dataPtr[1] == 'O' && dataPtr[2] == 'F' && dataPtr[3] == '2') {
        size_t outSize = woff2::ComputeWOFF2FinalSize(dataPtr, dataSize);
        ttfData.resize(outSize);
        if (!woff2::ConvertWOFF2ToTTF(dataPtr, dataSize, ttfData.data(), &outSize)) {
            return std::nullopt;
        }
        ttfData.resize(outSize);
        dataPtr = ttfData.data();
        dataSize = ttfData.size();
    }
#endif

    auto stream = std::make_unique<SkMemoryStream>(dataPtr, dataSize, true);
    return SkTypeface::MakeFromStream(std::move(stream));
}

void FontEngine::LayoutText(const LayoutParams& params) {
    sk_sp<skia::textlayout::FontCollection> fontCollection = sk_make_sp<skia::textlayout::FontCollection>();
    fontCollection->setDefaultFontManager(SkFontMgr::RefDefault());

    skia::textlayout::ParagraphStyle paraStyle;
    paraStyle.setLineHeight(params.lineHeight);

    skia::textlayout::TextStyle textStyle;
    textStyle.setFontSize(params.fontSize);
    textStyle.setLetterSpacing(params.letterSpacing);

    auto builder = skia::textlayout::ParagraphBuilder::make(paraStyle, fontCollection);
    builder->pushStyle(textStyle);
    builder->addText(params.text.c_str());
    builder->pop();

    auto paragraph = builder->Build();
    paragraph->layout(params.width);
}

} // namespace vectma

#else
namespace vectma {
std::optional<TypefaceHandle> FontEngine::LoadFont(const std::vector<uint8_t>& fontData) { (void)fontData; return std::nullopt; }
void FontEngine::LayoutText(const LayoutParams& params) { (void)params; }
}
#endif
