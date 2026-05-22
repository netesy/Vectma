#include "core/SceneGraph.hpp"
#include "core/RectNode.hpp"
#include "sync/CRDTTypes.hpp"
#include "sync/DeltaFrameCodec.hpp"
#include "sync/NetworkReconciliationEngine.hpp"
#include <cassert>
#include <iostream>
#include <vector>
#include <thread>

using namespace vectma;

void testLWWConvergence() {
    std::cout << "Testing LWW Property Convergence..." << std::endl;

    LWWProperty<GColor> colorProp;
    LamportTimestamp t1{100, 1, 1};
    LamportTimestamp t2{200, 2, 1};

    colorProp.update(GColor::Black(), t1);
    assert(colorProp.value.r == 0);

    colorProp.update(GColor::White(), t2);
    assert(colorProp.value.r == 255);

    LamportTimestamp t3{150, 1, 2};
    colorProp.update(GColor::Black(), t3);
    assert(colorProp.value.r == 255);

    std::cout << "LWW Convergence tests passed." << std::endl;
}

void testConcurrentMutations() {
    std::cout << "Testing Concurrent Mutations (Scenario A)..." << std::endl;

    SceneGraph scene;
    auto rect = std::make_unique<RectNode>(0, 0, 100, 100);
    NodeId id{50, 1, 0};
    rect->setIdRemote(id, {50, 1, 0});
    scene.addChildRemote(std::move(rect), {50, 1, 0});

    CanvasNode* node = scene.findNodeById(id);
    assert(node != nullptr);

    LamportTimestamp tsA{100, 1, 5};
    node->setFillColorRemote(GColor(255, 0, 0), tsA);

    LamportTimestamp tsB{110, 2, 3};
    if (auto* r = dynamic_cast<RectNode*>(node)) {
        r->setWRemote(200, tsB);
        r->setHRemote(200, tsB);
    }

    assert(node->getFillColor().r == 255);
    if (auto* r = dynamic_cast<RectNode*>(node)) {
        assert(r->getW() == 200);
    }

    std::cout << "Concurrent Mutation Scenario A passed." << std::endl;
}

void testDeleteVsUpdate() {
    std::cout << "Testing Delete vs Update Conflict (Scenario B)..." << std::endl;

    SceneGraph scene;
    NodeId id{50, 1, 0};
    auto rect = std::make_unique<RectNode>(0, 0, 100, 100);
    rect->setIdRemote(id, {50, 1, 0});
    scene.addChildRemote(std::move(rect), {50, 1, 0});

    LamportTimestamp tsDelete{200, 1, 10};
    scene.removeChildRemote(id, tsDelete);

    CanvasNode* node = scene.findNodeById(id);
    assert(node == nullptr);

    auto rect2 = std::make_unique<RectNode>(0, 0, 100, 100);
    rect2->setIdRemote(id, {50, 1, 0});
    scene.addChildRemote(std::move(rect2), {150, 2, 6});
    assert(scene.findNodeById(id) == nullptr);

    std::cout << "Delete vs Update Scenario B passed." << std::endl;
}

int main() {
    LamportClock::getInstance().setClientId(1);
    testLWWConvergence();
    testConcurrentMutations();
    testDeleteVsUpdate();
    std::cout << "All CRDT Sync tests passed!" << std::endl;
    return 0;
}
