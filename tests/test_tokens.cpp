#include <cassert>
#include <iostream>
#include <stdexcept>
#include <set>
#include "style/TokenRegistry.hpp"
#include "core/GColor.hpp"
#include "core/RectNode.hpp"
#include "core/WorkspaceStage.hpp"
#include "layout/LayoutSolver.hpp"

using namespace vectma;

void testTokenResolution() {
    std::cout << "Starting testTokenResolution..." << std::endl;
    auto& tr = TokenRegistry::getInstance();
    tr.clear();
    tr.switchTheme(ThemeType::Light);

    tr.registerColorToken("palette.blue.500", GColor(0, 0, 255));
    tr.registerColorToken("palette.blue.900", GColor(0, 0, 100), GColor(0, 0, 50));

    tr.aliasToken("color.brand", "palette.blue.500");

    assert(tr.resolveAs<GColor>("color.brand") == GColor(0, 0, 255));

    tr.switchTheme(ThemeType::Dark);
    assert(tr.resolveAs<GColor>("palette.blue.900") == GColor(0, 0, 50));

    std::cout << "testTokenResolution passed" << std::endl;
}

void testCycleDetection() {
    std::cout << "Starting testCycleDetection..." << std::endl;
    auto& tr = TokenRegistry::getInstance();
    tr.clear();

    tr.registerDimensionToken("token.a", 10.0);
    tr.registerDimensionToken("token.b", 20.0);

    tr.aliasToken("token.a", "token.b");
    tr.aliasToken("token.b", "token.a");

    assert(tr.detectReferenceCycles() == true);

    try {
        tr.resolve("token.a");
        assert(false && "Should have thrown runtime_error");
    } catch (const std::runtime_error& e) {
        std::cout << "Caught expected cycle error: " << e.what() << std::endl;
    }

    std::cout << "testCycleDetection passed" << std::endl;
}

void testPropertyBinding() {
    std::cout << "Starting testPropertyBinding..." << std::endl;
    auto& tr = TokenRegistry::getInstance();
    tr.clear();
    tr.registerColorToken("color.primary", GColor(255, 0, 0), GColor(0, 255, 0));

    {
        RectNode node(0, 0, 100, 100);
        tr.switchTheme(ThemeType::Light);
        node.setFillColorToken("color.primary");

        GColor col = node.getFillColor();
        assert(col == GColor(255, 0, 0));

        tr.switchTheme(ThemeType::Dark);
        assert(node.getFillColor() == GColor(0, 255, 0));
    }

    std::cout << "testPropertyBinding passed" << std::endl;
}

int main() {
    testTokenResolution();
    testCycleDetection();
    testPropertyBinding();
    std::cout << "All Token tests passed!" << std::endl;
    return 0;
}
