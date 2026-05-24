#include <cassert>
#include <iostream>
#include "core/ArtboardNode.hpp"
#include "core/RectNode.hpp"

using namespace vectma;

int main() {
    auto artboard = std::make_unique<ArtboardNode>("Mobile", GRect(100, 100, 400, 800));
    auto rect = std::make_unique<RectNode>(10, 10, 50, 50);
    artboard->addChild(std::move(rect));

    assert(artboard->containsPoint(GPoint(115, 115)) == true);
    assert(artboard->containsPoint(GPoint(50, 50)) == false);

    std::cout << "test_canvas_minimal passed" << std::endl;
    return 0;
}
