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
#include "ui/SplashWindow.hpp"
#include "ui/WelcomeWindow.hpp"
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
}

int main() {
    auto& tr = vectma::TokenRegistry::getInstance();
    tr.registerColorToken("color.primary", vectma::GColor::FromHex("#ddb7ff"), vectma::GColor::FromHex("#b76dff"));

#ifdef VECTMA_USE_OPENGL
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit()) return 1;
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

        std::unique_ptr<vectma::RenderPipeline> renderer;
#ifdef VECTMA_USE_SKIA
        auto surface = SkSurfaces::Raster(SkImageInfo::MakeN32Premul(1280, 720));
        renderer = std::make_unique<vectma::SkiaRenderPipeline>(surface->getCanvas());
#else
        renderer = std::make_unique<vectma::BaselineRenderer>();
#endif

        auto splash = std::make_unique<vectma::SplashWindow>();
        auto welcome = std::make_unique<vectma::WelcomeWindow>();
        auto editor = std::make_unique<vectma::WorkspaceWindow>(*workspace, *renderer);

        enum class AppState { Splash, Welcome, Editor };
        AppState state = AppState::Splash;

        welcome->onNewProject([&]() { state = AppState::Editor; });

#ifdef VECTMA_USE_OPENGL
        while (!glfwWindowShouldClose(window)) {
            glfwPollEvents();

            glClearColor(0.043f, 0.043f, 0.047f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);

            if (state == AppState::Splash) {
                splash->render();
                splash->tick(0.016f);
                if (splash->isFinished()) state = AppState::Welcome;
            } else if (state == AppState::Welcome) {
                welcome->render();
            } else {
                editor->render();
            }

            glfwSwapBuffers(window);
        }
#else
        std::cout << "Headless run..." << std::endl;
        editor->render();
#endif
    }

#ifdef VECTMA_USE_OPENGL
    glfwDestroyWindow(window);
    glfwTerminate();
#endif
    return 0;
}
