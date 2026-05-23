#include "geometry/BezierMath.hpp"
#include "core/PathNode.hpp"
#include <cassert>
#include <iostream>
using namespace vectma;
using namespace vectma::geometry;

void testHandleConstraints() {
    std::cout << "Testing Handle Constraints (Scenario A)..." << std::endl;
    AnchorPoint ap;
    ap.position = {100, 100};
    ap.relation = HandleRelation::Symmetric;
    ap.setHandleIn({50, 100});
    assert(std::abs(ap.getHandleOut().x - 150) < 1e-6);
    assert(std::abs(ap.getHandleOut().y - 100) < 1e-6);

    ap.relation = HandleRelation::Asymmetric;
    ap.setHandleOut({200, 100}); // lenOut = 100
    ap.setHandleIn({80, 100}); // lenIn = 20
    // Angle mirrored, lenIn preserved
    assert(std::abs(ap.getHandleOut().x - 200) < 1e-6);
    assert(std::abs(ap.getHandleIn().x - 80) < 1e-6);

    std::cout << "Handle Constraints passed." << std::endl;
}

void testPathTopology() {
    std::cout << "Testing Path Topology (Vector Network)..." << std::endl;
    PathTopology topo;
    AnchorPoint p1, p2;
    p1.position = {100, 100};
    p2.position = {200, 200};
    topo.points.push_back(p1);
    topo.points.push_back(p2);
    topo.edges.push_back({0, 1});

    PathNode path(topo);
    assert(path.getTopology().points.size() == 2);
    assert(path.getTopology().edges.size() == 1);

    std::cout << "Path Topology passed." << std::endl;
}

int main() {
    testHandleConstraints();
    testPathTopology();
    std::cout << "All Geometry tests passed!" << std::endl;
    return 0;
}
