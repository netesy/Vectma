#pragma once

#include <vector>
#include <memory>
#include <string>
#include <vendor/stb/stb_truetype.h>

namespace vectma {

class FontAsset {
public:
    FontAsset(const std::vector<unsigned char>& data) : m_data(data) {
        stbtt_InitFont(&m_info, m_data.data(), 0);
    }

    const stbtt_fontinfo& getInfo() const { return m_info; }

private:
    std::vector<unsigned char> m_data;
    stbtt_fontinfo m_info;
};

} // namespace vectma
