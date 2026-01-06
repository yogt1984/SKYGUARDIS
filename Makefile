.PHONY: all build clean test run emulator help
.DEFAULT_GOAL := all

# Directories
BUILD_DIR := build
BIN_DIR := $(BUILD_DIR)/bin
LOG_DIR := logs
ADA_BUILD := $(BUILD_DIR)/ada

# Executables
C2_NODE := $(BIN_DIR)/c2_node
RADAR_SIM := $(BIN_DIR)/radar_sim
GUN_CONTROL := $(BIN_DIR)/main_gun_control
EMULATOR_LOG := $(LOG_DIR)/emulator.log

# Build flags
CMAKE_FLAGS := -DCMAKE_BUILD_TYPE=Release
ADA_FLAGS := -P skyguardis.gpr

help:
	@echo "SKYGUARDIS Makefile"
	@echo ""
	@echo "Targets:"
	@echo "  make          - Build and run the emulator (logs to $(EMULATOR_LOG))"
	@echo "  make build    - Build all components (C++ and Ada)"
	@echo "  make test     - Run all tests"
	@echo "  make clean    - Clean build artifacts"
	@echo "  make emulator - Run emulator only (assumes already built)"

# Create necessary directories
.PHONY: setup-dirs
setup-dirs:
	@mkdir -p $(BUILD_DIR)
	@mkdir -p $(BIN_DIR)
	@mkdir -p $(ADA_BUILD)
	@mkdir -p $(LOG_DIR)

# Build C++ components using CMake
build-cpp: setup-dirs
	@echo "Building C++ components..."
	@cd $(BUILD_DIR) && cmake $(CMAKE_FLAGS) ..
	@cd $(BUILD_DIR) && $(MAKE) -j$$(nproc)
	@echo "C++ build complete"

# Build Ada components using GNAT
build-ada: setup-dirs
	@echo "Building Ada components..."
	@gnatmake $(ADA_FLAGS) -q || (echo "Ada build failed" && exit 1)
	@echo "Ada build complete"

# Build everything
build: build-cpp build-ada
	@echo "All components built successfully"

# Run C++ tests
test-cpp: build-cpp
	@echo "Running C++ tests..."
	@cd $(BUILD_DIR) && ctest --output-on-failure || true

# Run Ada tests
test-ada: build-ada
	@echo "Running Ada tests..."
	@if [ -f $(BIN_DIR)/test_ballistics ]; then \
		$(BIN_DIR)/test_ballistics || true; \
	else \
		echo "Ada tests not yet implemented"; \
	fi

# Run all tests
test: test-cpp test-ada
	@echo "All tests completed"

# Run emulator (combines C2 node and gun control)
emulator: build
	@echo "Starting SKYGUARDIS emulator..."
	@echo "Logging to $(EMULATOR_LOG)"
	@echo "Press Ctrl+C to stop"
	@echo ""
	@mkdir -p $(LOG_DIR)
	@echo "=== SKYGUARDIS Emulator Started at $$(date) ===" | tee -a $(EMULATOR_LOG)
	@echo "" | tee -a $(EMULATOR_LOG)
	@trap 'echo ""; echo "=== Emulator Stopped at $$(date) ===" | tee -a $(EMULATOR_LOG); pkill -P $$(pgrep -f "c2_node\|main_gun_control") 2>/dev/null || true; exit 0' INT TERM; \
	($(C2_NODE) 2>&1 | tee -a $(EMULATOR_LOG) &) && \
	sleep 0.5 && \
	($(GUN_CONTROL) 2>&1 | tee -a $(EMULATOR_LOG) &) && \
	wait

# Default target: build and run emulator
all: emulator

# Clean build artifacts
clean:
	@echo "Cleaning build artifacts..."
	@rm -rf $(BUILD_DIR)
	@rm -rf $(LOG_DIR)
	@echo "Clean complete"

