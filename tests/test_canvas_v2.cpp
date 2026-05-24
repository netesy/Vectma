#include <cassert>
#include <iostream>
#include <cmath>
#include "core/ArtboardNode.hpp"
#include "core/RectNode.hpp"
#include "core/WorkspaceStage.hpp"
#include "renderer/RenderPipeline.hpp"

using namespace vectma;

int main() {
    std::cout << "Running minimal canvas test..." << std::endl;
    auto artboard = std::make_unique<ArtboardNode>("Mobile", GRect(100, 100, 400, 800));
    assert(artboard->getName() == "Mobile");
    std::cout << "Done." << std::endl;
    return 0;
}
