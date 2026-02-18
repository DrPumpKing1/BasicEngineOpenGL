.PHONY: all build clean

all: build

CONFIG ?= Debug

build:
	cmake -S . -B build
	cmake --build build --config $(CONFIG)

clean:
	cmake --build build --target clean