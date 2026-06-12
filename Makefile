CXX ?= g++
CXXFLAGS ?= -std=c++17 -Wall -Wextra -O2 -Iinclude

BUILD_DIR := build
BIN_DIR := bin
TARGET := $(BIN_DIR)/ms_cflp_ci

SRCS := main.cpp $(wildcard src/*.cpp)
OBJS := $(patsubst %.cpp,$(BUILD_DIR)/%.o,$(SRCS))

all: $(TARGET)

$(TARGET): $(OBJS) | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(TARGET)

$(BUILD_DIR)/%.o: %.cpp | $(BUILD_DIR)
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR) $(BIN_DIR):
	@mkdir -p $@

clean:
	rm -rf $(BUILD_DIR) $(BIN_DIR)

.PHONY: all clean
