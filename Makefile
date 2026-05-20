CXX = g++
CXXFLAGS = -std=c++20 -Iinclude -Ivendor/imgui -Ivendor/imgui/backends -Wall -Wextra
LDFLAGS = -lGL -lGLEW -lglfw

IMGUI_OBJS = vendor/imgui/imgui.o              vendor/imgui/imgui_draw.o              vendor/imgui/imgui_widgets.o              vendor/imgui/imgui_tables.o              vendor/imgui/backends/imgui_impl_glfw.o              vendor/imgui/backends/imgui_impl_opengl3.o

SRC_DIRS = src src/core src/renderer src/ui
SRCS = $(foreach dir,$(SRC_DIRS),$(wildcard $(dir)/*.cpp))
OBJS = $(SRCS:.cpp=.o) $(IMGUI_OBJS)

TARGET = vectma

TEST_SRCS = tests/test_core.cpp
TEST_OBJS = $(TEST_SRCS:.cpp=.o)
CORE_OBJS = $(filter-out src/main.o, $(SRCS:.cpp=.o))
TEST_TARGET = test_runner

.PHONY: all clean run test

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

clean:
	rm -f $(OBJS) $(TARGET) $(TEST_OBJS) $(TEST_TARGET)

run: $(TARGET)
	./$(TARGET)

test: $(TEST_TARGET)
	./$(TEST_TARGET)

$(TEST_TARGET): $(TEST_OBJS) $(CORE_OBJS) $(IMGUI_OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)
