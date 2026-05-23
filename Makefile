# Vectma Studio Cross-Platform Makefile

CXX = g++
CXXFLAGS = -std=c++20 -Iinclude -I. -Ivendor -Ivendor/imgui -Ivendor/imgui/backends -Ivendor/glfw/include -Wall -Wextra -Wpedantic -Werror

# OS Detection
ifeq ($(OS),Windows_NT)
    PLATFORM = Windows
    EXE_EXT = .exe
else
    PLATFORM = Linux
    EXE_EXT =
endif

# Centralized vendor objects
VENDOR_SRCS = vendor/imgui/imgui.cpp
VENDOR_OBJS = $(VENDOR_SRCS:.cpp=.o)

SRC_DIRS = src src/core src/core/spatial src/core/snap src/core/modifiers src/renderer src/ui src/sync src/geometry src/layout
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

.PHONY: all clean run test

all: $(TARGET)

$(TARGET): $(OBJS) src/main.o
	$(CXX) $(CXXFLAGS) -o $@ $^

src/%.o: src/%.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

vendor/%.o: vendor/%.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

tests/%.o: tests/%.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

clean:
	rm -f $(OBJS) src/main.o $(TARGET) tests/*.o $(TEST_TARGET) $(SYNC_TEST_TARGET) $(GEOM_TEST_TARGET) $(BOOL_TEST_TARGET) $(COMP_TEST_TARGET) $(LAYOUT_TEST_TARGET)

test: $(TEST_TARGET) $(SYNC_TEST_TARGET) $(GEOM_TEST_TARGET) $(BOOL_TEST_TARGET) $(COMP_TEST_TARGET) $(LAYOUT_TEST_TARGET)
	./$(TEST_TARGET)
	./$(SYNC_TEST_TARGET)
	./$(GEOM_TEST_TARGET)
	./$(BOOL_TEST_TARGET)
	./$(COMP_TEST_TARGET)
	./$(LAYOUT_TEST_TARGET)

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
