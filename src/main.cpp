#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "core/WorkspaceStage.hpp"
#include "core/SceneGraph.hpp"
#include "core/RectNode.hpp"
#include "core/EllipseNode.hpp"
#include "renderer/RenderPipeline.hpp"
#include "ui/EditorUI.hpp"
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <iostream>
#include <memory>

static void glfw_error_callback(int error, const char* description) {
    fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

// Seed the scene with some nodes for the screenshot
void seed_scene(std::shared_ptr<vectma::SceneGraph> scene) {
    scene->addChild(std::make_unique<vectma::RectNode>(100, 100, 200, 150));
    scene->addChild(std::make_unique<vectma::EllipseNode>(500, 300, 80, 100));
}

int main() {
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit()) return 1;

    const char* glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    GLFWwindow* window = glfwCreateWindow(1280, 720, "Vectma Core", NULL, NULL);
    if (window == NULL) return 1;
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync

    if (glewInit() != GLEW_OK) return 1;

    // Setup ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);

    // Initialize core components
    auto workspace = std::make_unique<vectma::WorkspaceStage>();
    auto scene = std::make_shared<vectma::SceneGraph>();
    workspace->setScene(scene);

    seed_scene(scene);

    auto renderer = std::make_unique<vectma::BaselineRenderer>();
    auto ui = std::make_unique<vectma::EditorUI>(*workspace, *renderer);

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

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
