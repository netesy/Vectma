#ifdef VECTMA_USE_OPENGL
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#endif

#include "core/WorkspaceStage.hpp"
#include "core/SceneGraph.hpp"
#include "core/RectNode.hpp"
#include "core/EllipseNode.hpp"
#include "renderer/RenderPipeline.hpp"
#include "ui/EditorUI.hpp"

#ifdef VECTMA_USE_SKIA
#include "renderer/SkiaRenderPipeline.hpp"
#include "include/core/SkCanvas.h"
#include "include/core/SkSurface.h"
#endif

#include <imgui.h>
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
#ifdef VECTMA_USE_OPENGL
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit()) return 1;

    const char* glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    GLFWwindow* window = glfwCreateWindow(1280, 720, "Vectma Core", NULL, NULL);
    if (window == NULL) return 1;
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    if (glewInit() != GLEW_OK) return 1;

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);
#else
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
#endif

    {
        auto workspace = std::make_unique<vectma::WorkspaceStage>();
        auto scene = std::make_shared<vectma::SceneGraph>();
        workspace->setScene(scene);
        seed_scene(scene);

        std::unique_ptr<vectma::RenderPipeline> renderer;

#ifdef VECTMA_USE_SKIA
        // In a real app, this surface would be backed by the OpenGL context
        auto surface = SkSurfaces::Raster(SkImageInfo::MakeN32Premul(1280, 720));
        renderer = std::make_unique<vectma::SkiaRenderPipeline>(surface->getCanvas());
#else
        renderer = std::make_unique<vectma::BaselineRenderer>();
#endif

        auto ui = std::make_unique<vectma::EditorUI>(*workspace, *renderer);

#ifdef VECTMA_USE_OPENGL
        while (!glfwWindowShouldClose(window)) {
            glfwPollEvents();
            ui->render();

            int display_w, display_h;
            glfwGetFramebufferSize(window, &display_w, &display_h);
            glViewport(0, 0, display_w, display_h);
            glClearColor(0.043f, 0.043f, 0.047f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);

            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
            glfwSwapBuffers(window);
        }

        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
#else
        std::cout << "OpenGL disabled. Running headless core logic." << std::endl;
        ui->render();
#endif
    }

    ImGui::DestroyContext();

#ifdef VECTMA_USE_OPENGL
    glfwDestroyWindow(window);
    glfwTerminate();
#endif

    return 0;
}
