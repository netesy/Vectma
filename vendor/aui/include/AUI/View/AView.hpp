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
    virtual void onMouseDown(float x, float y) { (void)x; (void)y; }
    virtual void onMouseMove(float x, float y) { (void)x; (void)y; }
    virtual void onMouseUp() { }

protected:
    std::string m_style;
};

} // namespace aui
