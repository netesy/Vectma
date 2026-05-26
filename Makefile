# Vectma Studio Cross-Platform Makefile - Headless Only for Tests

CXX = g++
CXXFLAGS = -std=c++20 -Iinclude -I. -Ivendor -Ivendor/aui/include -Wall -Wextra -Wpedantic -Werror

SRC_DIRS = src src/core src/core/spatial src/core/snap src/core/modifiers src/renderer src/ui src/sync src/geometry src/layout src/style src/prototype
SRCS = $(foreach dir,$(SRC_DIRS),$(wildcard $(dir)/*.cpp))
OBJS = $(SRCS:.cpp=.o)

TARGET = vectma

CORE_OBJS = $(filter-out src/main.o, $(SRCS:.cpp=.o))
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

.PHONY: all clean run test auic

all: auic $(TARGET)

auic:
	@echo "Processing AUI assets..."
	@mkdir -p build/assets
	@touch build/assets/compiled_assets.h

$(TARGET): $(OBJS) src/main.o
	$(CXX) $(CXXFLAGS) -o $@ $^

src/%.o: src/%.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

tests/%.o: tests/%.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $<

clean:
	rm -f $(OBJS) src/main.o $(TARGET) tests/*.o $(TEST_TARGET) $(SYNC_TEST_TARGET) $(GEOM_TEST_TARGET) $(BOOL_TEST_TARGET) $(COMP_TEST_TARGET) $(LAYOUT_TEST_TARGET) $(CANVAS_TEST_TARGET) $(TOKEN_TEST_TARGET) $(SELECTION_TEST_TARGET) $(PROTOTYPE_TEST_TARGET)
	rm -rf build/assets

test: $(TEST_TARGET) $(SYNC_TEST_TARGET) $(GEOM_TEST_TARGET) $(BOOL_TEST_TARGET) $(COMP_TEST_TARGET) $(LAYOUT_TEST_TARGET) $(CANVAS_TEST_TARGET) $(TOKEN_TEST_TARGET) $(SELECTION_TEST_TARGET) $(PROTOTYPE_TEST_TARGET)
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

$(TEST_TARGET): tests/test_core.o $(CORE_OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

$(SYNC_TEST_TARGET): tests/test_sync.o $(CORE_OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

$(GEOM_TEST_TARGET): tests/test_geometry.o $(CORE_OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

$(BOOL_TEST_TARGET): tests/test_boolean.o $(CORE_OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

$(COMP_TEST_TARGET): tests/test_components.o $(CORE_OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

$(LAYOUT_TEST_TARGET): tests/test_layout.o $(CORE_OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

$(CANVAS_TEST_TARGET): tests/test_canvas.o $(CORE_OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

$(TOKEN_TEST_TARGET): tests/test_tokens.o $(CORE_OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

$(SELECTION_TEST_TARGET): tests/test_selection.o $(CORE_OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

$(PROTOTYPE_TEST_TARGET): tests/test_prototype.o $(CORE_OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^
