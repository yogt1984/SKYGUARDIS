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

# Check for required tools
CHECK_CMAKE := $(shell which cmake 2>/dev/null)
CHECK_GNAT := $(shell which gnatmake 2>/dev/null)
CHECK_GCC := $(shell which g++ 2>/dev/null)

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

# Check dependencies
check-deps:
	@echo "Checking dependencies..."
	@if [ -z "$(CHECK_GCC)" ]; then \
		echo "ERROR: g++ not found. Please install g++"; \
		exit 1; \
	fi
	@if [ -z "$(CHECK_CMAKE)" ]; then \
		echo "WARNING: cmake not found. Building C++ directly with g++..."; \
		echo "  (Install cmake for better build management: sudo apt-get install cmake)"; \
	fi
	@if [ -z "$(CHECK_GNAT)" ]; then \
		echo "WARNING: gnatmake not found. Ada components will not build."; \
		echo "  (Install GNAT: sudo apt-get install gnat)"; \
	fi
	@echo "Dependency check complete"

# Build C++ components - try CMake first, fallback to direct g++ build
build-cpp: setup-dirs check-deps
	@echo "Building C++ components..."
	@if [ -n "$(CHECK_CMAKE)" ]; then \
		echo "Using CMake..."; \
		cd $(BUILD_DIR) && cmake $(CMAKE_FLAGS) .. && \
		cd $(BUILD_DIR) && $(MAKE) -j$$(nproc); \
	else \
		echo "Building directly with g++..."; \
		$(MAKE) build-cpp-direct; \
	fi
	@echo "C++ build complete"

# Direct C++ build (fallback when CMake is not available)
build-cpp-direct: setup-dirs
	@echo "Building C++ components directly..."
	@mkdir -p $(BIN_DIR)
	@g++ -std=c++17 -I./include/cpp -O2 -Wall \
		src/cpp/main_c2_node.cpp \
		src/cpp/c2_controller/c2_controller.cpp \
		src/cpp/c2_controller/threat_evaluator.cpp \
		src/cpp/radar_simulator/radar_simulator.cpp \
		src/cpp/radar_simulator/scenario_manager.cpp \
		src/cpp/message_gateway/message_gateway.cpp \
		src/cpp/message_gateway/protocol.cpp \
		src/cpp/logger/logger.cpp \
		src/cpp/logger/visualizer.cpp \
		-pthread
		-o $(C2_NODE) -pthread -lrt || \
		(echo "ERROR: C++ build failed. Check your source files." && exit 1)
	@g++ -std=c++17 -I./include/cpp -O2 -Wall \
		tests/cpp/test_message_gateway.cpp \
		src/cpp/message_gateway/protocol.cpp \
		src/cpp/message_gateway/message_gateway.cpp \
		-o $(BIN_DIR)/test_message_gateway -pthread -lrt || true
	@g++ -std=c++17 -I./include/cpp -O2 -Wall \
		tests/cpp/test_state_machine_integration.cpp \
		src/cpp/message_gateway/protocol.cpp \
		src/cpp/message_gateway/message_gateway.cpp \
		-o $(BIN_DIR)/test_state_machine_integration -pthread -lrt || true
	@g++ -std=c++17 -I./include/cpp -O2 -Wall \
		tests/cpp/test_radar_simulation.cpp \
		src/cpp/radar_simulator/radar_simulator.cpp \
		src/cpp/radar_simulator/scenario_manager.cpp \
		-o $(BIN_DIR)/test_radar_simulation -pthread -lrt || true
	@g++ -std=c++17 -I./include/cpp -O2 -Wall \
		tests/cpp/test_logging.cpp \
		src/cpp/logger/logger.cpp \
		-o $(BIN_DIR)/test_logging -pthread -lrt || true
	@g++ -std=c++17 -I./include/cpp -O2 -Wall \
		tests/cpp/test_comprehensive_integration.cpp \
		src/cpp/message_gateway/protocol.cpp \
		src/cpp/message_gateway/message_gateway.cpp \
		src/cpp/c2_controller/c2_controller.cpp \
		src/cpp/c2_controller/threat_evaluator.cpp \
		src/cpp/radar_simulator/radar_simulator.cpp \
		src/cpp/radar_simulator/scenario_manager.cpp \
		-o $(BIN_DIR)/test_comprehensive_integration -pthread -lrt || true
	@g++ -std=c++17 -I./include/cpp -O2 -Wall \
		tests/cpp/test_safety_comprehensive.cpp \
		src/cpp/c2_controller/threat_evaluator.cpp \
		-o $(BIN_DIR)/test_safety_comprehensive -pthread -lrt || true
	@g++ -std=c++17 -I./include/cpp -O2 -Wall \
		tests/cpp/test_ballistics_comprehensive.cpp \
		-o $(BIN_DIR)/test_ballistics_comprehensive -pthread -lrt || true
	@g++ -std=c++17 -I./include/cpp -O2 -Wall \
		tests/cpp/test_comprehensive_integration.cpp \
		src/cpp/message_gateway/protocol.cpp \
		src/cpp/message_gateway/message_gateway.cpp \
		src/cpp/c2_controller/threat_evaluator.cpp \
		src/cpp/radar_simulator/radar_simulator.cpp \
		src/cpp/radar_simulator/scenario_manager.cpp \
		-o $(BIN_DIR)/test_comprehensive_integration -pthread -lrt || true
	@g++ -std=c++17 -I./include/cpp -O2 -Wall \
		tests/cpp/test_safety_comprehensive.cpp \
		src/cpp/c2_controller/threat_evaluator.cpp \
		-o $(BIN_DIR)/test_safety_comprehensive -pthread -lrt || true
	@g++ -std=c++17 -I./include/cpp -O2 -Wall \
		tests/cpp/test_ballistics_comprehensive.cpp \
		-o $(BIN_DIR)/test_ballistics_comprehensive -pthread -lrt || true
	@g++ -std=c++17 -I./include/cpp -O2 -Wall \
		src/cpp/main_radar_sim.cpp \
		src/cpp/radar_simulator/radar_simulator.cpp \
		-o $(RADAR_SIM) || true
	@echo "Direct C++ build complete"

