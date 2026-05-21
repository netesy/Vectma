#pragma once

#include <vector>
#include <string>
#include <memory>
#include <optional>

#ifdef VECTMA_USE_SKIA
#include "include/core/SkImage.h"
#include "include/core/SkData.h"
#endif

namespace vectma {

#ifdef VECTMA_USE_SKIA
using ImageHandle = sk_sp<SkImage>;
#else
using ImageHandle = void*; // Placeholder
#endif

/**
 * @brief Manages image asset allocations and codec processing via Skia.
 */
class ImageEngine {
public:
    /**
     * @brief Decodes raw binary data into a Skia image handle.
     */
    static std::optional<ImageHandle> DecodeImage(const std::vector<uint8_t>& data);

    /**
     * @brief Encodes an image handle into a compressed byte array.
     */
    enum class Format { PNG, JPEG, WebP };
    static std::vector<uint8_t> EncodeImage(ImageHandle image, Format format, float quality = 0.8f);
};

} // namespace vectma
