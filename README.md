# Vectma Core

<p align="center">
  <img src="logo.png" alt="Vectma Logo" width="200" />
</p>

<p align="center">
  A high-performance, native C++20 desktop vector design application.
</p>

---

## Overview

Vectma is a modern vector graphics engine built from the ground up for professional design workflows. It features a completely native C++20 architecture, hardware-accelerated rendering, and a non-destructive geometry pipeline.

## Key Features

- **Native Performance**: Written in C++20 with a focus on low-level optimization and zero-allocation math.
- **Hardware Acceleration**: Isolated graphics abstraction layer supporting modern rendering backends.
- **Non-Destructive Pipeline**: Parametric path modifiers (Corner Rounding, Dashing, Offsets) with lazy evaluation and render-time caching.
- **Hierarchical Layer Management**: Deeply nested document structures with granular visibility and locking controls.
- **Infinite Canvas Artboards**: Multi-artboard support with hardware-accelerated clipping bounds.
- **Precision Snapping Engine**: Geometry-aware constraint solver integrated with a 2D spatial Quadtree for O(log N) hit-testing and snapping.
- **Headless Soundness**: Robust test suite verifying core logic without requiring a windowing context.

## Technology Stack

- **Language**: C++20
- **UI Framework**: Dear ImGui (Immediate Mode)
- **Windowing & Input**: GLFW
- **Graphics Backend**: OpenGL / Graphics Hardware Abstraction
- **Spatial Indexing**: 2D Quadtree / BVH

## Getting Started

### Prerequisites

- C++20 compatible compiler (g++ 10+ or clang 12+)
- GNU Make

### Building from Source

```bash
# Clone the repository
git clone <repo-url> vectma
cd vectma

# Build the application
make -j4

# Run unit tests
make test
```

### Running the Application

```bash
./vectma
```

## Project Structure

- `include/core/`: Headless geometry, matrix, node, and state engines.
- `include/ui/`: Theme configurations, toolbars, and adaptive inspectors.
- `src/core/`: Implementation of core data and math engines.
- `src/ui/`: Immediate-mode interface panel assemblies.
- `vendor/`: Hermetic third-party dependencies (Dear ImGui, GLFW, stb).
- `tests/`: Headless test runner matrices.

## Roadmap & Progress

The project is currently undergoing a phased native migration. Current status:
- **Phases 1-18**: COMPLETE (Core Math, Styling, CSG, Path Effects, Symbols, Spatial Indexing, Snapping, Layers & Artboards)
- **Phase 19**: ACTIVE (UI/UX Refinement & Stitch Integration)
- **Phase 20**: UPCOMING (CRDT Collaborative Real-Time Sync)

## License

Internal Development Version. All rights reserved.
