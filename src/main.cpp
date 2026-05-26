#ifdef VECTMA_USE_OPENGL
#include <vendor/glfw/deps/glad/gl.h>
#include <vendor/glfw/include/GLFW/glfw3.h>
#endif

#include "core/WorkspaceStage.hpp"
#include "core/SceneGraph.hpp"
#include "core/RectNode.hpp"
#include "core/EllipseNode.hpp"
#include "core/PathNode.hpp"
#include "core/modifiers/CornerRoundingModifier.hpp"
#include "renderer/RenderPipeline.hpp"
#include "ui/WorkspaceWindow.hpp"
#include "style/TokenRegistry.hpp"

#ifdef VECTMA_USE_SKIA
#include "renderer/SkiaRenderPipeline.hpp"
#include "include/core/SkCanvas.h"
#include "include/core/SkSurface.h"
#endif

#include <iostream>
#include <memory>

#ifdef VECTMA_USE_OPENGL
static void glfw_error_callback(int error, const char* description) {
    fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}
#endif

void seed_scene(std::shared_ptr<vectma::SceneGraph> scene) {
    scene->addChild(std::make_unique<vectma::RectNode>(100, 100, 200, 150));
    scene->addChild(std::make_unique<vectma::EllipseNode>(500, 300, 80, 100));

    auto path = std::make_unique<vectma::PathNode>();
    path->addAnchor({{700, 100}, {700, 100}, {700, 100}});
    path->addAnchor({{900, 100}, {900, 100}, {900, 100}});
    path->addAnchor({{900, 300}, {900, 300}, {900, 300}});
    path->addAnchor({{700, 300}, {700, 300}, {700, 300}});
    path->setClosed(true);
    path->addModifier(std::make_unique<vectma::CornerRoundingModifier>(0.0f));
    scene->addChild(std::move(path));
}

void init_tokens() {
    auto& tr = vectma::TokenRegistry::getInstance();
    tr.registerColorToken("color.primary", vectma::GColor::FromHex("#ddb7ff"), vectma::GColor::FromHex("#b76dff"));
    tr.registerColorToken("color.bg", vectma::GColor::FromHex("#131313"), vectma::GColor::FromHex("#0e0e0e"));
    tr.registerDimensionToken("spacing.gutter", 12.0, 16.0);
    tr.registerRadiusToken("radius.card", 8.0, 12.0);
}

int main() {
    init_tokens();

#ifdef VECTMA_USE_OPENGL
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit()) return 1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    GLFWwindow* window = glfwCreateWindow(1280, 720, "Vectma Studio", NULL, NULL);
    if (window == NULL) return 1;
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    if (!gladLoadGL(glfwGetProcAddress)) return 1;
#endif

    {
        auto workspace = std::make_unique<vectma::WorkspaceStage>();
        auto scene = std::make_shared<vectma::SceneGraph>();
        workspace->setScene(scene);
        seed_scene(scene);

        vectma::TokenRegistry::getInstance().setOnChangeCallback([&](){
            vectma::MarkSceneDirty(scene.get());
        });

        std::unique_ptr<vectma::RenderPipeline> renderer;

#ifdef VECTMA_USE_SKIA
        auto surface = SkSurfaces::Raster(SkImageInfo::MakeN32Premul(1280, 720));
        renderer = std::make_unique<vectma::SkiaRenderPipeline>(surface->getCanvas());
#else
        renderer = std::make_unique<vectma::BaselineRenderer>();
#endif

        auto ui = std::make_unique<vectma::WorkspaceWindow>(*workspace, *renderer);

#ifdef VECTMA_USE_OPENGL
        while (!glfwWindowShouldClose(window)) {
            glfwPollEvents();

            int display_w, display_h;
            glfwGetFramebufferSize(window, &display_w, &display_h);
            glViewport(0, 0, display_w, display_h);
            glClearColor(0.043f, 0.043f, 0.047f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);

            ui->render();
            glfwSwapBuffers(window);
        }
#else
        std::cout << "OpenGL disabled. Running AUI headless simulation." << std::endl;
        ui->render();
#endif
    }

#ifdef VECTMA_USE_OPENGL
    glfwDestroyWindow(window);
    glfwTerminate();
#endif

    return 0;
}
