Welcome to the Vectma core repository. This document serves as the absolute source of truth and architectural instruction manual for all AI code assistants, engineering agents, and human collaborators.

Vectma is a modern, high-performance, native C++20 desktop vector design application running on an isolated, hardware-accelerated Dear ImGui + GLFW stack.

---

## 1. Build System & Dependency Hermeticity

To maintain flawless cross-platform builds and prevent version drift, all third-party dependencies are strictly vendored inside the repository tree.

*   **Zero System-Wide Includes:** Never look for or link against globally installed system packages (e.g., via apt, brew, or system-wide vcpkg).
*   **Vendored Search Paths:** All dependency inclusions must resolve directly through the project's internal vendor tree:
    *   Dear ImGui core and backends: #include <vendor/imgui/imgui.h> and #include <vendor/imgui/backends/imgui_impl_glfw.h>
    *   Windowing and system context: #include <vendor/glfw/include/GLFW/glfw3.h>
*   **Static Compilation:** Any addition to the UI or window layers must rely exclusively on the local translation units built via our internal project build configuration (CMake/XCode/Make). Do not modify vendor sources directly; use wrapper structures if mutations are necessary.

---

## 2. Architectural Philosophy & Coding Standards

### Memory Management & Resource Ownership
*   **Exclusive Ownership:** The SceneGraph retains sole ownership of structural nodes via `std::unique_ptr<CanvasNode>`. Never break this chain.
*   **Observation & Registries:** The selection engine inside WorkspaceStage tracks nodes using non-owning raw pointers (`CanvasNode*`). This eliminates smart-pointer counter overhead and guarantees fast, volatile mutation tracking.
*   **Zero-Allocation Math:** Primitives such as `Point2D`, `GRect`, and `GTransform` must remain stack-allocated Plain Old Data (POD) structures. No heap allocations are allowed in the inner viewport loop or transformation tracks.

### Rendering Architecture (Double-Dispatch Visitor Pattern)
*   Do not perform string-matching type queries (e.g., `if (node->type == "rect")`) or heavy runtime type inspections (`dynamic_cast`).
*   All element drawing operations must utilize clean double-dispatch routines. Leaf elements implement `void render(RenderPipeline& pipeline)` to execute hardware-isolated drawing tasks with zero overhead.

### UI Design System Spirit
*   **Elevation over Borders:** Do not use harsh, bright bounding outlines to separate interface panels. Separate regions entirely via depth shading using low-contrast container pairings (`#0B0B0C` as pure viewport base background, `#121214` for floating panels, sidebar docks, and card structures).
*   **Context Reactivity:** Keep UI controls immediate-mode driven. Inspect properties in real-time based on active selection flags to prevent out-of-sync configuration fields.

---

## 3. Directory & Component Architecture

```text
vectma/
├── include/
│   ├── core/       # Pure headless geometry, matrix, node, and state engines
│   └── ui/         # Theme configurations, toolbars, and adaptive inspectors
├── src/
│   ├── core/       # Implementation files for core data engines
│   └── ui/         # Immediate-mode interface panel assemblies
├── vendor/         # Hermetic third-party dependencies
│   ├── imgui/      # Labeled Dear ImGui core & platform backends
│   └── glfw/       # Native windowing and input pipelines
├── docs/           # Specifications and architectural manuals (DESIGN.md)
└── tests/          # Headless test runner matrices

```

---

## 4. Verification & Testing Guardrails

Before submitting changes, all automated workflows and human collaborators must verify:

* **Headless Soundness:** Running `make test` must return a 100% green pass. All core selection math and node grouping mechanics must verify accurately without opening a window context.
* **Warning-Free Builds:** Code must compile clean with zero compiler warnings under standard build constraints.
* **Progressive Logs:** Maintain a progressive `MIGRATION_PROGRESS.md` and `TODO.md` log at the root, checking off each legacy module group or phase as it reaches functional status.
* **Test Requirement:** For every module refactored or feature added, you must write matching validation checks inside the `tests/` directory using simple assertions.
* **Coordinate Isolation Rule:** Ensure screen interaction events always route through our inverse viewport transformations:

$$P_{canvas} = M^{-1} \cdot P_{screen}$$

---

## 5. Active Execution Phase: Phase 13 — Command Pattern & State History Engine

Your immediate goal is to implement a high-performance, memory-bounded undo/redo state history engine based on a strict structural Command Pattern.

