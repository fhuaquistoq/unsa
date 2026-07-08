BUILD_TYPE ?= Debug
BUILD_DIR  ?= build/debug
TARGET     ?= rain_billboards_lab
CXX_STANDARD ?= 17

CMAKE  ?= cmake
CONAN  ?= conan
PYTHON ?= python

ifeq ($(OS),Windows_NT)
	SHELL := cmd.exe
	.SHELLFLAGS := /C
	EXE_EXT := .exe
	BUILD_DIR_NATIVE := $(subst /,\,$(BUILD_DIR))
	EXE_NATIVE := $(subst /,\,$(abspath bin/$(TARGET)$(EXE_EXT)))
	BUILD_ENV := call "$(BUILD_DIR_NATIVE)\conanbuild.bat" &&
	RUN_ENV := call "$(BUILD_DIR_NATIVE)\conanrun.bat" &&
else
	SHELL := /bin/sh
	EXE_EXT :=
	BUILD_DIR_NATIVE := $(BUILD_DIR)
	EXE_NATIVE := bin/$(TARGET)
	BUILD_ENV := . "$(BUILD_DIR)/conanbuild.sh" &&
	RUN_ENV := . "$(BUILD_DIR)/conanrun.sh" &&
endif

EXE := bin/$(TARGET)$(EXE_EXT)
BUILD_STAMP := $(BUILD_DIR)/$(TARGET).stamp

SOURCES := $(wildcard src/*.cpp src/*.c src/*.hpp src/*.h include/*.hpp include/*.h)

.PHONY: all setup pydeps profile deps configure build run clean rebuild release run-release help

all: build

help:
	@echo Available targets:
	@echo   make setup        - install Python deps and detect Conan profile
	@echo   make build        - build Debug
	@echo   make run          - run Debug
	@echo   make release      - build Release
	@echo   make run-release  - run Release
	@echo   make clean        - remove build files
	@echo   make rebuild      - clean and build Debug

.make:
	$(CMAKE) -E make_directory ".make"

$(BUILD_DIR):
	$(CMAKE) -E make_directory "$(BUILD_DIR)"

.make/pydeps.stamp: | .make
	$(PYTHON) -m pip install --upgrade pip jinja2
	$(CMAKE) -E touch ".make/pydeps.stamp"

pydeps: .make/pydeps.stamp

profile:
	$(CONAN) profile detect --force

setup: pydeps profile

$(BUILD_DIR)/conan_toolchain.cmake: conanfile.txt .make/pydeps.stamp | $(BUILD_DIR)
	$(CONAN) install . --output-folder="$(BUILD_DIR)" --build=missing -s build_type=$(BUILD_TYPE) -s compiler.cppstd=$(CXX_STANDARD) -c tools.cmake.cmaketoolchain:generator=Ninja

deps: $(BUILD_DIR)/conan_toolchain.cmake

$(BUILD_DIR)/build.ninja: CMakeLists.txt $(BUILD_DIR)/conan_toolchain.cmake
	$(BUILD_ENV) $(CMAKE) -S . -B "$(BUILD_DIR)" -G Ninja -DCMAKE_TOOLCHAIN_FILE="$(BUILD_DIR)/conan_toolchain.cmake" -DCMAKE_BUILD_TYPE=$(BUILD_TYPE) -DCMAKE_EXPORT_COMPILE_COMMANDS=ON

configure: $(BUILD_DIR)/build.ninja

$(BUILD_STAMP): $(BUILD_DIR)/build.ninja $(SOURCES)
	$(BUILD_ENV) $(CMAKE) --build "$(BUILD_DIR)"
	$(CMAKE) -E touch "$@"

build: $(BUILD_STAMP)

run: build
	$(RUN_ENV) "$(EXE_NATIVE)"

clean:
	$(CMAKE) -E rm -rf build .make bin

rebuild: clean build

release:
	$(MAKE) BUILD_TYPE=Release BUILD_DIR=build/release build

run-release:
	$(MAKE) BUILD_TYPE=Release BUILD_DIR=build/release run
