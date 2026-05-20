# Engineering Instructions for Vectma Core Migration

You are tasked with refactoring an Electron/NodeJS vector graphics editor ('gravit') into a native, standalone C++20 application named **Vectma**.

## 1. Architectural Mapping Rules
Translate legacy JavaScript prototypes and modules to modern C++ structures using the following strict semantic map:
- `GObject` -> `CanvasNode` (Abstract polymorphic base class)
- `GScene` -> `SceneGraph` (Composite structural layer tree)
- `GEditor` -> `WorkspaceStage` (State coordinator for active tools and selections)
- `GPaintCanvas` -> `RenderPipeline` (Graphics hardware abstraction interface)

## 2. Code Quality & Language Standards
- **Standard:** Modern C++20 (`-std=c++20`).
- **Memory Management:** Enforce strict RAII boundaries. Raw pointers are forbidden for ownership; use `std::unique_ptr` for exclusive scene hierarchy nodes and `std::shared_ptr` only where multiple modules co-own resources.
- **Concurrency:** Replace all background Web Workers (`*.worker.js`) with native `std::jthread` workers pulling from thread-safe queues.
- **Dependencies:** Assume a desktop runtime execution environment utilizing GLFW for windowing and a generalized abstraction loop for drawing vector paths. Do not bundle heavy web frameworks or external server layers.

## 3. Repository Directory Structure
Organize all generated native files within these explicit zones:
- `/include/core/`, `/include/ui/`, `/include/renderer/` -> Public or module header declarations (`.hpp`)
- `/src/core/`, `/src/ui/`, `/src/renderer/` -> Implementation source files (`.cpp`)
- `/` (Root) -> Main entrypoint (`src/main.cpp`) and the orchestration build layer (`Makefile`)

## 4. Change Tracking & Verification
- For every module refactored, you must write a matching lightweight, header-only validation check inside a `/tests` directory using simple assertions.
- Maintain a progressive `MIGRATION_PROGRESS.md` log at the root, checking off each legacy module group as its C++ counterpart reaches functional compilation status.
