#pragma once
#include <string>
#include <vector>
#include <memory>
#include "AUI/Core/AProperty.hpp"

namespace aui {

class AView {
public:
    virtual ~AView();
    virtual void render();

    void setStyle(const std::string& style);
    virtual void onMouseDown(float x, float y);
    virtual void onMouseMove(float x, float y);
    virtual void onMouseUp();

protected:
    std::string m_style;
};

} // namespace aui
