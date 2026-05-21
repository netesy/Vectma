#pragma once

#include <vector>
#include <string>
#include <memory>
#include <optional>

#ifdef VECTMA_USE_SKIA
#include "include/core/SkTypeface.h"
#include "modules/skottie/include/Skottie.h"
#include "modules/skparagraph/include/Paragraph.h"
#endif

namespace vectma {

#ifdef VECTMA_USE_SKIA
using TypefaceHandle = sk_sp<SkTypeface>;
#else
using TypefaceHandle = void*;
#endif

/**
 * @brief Manages font extraction, loading, and text layout via Skia & WOFF2.
 */
class FontEngine {
public:
    /**
     * @brief Converts WOFF2 data to TTF if necessary and creates a Skia typeface.
     */
    static std::optional<TypefaceHandle> LoadFont(const std::vector<uint8_t>& fontData);

    /**
     * @brief Executes professional text layout calculations.
     */
    struct LayoutParams {
        std::string text;
        float fontSize = 14.0f;
        float width = 100.0f;
        float letterSpacing = 0.0f;
        float lineHeight = 1.2f;
        // ... weights, colors etc
    };

    static void LayoutText(const LayoutParams& params);
};

} // namespace vectma
