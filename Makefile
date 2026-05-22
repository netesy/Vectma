CXX = g++
CXXFLAGS = -std=c++20 -Iinclude -I. -Ivendor -Ivendor/imgui -Ivendor/imgui/backends -Ivendor/glfw/include -Wall -Wextra -Wpedantic -Werror

# Centralized vendor objects
VENDOR_SRCS = vendor/imgui/imgui.cpp
VENDOR_OBJS = $(VENDOR_SRCS:.cpp=.o)

SRC_DIRS = src src/core src/core/spatial src/core/snap src/core/modifiers src/renderer src/ui src/sync
SRCS = $(foreach dir,$(SRC_DIRS),$(wildcard $(dir)/*.cpp))
OBJS = $(SRCS:.cpp=.o) $(VENDOR_OBJS)

TARGET = vectma

CORE_OBJS = $(filter-out src/main.o, $(SRCS:.cpp=.o))
TEST_TARGET = test_runner
SYNC_TEST_TARGET = test_sync

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
	rm -f $(OBJS) src/main.o $(TARGET) tests/*.o $(TEST_TARGET) $(SYNC_TEST_TARGET)

test: $(TEST_TARGET) $(SYNC_TEST_TARGET)
	./$(TEST_TARGET)
	./$(SYNC_TEST_TARGET)

$(TEST_TARGET): tests/test_core.o $(CORE_OBJS) $(VENDOR_OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

$(SYNC_TEST_TARGET): tests/test_sync.o $(CORE_OBJS) $(VENDOR_OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^
