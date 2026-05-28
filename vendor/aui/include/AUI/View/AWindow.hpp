#pragma once
#include "AUI/View/AView.hpp"
#include <string>
#include <memory>

namespace aui {

class AWindow {
public:
    AWindow(const std::string& title, int width, int height);
    virtual ~AWindow();
    void setContentView(std::shared_ptr<AView> view);
    virtual void render();

protected:
    std::string m_title;
    int m_width, m_height;
    std::shared_ptr<AView> m_contentView;
};

} // namespace aui
