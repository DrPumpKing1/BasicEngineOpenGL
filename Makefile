.PHONY: all build clean

all: build

BUILD_DIR := build

CONFIG ?= Debug

build:
	cmake -S . -B $(BUILD_DIR)
	cmake --build $(BUILD_DIR) --config $(CONFIG)

clean:
	cmake --build $(BUILD_DIR) --target clean