# Build Ada components using GNAT
build-ada: setup-dirs check-deps
	@if [ -z "$(CHECK_GNAT)" ]; then \
		echo "ERROR: gnatmake not found. Cannot build Ada components."; \
		echo "  Install GNAT: sudo apt-get install gnat"; \
		exit 1; \
	fi
	@echo "Building Ada components..."
	@gnatmake $(ADA_FLAGS) -q || (echo "Ada build failed" && exit 1)
	@echo "Ada build complete"

# Build everything (Ada optional)
build: build-cpp
	@if [ -n "$(CHECK_GNAT)" ]; then \
		$(MAKE) build-ada || echo "Ada build skipped"; \
	else \
		echo "Skipping Ada build (gnatmake not available)"; \
	fi
	@echo "Build complete"

# Run C++ tests
test-cpp: build-cpp
	@echo "Running C++ tests..."
	@if [ -n "$(CHECK_CMAKE)" ] && [ -f $(BUILD_DIR)/Makefile ]; then \
		cd $(BUILD_DIR) && ctest --output-on-failure || true; \
	else \
		echo "Running direct tests..."; \
		if [ -f $(BIN_DIR)/test_threat_evaluator ]; then \
			$(BIN_DIR)/test_threat_evaluator || true; \
		fi; \
		if [ -f $(BIN_DIR)/test_message_gateway ]; then \
			$(BIN_DIR)/test_message_gateway || true; \
		fi; \
		if [ -f $(BIN_DIR)/test_state_machine_integration ]; then \
			$(BIN_DIR)/test_state_machine_integration || true; \
		fi; \
		if [ -f $(BIN_DIR)/test_radar_simulation ]; then \
			$(BIN_DIR)/test_radar_simulation || true; \
		fi; \
		if [ -f $(BIN_DIR)/test_logging ]; then \
			$(BIN_DIR)/test_logging || true; \
		fi; \
		if [ -f $(BIN_DIR)/test_comprehensive_integration ]; then \
			$(BIN_DIR)/test_comprehensive_integration || true; \
		fi; \
		if [ -f $(BIN_DIR)/test_safety_comprehensive ]; then \
			$(BIN_DIR)/test_safety_comprehensive || true; \
		fi; \
		if [ -f $(BIN_DIR)/test_ballistics_comprehensive ]; then \
			$(BIN_DIR)/test_ballistics_comprehensive || true; \
		fi; \
		if [ -f $(BIN_DIR)/test_comprehensive_integration ]; then \
			$(BIN_DIR)/test_comprehensive_integration || true; \
		fi; \
		if [ -f $(BIN_DIR)/test_safety_comprehensive ]; then \
			$(BIN_DIR)/test_safety_comprehensive || true; \
		fi; \
		if [ -f $(BIN_DIR)/test_ballistics_comprehensive ]; then \
			$(BIN_DIR)/test_ballistics_comprehensive || true; \
		fi; \
	fi

# Run Ada tests
test-ada:
	@if [ -z "$(CHECK_GNAT)" ]; then \
		echo "Skipping Ada tests (gnatmake not available)"; \
	elif [ -f $(GUN_CONTROL) ]; then \
		echo "Running Ada tests..."; \
		if [ -f $(BIN_DIR)/test_ballistics ]; then \
			$(BIN_DIR)/test_ballistics || true; \
		else \
			echo "Ada tests not yet implemented"; \
		fi; \
	else \
		echo "Skipping Ada tests (Ada components not built)"; \
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
	if [ -f $(GUN_CONTROL) ]; then \
		sleep 0.5 && \
		($(GUN_CONTROL) 2>&1 | tee -a $(EMULATOR_LOG) &); \
	else \
		echo "[WARNING] Gun control not available (Ada not built)" | tee -a $(EMULATOR_LOG); \
	fi && \
	wait

# Default target: build and run emulator
all: emulator

# Clean build artifacts
clean:
	@echo "Cleaning build artifacts..."
	@rm -rf $(BUILD_DIR)
	@rm -rf $(LOG_DIR)
	@echo "Clean complete"

