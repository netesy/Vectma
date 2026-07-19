# Vectma Studio Cross-Platform Makefile

CXX = g++
CC = gcc
# Default flags with GUI support
CXXFLAGS = -std=c++20 -Iinclude -I. -Ivendor -Ivendor/aui/include -Ivendor/glfw/include -Ivendor/glfw/deps -Ivendor/glm -Ivendor/fmt -Wall -Wextra -Wpedantic -Werror -Wno-unknown-pragmas -DGLFW_INCLUDE_NONE -DVECTMA_USE_OPENGL -DAUI_STATIC -DAUI_PLATFORM_WIN -DFMT_HEADER_ONLY
CFLAGS = -std=c11 -Iinclude -I. -Ivendor -Ivendor/aui/include -Ivendor/glfw/include -Ivendor/glfw/deps -Wall -Wextra

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
GLAD_SRCS = vendor/glfw/deps/glad_gl.c
GLFW_SRCS = vendor/glfw/src/context.c vendor/glfw/src/init.c vendor/glfw/src/input.c vendor/glfw/src/monitor.c vendor/glfw/src/vulkan.c vendor/glfw/src/window.c vendor/glfw/src/egl_context.c vendor/glfw/src/osmesa_context.c $(GLFW_PLATFORM_SRCS)
AUI_SRCS = $(wildcard vendor/aui/include/AUI/**/*.cpp)
VENDOR_OBJS = $(GLFW_SRCS:.c=.o) $(GLAD_SRCS:.c=.o) $(AUI_SRCS:.cpp=.o)

SRC_DIRS = src src/core src/core/spatial src/core/snap src/core/modifiers src/renderer src/ui src/sync src/geometry src/layout src/style src/prototype src/network src/export
SRCS = $(foreach dir,$(SRC_DIRS),$(wildcard $(dir)/*.cpp))
OBJS = $(SRCS:.cpp=.o) $(VENDOR_OBJS)

TARGET = vectma$(EXE_EXT)

CORE_OBJS = $(filter-out src/main.o src/ui/ExportHub.o src/ui/SplashWindow.o src/ui/WelcomeWindow.o src/ui/WorkspaceWindow.o, $(SRCS:.cpp=.o))

# Test targets
TEST_TARGET = test_runner
SYNC_TEST_TARGET = test_sync
BOOL_TEST_TARGET = test_boolean
GEOM_TEST_TARGET = test_geometry
COMP_TEST_TARGET = test_components
LAYOUT_TEST_TARGET = test_layout
CANVAS_TEST_TARGET = test_canvas
TOKEN_TEST_TARGET = test_tokens
SELECTION_TEST_TARGET = test_selection
PROTOTYPE_TEST_TARGET = test_prototype
ASSETS_TEST_TARGET = test_assets

.PHONY: all clean run test headless

all: $(TARGET)

headless: CXXFLAGS = -std=c++20 -Iinclude -I. -Ivendor -Ivendor/aui/include -Wall -Wextra -Wpedantic -Werror
headless: $(CORE_OBJS) src/main.o
	$(CXX) $(CXXFLAGS) -o vectma_headless $(CORE_OBJS) src/main.o -lpthread -lm -ldl

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
	rm -f $(OBJS) src/main.o $(TARGET) vectma_headless tests/*.o $(TEST_TARGET) $(SYNC_TEST_TARGET) $(GEOM_TEST_TARGET) $(BOOL_TEST_TARGET) $(COMP_TEST_TARGET) $(LAYOUT_TEST_TARGET) $(CANVAS_TEST_TARGET) $(TOKEN_TEST_TARGET) $(SELECTION_TEST_TARGET) $(PROTOTYPE_TEST_TARGET) $(ASSETS_TEST_TARGET)
	rm -rf build/assets

test: $(TEST_TARGET) $(SYNC_TEST_TARGET) $(GEOM_TEST_TARGET) $(BOOL_TEST_TARGET) $(COMP_TEST_TARGET) $(LAYOUT_TEST_TARGET) $(CANVAS_TEST_TARGET) $(TOKEN_TEST_TARGET) $(SELECTION_TEST_TARGET) $(PROTOTYPE_TEST_TARGET) $(ASSETS_TEST_TARGET)
	./$(TEST_TARGET)
	./$(SYNC_TEST_TARGET)
	./$(GEOM_TEST_TARGET)
	./$(BOOL_TEST_TARGET)
	./$(COMP_TEST_TARGET)
	./$(LAYOUT_TEST_TARGET)
	./$(CANVAS_TEST_TARGET)
	./$(TOKEN_TEST_TARGET)
	./$(SELECTION_TEST_TARGET)
	./$(PROTOTYPE_TEST_TARGET)
	./$(ASSETS_TEST_TARGET)

$(TEST_TARGET): tests/test_core.o $(CORE_OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^ -lpthread -lm -ldl

$(SYNC_TEST_TARGET): tests/test_sync.o $(CORE_OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^ -lpthread -lm -ldl

$(GEOM_TEST_TARGET): tests/test_geometry.o $(CORE_OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^ -lpthread -lm -ldl

$(BOOL_TEST_TARGET): tests/test_boolean.o $(CORE_OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^ -lpthread -lm -ldl

$(COMP_TEST_TARGET): tests/test_components.o $(CORE_OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^ -lpthread -lm -ldl

$(LAYOUT_TEST_TARGET): tests/test_layout.o $(CORE_OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^ -lpthread -lm -ldl

$(CANVAS_TEST_TARGET): tests/test_canvas.o $(CORE_OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^ -lpthread -lm -ldl

$(TOKEN_TEST_TARGET): tests/test_tokens.o $(CORE_OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^ -lpthread -lm -ldl

$(SELECTION_TEST_TARGET): tests/test_selection.o $(CORE_OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^ -lpthread -lm -ldl

$(PROTOTYPE_TEST_TARGET): tests/test_prototype.o $(CORE_OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^ -lpthread -lm -ldl

$(ASSETS_TEST_TARGET): tests/test_assets.o $(CORE_OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^ -lpthread -lm -ldl
