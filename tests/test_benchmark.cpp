#include <iostream>
#include <chrono>
#include <cassert>
#include <vector>
#include <memory>
#include "core/SceneGraph.hpp"
#include "core/RectNode.hpp"
#include "style/TokenRegistry.hpp"
#include "layout/LayoutSolver.hpp"
#include "renderer/RenderPipeline.hpp"

using namespace vectma;

void runTokenRegistryBenchmark() {
    std::cout << "\n=== Running Style Token Resolution Benchmarks ===" << std::endl;
    auto& tr = TokenRegistry::getInstance();
    tr.clear();
    tr.registerColorToken("color.accent", GColor(100, 150, 200), GColor(10, 20, 30));

    TokenId id = tr.getOrCreateTokenId("color.accent");

    constexpr size_t ITERATIONS = 10000000; // 10 Million

    // Test A: Legacy string resolution
    auto startA = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < ITERATIONS; ++i) {
        GColor col = tr.resolveAs<GColor>("color.accent");
        (void)col;
    }
    auto endA = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> elapsedA = endA - startA;

    // Test B: DOD contiguous O(1) resolution
    auto startB = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < ITERATIONS; ++i) {
        GColor col = tr.resolveAs<GColor>(id);
        (void)col;
    }
    auto endB = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> elapsedB = endB - startB;

    std::cout << "Legacy string resolution: " << elapsedA.count() << " ms (" << (ITERATIONS / (elapsedA.count() / 1000.0)) / 1000000.0 << " million ops/sec)" << std::endl;
    std::cout << "DOD integer token interning: " << elapsedB.count() << " ms (" << (ITERATIONS / (elapsedB.count() / 1000.0)) / 1000000.0 << " million ops/sec)" << std::endl;
    std::cout << "Performance speedup: " << elapsedA.count() / elapsedB.count() << "x faster!" << std::endl;
}

void runLayoutSolverBenchmark() {
    std::cout << "\n=== Running Layout Constraints Resolution Benchmarks ===" << std::endl;
    auto container = std::make_unique<RectNode>(0, 0, 1000, 1000);
    LayoutProperties props;
    props.enabled = true;
    props.direction = LayoutDirection::Vertical;
    props.gap = 5.0f;
    container->setLayoutProps(props);

    for (int i = 0; i < 100; ++i) {
        container->addChild(std::make_unique<RectNode>(0, 0, 50, 50));
    }

    constexpr size_t ITERATIONS = 5000;

    auto start = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < ITERATIONS; ++i) {
        LayoutSolver::ResolveConstraints(container.get());
    }
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> elapsed = end - start;

    std::cout << "Resolved 100-node constraints " << ITERATIONS << " times: " << elapsed.count() << " ms" << std::endl;
    std::cout << "Average resolution time per pass: " << (elapsed.count() / ITERATIONS) * 1000.0 << " microseconds" << std::endl;
}

int main() {
    std::cout << "=================================================" << std::endl;
    std::cout << "          VECTMA PERFORMANCE BENCHMARKS          " << std::endl;
    std::cout << "=================================================" << std::endl;

    runTokenRegistryBenchmark();
    runLayoutSolverBenchmark();

    std::cout << "\nAll benchmarks completed successfully!" << std::endl;
    return 0;
}
