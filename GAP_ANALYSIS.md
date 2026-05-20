# Vectma Engine Migration: Cross-Language Gap Analysis

This document provides a technical comparison between the original JavaScript codebase ('Gravit') and the newly implemented C++20 engine ('Vectma').

## 1. Structural Comparison

| Feature | Legacy JavaScript (Gravit) | Native C++20 (Vectma) | Migration Strategy |
|:---|:---|:---|:---|
| **Base Class** | `GObject` (Dynamic Prototypes) | `CanvasNode` (Static Polymorphism) | Strict C++20 virtual dispatch; eliminated dynamic prototype overhead. |
| **Scene Tree** | `GScene` / `GNode` | `SceneGraph` (Composite Tree) | Recursive depth-first traversal using `std::unique_ptr`. |
| **Memory** | Garbage Collected (NodeJS/V8) | RAII (Smart Pointers) | Manual ownership management via `unique_ptr` and `move` semantics. |
| **Math** | `GMath`, `GTransform`, `GRect` | `GTransform`, `GRect`, `Point2D` | Implemented as stack-allocated, high-performance POD structures. |
| **Rendering** | `GPaintCanvas` (Web Canvas/WebGL) | `RenderPipeline` (Abstract Interface) | Decoupled rendering logic using the Visitor pattern (Double Dispatch). |
| **Interaction** | `GEditor`, `GTool` | `WorkspaceStage` (Interaction State) | Data-driven tool state machine independent of GUI frameworks. |

## 2. Module Mapping & Implementation Status

| Legacy Path (JS) | Native Implementation (C++) | Status | Implementation Details |
|:---|:---|:---:|:---|
| `infinity/core/GObject.js` | `include/core/CanvasNode.hpp` | 🟢 | Abstract base with pure virtual dispatch hooks. |
| `infinity/core/GNode.js` | `src/core/CanvasNode.cpp` | 🟢 | Integrated parent-child logic with `std::unique_ptr`. |
| `infinity/core/GScene.js` | `include/core/SceneGraph.hpp` | 🟢 | Composite graph with z-order and grouping. |
| `editor/GEditor.js` | `src/core/WorkspaceStage.cpp` | 🟢 | Tool modalities (Rect, Marquee, Select) implemented. |
| `infinity/math/GTransform.js` | `include/core/GTransform.hpp` | 🟢 | Full 3x3 affine matrix support with inversion. |
| `infinity/math/GRect.js` | `include/core/GRect.hpp` | 🟢 | Intersection and union logic for bounding boxes. |
| `infinity/paint/GColor.js` | `include/core/GColor.hpp` | 🟢 | Native RGBA8 structure. |
| `infinity/scene/GGroup.js` | `src/core/SceneGraph.cpp` | 🟢 | Atomic `groupNodes` and `ungroupNode` methods. |

## 3. Interaction & Coordinate Gaps

*   **Mapping:** Vectma centralizes coordinate translation in `WorkspaceStage::screenToCanvas` using {canvas} = M^{-1} \cdot P_{screen}$, where $ is the viewport matrix.
*   **Encapsulation:** Shape leaf nodes (`RectNode`, `EllipseNode`) encapsulate their own data and hit-testing logic, returning references to the `RenderPipeline` via double-dispatch.
*   **Stack vs Heap:** Mathematical primitives (`GPoint`, `GRect`, `GTransform`) have been transitioned from JS objects (heap-allocated in V8) to C++ stack-allocated structures for zero-allocation math.

## 4. Performance & Validation

*   **Runtime:** Eliminated Electron, V8, and NodeJS overhead.
*   **Compilation:** Fully C++20 compliant with parallel Makefile configuration.
*   **Tests:** 100% green pass on interaction, rendering, and structural integrity tests in `/tests/test_core.cpp`.
