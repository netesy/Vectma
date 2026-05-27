#include "network/AssetFetchEngine.hpp"
#include "export/ExportEngine.hpp"
#include "core/RectNode.hpp"
#include "core/SceneGraph.hpp"
#include <cassert>
#include <iostream>

using namespace vectma;

void test_export_svg() {
    auto scene = std::make_shared<SceneGraph>();
    scene->addChild(std::make_unique<RectNode>(0, 0, 100, 100));

    std::string svg = export_engine::ExportEngine::exportToSVG(scene.get());
    assert(svg.find("<svg") != std::string::npos);
    assert(svg.find("<rect") != std::string::npos);
    std::cout << "test_export_svg passed." << std::endl;
}

void test_asset_fetch_sim() {
    (void)network::AssetFetchEngine::getInstance();
    std::cout << "test_asset_fetch_sim passed (structural check)." << std::endl;
}

int main() {
    test_export_svg();
    test_asset_fetch_sim();
    return 0;
}
