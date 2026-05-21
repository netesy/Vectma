CXX = g++
CXXFLAGS = -std=c++20 -Iinclude -I. -Ivendor/imgui -Ivendor/imgui/backends -Ivendor/glfw/include -Wall -Wextra -Werror

IMGUI_OBJS = vendor/imgui/imgui.o

SRC_DIRS = src src/core src/core/spatial src/core/snap src/core/modifiers src/renderer src/ui
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
	$(CXX) $(CXXFLAGS) -o $@ $^

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

clean:
	rm -f $(OBJS) $(TARGET) $(TEST_OBJS) $(TEST_TARGET)

run: $(TARGET)
	./$(TARGET)

test: $(TEST_TARGET)
	./$(TEST_TARGET)

$(TEST_TARGET): $(TEST_OBJS) $(CORE_OBJS) $(IMGUI_OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^
