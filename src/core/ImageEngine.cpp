#include "core/ImageEngine.hpp"

#ifdef VECTMA_USE_SKIA
#include "include/core/SkCodec.h"
#include "include/core/SkData.h"
#include "include/core/SkImage.h"
#include "include/encode/SkPngEncoder.h"
#include "include/encode/SkJpegEncoder.h"
#include "include/encode/SkWebpEncoder.h"

namespace vectma {

std::optional<ImageHandle> ImageEngine::DecodeImage(const std::vector<uint8_t>& data) {
    sk_sp<SkData> skData = SkData::MakeWithCopy(data.data(), data.size());
    if (!skData) return std::nullopt;

    std::unique_ptr<SkCodec> codec = SkCodec::MakeFromData(skData);
    if (!codec) return std::nullopt;

    auto [image, result] = codec->getImage();
    if (result != SkCodec::Result::kSuccess) return std::nullopt;

    return image;
}

std::vector<uint8_t> ImageEngine::EncodeImage(ImageHandle image, Format format, float quality) {
    if (!image) return {};

    SkDynamicMemoryWStream stream;
    bool success = false;

    switch (format) {
        case Format::PNG: {
            SkPngEncoder::Options options;
            success = SkPngEncoder::Encode(&stream, image->makeRasterImage().get(), options);
            break;
        }
        case Format::JPEG: {
            SkJpegEncoder::Options options;
            options.fQuality = static_cast<int>(quality * 100);
            success = SkJpegEncoder::Encode(&stream, image->makeRasterImage().get(), options);
            break;
        }
        case Format::WebP: {
            SkWebpEncoder::Options options;
            options.fQuality = quality * 100;
            success = SkWebpEncoder::Encode(&stream, image->makeRasterImage().get(), options);
            break;
        }
    }

    if (!success) return {};

    sk_sp<SkData> data = stream.detachAsData();
    if (!data) return {};

    return std::vector<uint8_t>(static_cast<const uint8_t*>(data->data()), static_cast<const uint8_t*>(data->data()) + data->size());
}

} // namespace vectma

#else
namespace vectma {
std::optional<ImageHandle> ImageEngine::DecodeImage(const std::vector<uint8_t>& data) { (void)data; return std::nullopt; }
std::vector<uint8_t> ImageEngine::EncodeImage(ImageHandle image, Format format, float quality) { (void)image; (void)format; (void)quality; return {}; }
}
#endif
