#pragma once
#include <string>
#include <vector>
#include <memory>
#include "AUI/Core/AProperty.hpp"

namespace aui {

class AView {
public:
    virtual ~AView() = default;
    virtual void render() {} // Not pure virtual anymore to allow make_shared<AView>

    void setStyle(const std::string& style) { m_style = style; }

protected:
    std::string m_style;
};

} // namespace aui
