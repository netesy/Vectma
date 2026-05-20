#pragma once
#define GLFW_CONTEXT_VERSION_MAJOR 3
#define GLFW_CONTEXT_VERSION_MINOR 0
typedef struct GLFWwindow GLFWwindow;
inline int glfwInit() { return 1; }
inline void glfwTerminate() {}
inline void glfwSetErrorCallback(void*) {}
inline GLFWwindow* glfwCreateWindow(int, int, const char*, void*, void*) { return (GLFWwindow*)1; }
inline void glfwMakeContextCurrent(GLFWwindow*) {}
inline void glfwSwapInterval(int) {}
inline int glfwWindowShouldClose(GLFWwindow*) { return 0; }
inline void glfwPollEvents() {}
inline void glfwGetFramebufferSize(GLFWwindow*, int* w, int* h) { *w=1280; *h=720; }
inline void glfwSwapBuffers(GLFWwindow*) {}
inline void glfwDestroyWindow(GLFWwindow*) {}
