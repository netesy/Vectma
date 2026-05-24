# Vectma Studio Cross-Platform Makefile

CXX = g++
CC = gcc
CXXFLAGS = -std=c++20 -Iinclude -I. -Ivendor -Ivendor/imgui -Ivendor/imgui/backends -Ivendor/glfw/include -Ivendor/glfw/deps -Wall -Wextra -Wpedantic -Werror -DIMGUI_API= -DIMGUI_IMPL_OPENGL_LOADER_GLAD -DGLFW_INCLUDE_NONE -DVECTMA_USE_OPENGL
CFLAGS = -std=c11 -Iinclude -I. -Ivendor -Ivendor/imgui -Ivendor/imgui/backends -Ivendor/glfw/include -Ivendor/glfw/deps -Wall -Wextra -DIMGUI_IMPL_OPENGL_LOADER_GLAD

# OS Detection
ifeq ($(OS),Windows_NT)
    PLATFORM = Windows
    EXE_EXT = .exe
    GLFW_PLATFORM_DEFS = -D_GLFW_WIN32 -D_GLFW_WGL -DUNICODE
    GLFW_PLATFORM_SRCS = vendor/glfw/src/win32_init.c vendor/glfw/src/win32_joystick.c vendor/glfw/src/win32_monitor.c vendor/glfw/src/win32_thread.c vendor/glfw/src/win32_time.c vendor/glfw/src/win32_window.c vendor/glfw/src/wgl_context.c
    PLATFORM_LIBS = -lgdi32 -lopengl32
else
    PLATFORM = Linux
    EXE_EXT =
    GLFW_PLATFORM_DEFS = -D_GLFW_X11 -D_GLFW_GLX
    GLFW_PLATFORM_SRCS = vendor/glfw/src/x11_init.c vendor/glfw/src/x11_monitor.c vendor/glfw/src/x11_window.c vendor/glfw/src/xkb_unicode.c vendor/glfw/src/posix_module.c vendor/glfw/src/posix_thread.c vendor/glfw/src/posix_time.c vendor/glfw/src/glx_context.c vendor/glfw/src/linux_joystick.c
    PLATFORM_LIBS = -lX11 -lXrandr -lXi -lXxf86vm -lXinerama -lXcursor -ldl -lpthread -lm -lGL
endif

# Centralized vendor objects
IMGUI_SRCS = vendor/imgui/imgui.cpp vendor/imgui/imgui_draw.cpp vendor/imgui/imgui_tables.cpp vendor/imgui/imgui_widgets.cpp vendor/imgui/backends/imgui_impl_glfw.cpp vendor/imgui/backends/imgui_impl_opengl3.cpp
GLAD_SRCS = vendor/glfw/deps/glad_gl.c
GLFW_SRCS = vendor/glfw/src/context.c vendor/glfw/src/init.c vendor/glfw/src/input.c vendor/glfw/src/monitor.c vendor/glfw/src/vulkan.c vendor/glfw/src/window.c vendor/glfw/src/egl_context.c vendor/glfw/src/osmesa_context.c $(GLFW_PLATFORM_SRCS)
VENDOR_OBJS = $(IMGUI_SRCS:.cpp=.o) $(GLFW_SRCS:.c=.o) $(GLAD_SRCS:.c=.o)

SRC_DIRS = src src/core src/core/spatial src/core/snap src/core/modifiers src/renderer src/ui src/sync src/geometry src/layout src/style
SRCS = $(foreach dir,$(SRC_DIRS),$(wildcard $(dir)/*.cpp))
OBJS = $(SRCS:.cpp=.o) $(VENDOR_OBJS)

TARGET = vectma$(EXE_EXT)

CORE_OBJS = $(filter-out src/main.o, $(SRCS:.cpp=.o))
TEST_TARGET = test_runner$(EXE_EXT)
SYNC_TEST_TARGET = test_sync$(EXE_EXT)
BOOL_TEST_TARGET = test_boolean$(EXE_EXT)
GEOM_TEST_TARGET = test_geometry$(EXE_EXT)
COMP_TEST_TARGET = test_components$(EXE_EXT)
LAYOUT_TEST_TARGET = test_layout$(EXE_EXT)
CANVAS_TEST_TARGET = test_canvas$(EXE_EXT)
TOKEN_TEST_TARGET = test_tokens$(EXE_EXT)
SELECTION_TEST_TARGET = test_selection$(EXE_EXT)

.PHONY: all clean run test

all: $(TARGET)

$(TARGET): $(OBJS) src/main.o
	$(CXX) $(CXXFLAGS) -o $@ $^ $(PLATFORM_LIBS)

src/%.o: src/%.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

vendor/%.o: vendor/%.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

vendor/%.o: vendor/%.c
	$(CC) $(CFLAGS) $(GLFW_PLATFORM_DEFS) -c -o $@ $<

tests/%.o: tests/%.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

clean:
	rm -f $(OBJS) src/main.o $(TARGET) tests/*.o $(TEST_TARGET) $(SYNC_TEST_TARGET) $(GEOM_TEST_TARGET) $(BOOL_TEST_TARGET) $(COMP_TEST_TARGET) $(LAYOUT_TEST_TARGET) $(CANVAS_TEST_TARGET) $(TOKEN_TEST_TARGET) $(SELECTION_TEST_TARGET)

test: $(TEST_TARGET) $(SYNC_TEST_TARGET) $(GEOM_TEST_TARGET) $(BOOL_TEST_TARGET) $(COMP_TEST_TARGET) $(LAYOUT_TEST_TARGET) $(CANVAS_TEST_TARGET) $(TOKEN_TEST_TARGET) $(SELECTION_TEST_TARGET)
	./$(TEST_TARGET)
	./$(SYNC_TEST_TARGET)
	./$(GEOM_TEST_TARGET)
	./$(BOOL_TEST_TARGET)
	./$(COMP_TEST_TARGET)
	./$(LAYOUT_TEST_TARGET)
	./$(CANVAS_TEST_TARGET)
	./$(TOKEN_TEST_TARGET)
	./$(SELECTION_TEST_TARGET)

$(TEST_TARGET): tests/test_core.o $(CORE_OBJS) $(VENDOR_OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

$(SYNC_TEST_TARGET): tests/test_sync.o $(CORE_OBJS) $(VENDOR_OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

$(GEOM_TEST_TARGET): tests/test_geometry.o $(CORE_OBJS) $(VENDOR_OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

$(BOOL_TEST_TARGET): tests/test_boolean.o $(CORE_OBJS) $(VENDOR_OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

$(COMP_TEST_TARGET): tests/test_components.o $(CORE_OBJS) $(VENDOR_OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

$(LAYOUT_TEST_TARGET): tests/test_layout.o $(CORE_OBJS) $(VENDOR_OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

$(CANVAS_TEST_TARGET): tests/test_canvas.o $(CORE_OBJS) $(VENDOR_OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

$(TOKEN_TEST_TARGET): tests/test_tokens.o $(CORE_OBJS) $(VENDOR_OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

$(SELECTION_TEST_TARGET): tests/test_selection.o $(CORE_OBJS) $(VENDOR_OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^
