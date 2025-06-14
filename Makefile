# MIPS Simulator Makefile
# Provides convenient build targets for the MIPS simulator project

# Variables
BUILD_DIR = build
CMAKE_BUILD_TYPE ?= Release
PYTHON_EXECUTABLE ?= python3
JOBS ?= $(shell nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4)

# Default target
.PHONY: all
all: cpp python

# Help target
.PHONY: help
help:
	@echo "MIPS Simulator Build System"
	@echo "============================"
	@echo ""
	@echo "Available targets:"
	@echo "  all          - Build C++ library and Python bindings"
	@echo "  cpp          - Build C++ components only"
	@echo "  python       - Build Python bindings"
	@echo "  test         - Run test suite"
	@echo "  docs         - Generate documentation"
	@echo "  install      - Install the simulator"
	@echo "  clean        - Clean build artifacts"
	@echo "  format       - Format source code"
	@echo "  lint         - Run linting tools"
	@echo "  package      - Create distribution packages"
	@echo "  dev          - Setup development environment"
	@echo "  run-flask    - Start Flask development server"
	@echo ""
	@echo "Variables:"
	@echo "  BUILD_DIR         = $(BUILD_DIR)"
	@echo "  CMAKE_BUILD_TYPE  = $(CMAKE_BUILD_TYPE)"
	@echo "  PYTHON_EXECUTABLE = $(PYTHON_EXECUTABLE)"
	@echo "  JOBS              = $(JOBS)"
	@echo ""
	@echo "Examples:"
	@echo "  make cpp CMAKE_BUILD_TYPE=Debug"
	@echo "  make test JOBS=8"
	@echo "  make python PYTHON_EXECUTABLE=python3.9"

# C++ build target
.PHONY: cpp
cpp: $(BUILD_DIR)/Makefile
	@echo "Building C++ components..."
	@cd $(BUILD_DIR) && $(MAKE) -j$(JOBS)

# CMake configuration
$(BUILD_DIR)/Makefile:
	@echo "Configuring CMake build..."
	@mkdir -p $(BUILD_DIR)
	@cd $(BUILD_DIR) && cmake \
		-DCMAKE_BUILD_TYPE=$(CMAKE_BUILD_TYPE) \
		-DBUILD_TESTS=ON \
		-DBUILD_PYTHON_BINDINGS=ON \
		-DPYTHON_EXECUTABLE=$(PYTHON_EXECUTABLE) \
		..

# Python bindings
.PHONY: python
python:
	@echo "Building Python bindings..."
	@$(PYTHON_EXECUTABLE) -m pip install -e .

# Development setup
.PHONY: dev
dev:
	@echo "Setting up development environment..."
	@$(PYTHON_EXECUTABLE) -m pip install -r requirements.txt
	@$(PYTHON_EXECUTABLE) -m pip install -e .
	@echo "Development environment ready!"

# Test targets
.PHONY: test
test: test-cpp test-python

.PHONY: test-cpp
test-cpp: cpp
	@echo "Running C++ tests..."
	@cd $(BUILD_DIR) && ctest --output-on-failure -j$(JOBS)

.PHONY: test-python
test-python: python
	@echo "Running Python tests..."
	@$(PYTHON_EXECUTABLE) -m pytest tests/ -v

.PHONY: test-coverage
test-coverage: python
	@echo "Running tests with coverage..."
	@$(PYTHON_EXECUTABLE) -m pytest tests/ --cov=mips_simulator --cov-report=html --cov-report=term

# Documentation
.PHONY: docs
docs: cpp
	@echo "Generating documentation..."
	@cd $(BUILD_DIR) && $(MAKE) docs
	@echo "Documentation generated in $(BUILD_DIR)/docs/html/"

# Code formatting
.PHONY: format
format:
	@echo "Formatting C++ code..."
	@find src/ -name "*.cpp" -o -name "*.h" | xargs clang-format -i
	@echo "Formatting Python code..."
	@$(PYTHON_EXECUTABLE) -m black flask_app/ python_bindings/ tests/

# Linting
.PHONY: lint
lint:
	@echo "Linting C++ code..."
	@find src/ -name "*.cpp" -o -name "*.h" | xargs cppcheck --enable=all --suppress=missingIncludeSystem
	@echo "Linting Python code..."
	@$(PYTHON_EXECUTABLE) -m flake8 flask_app/ python_bindings/ tests/
	@$(PYTHON_EXECUTABLE) -m mypy flask_app/ python_bindings/

# Installation
.PHONY: install
install: cpp
	@echo "Installing MIPS Simulator..."
	@cd $(BUILD_DIR) && sudo $(MAKE) install
	@$(PYTHON_EXECUTABLE) -m pip install .

# Packaging
.PHONY: package
package: cpp
	@echo "Creating distribution packages..."
	@cd $(BUILD_DIR) && $(MAKE) package
	@$(PYTHON_EXECUTABLE) setup.py sdist bdist_wheel

# Flask development server
.PHONY: run-flask
run-flask: python
	@echo "Starting Flask development server..."
	@cd flask_app && $(PYTHON_EXECUTABLE) app.py

# Flask production server
.PHONY: run-production
run-production: python
	@echo "Starting Flask production server..."
	@cd flask_app && gunicorn -w 4 -b 0.0.0.0:5000 app:app

# Example programs
.PHONY: run-examples
run-examples: cpp
	@echo "Running example programs..."
	@$(BUILD_DIR)/mips_simulator_cli examples/sample_program.s
	@echo ""
	@$(BUILD_DIR)/mips_simulator_cli examples/test_hazards.s
	@echo ""
	@$(BUILD_DIR)/mips_simulator_cli examples/branch_test.s

# Benchmarking
.PHONY: benchmark
benchmark: cpp python
	@echo "Running performance benchmarks..."
	@$(PYTHON_EXECUTABLE) -m pytest tests/test_performance.py -v --benchmark-only

# Debug build
.PHONY: debug
debug:
	@$(MAKE) cpp CMAKE_BUILD_TYPE=Debug

# Release build
.PHONY: release
release:
	@$(MAKE) cpp CMAKE_BUILD_TYPE=Release

# Clean targets
.PHONY: clean
clean:
	@echo "Cleaning build artifacts..."
	@rm -rf $(BUILD_DIR)
	@rm -rf dist/
	@rm -rf *.egg-info/
	@find . -name "*.pyc" -delete
	@find . -name "__pycache__" -type d -exec rm -rf {} +
	@echo "Clean complete."

.PHONY: clean-all
clean-all: clean
	@echo "Cleaning all generated files..."
	@rm -rf htmlcov/
	@rm -rf .coverage
	@rm -rf .pytest_cache/
	@rm -rf .mypy_cache/

# Development utilities
.PHONY: check-deps
check-deps:
	@echo "Checking dependencies..."
	@command -v cmake >/dev/null 2>&1 || { echo "CMake not found. Please install CMake."; exit 1; }
	@command -v $(PYTHON_EXECUTABLE) >/dev/null 2>&1 || { echo "Python not found. Please install Python."; exit 1; }
	@$(PYTHON_EXECUTABLE) -c "import pybind11" 2>/dev/null || { echo "pybind11 not found. Run 'pip install pybind11'."; exit 1; }
	@echo "All dependencies satisfied."

.PHONY: setup-git-hooks
setup-git-hooks:
	@echo "Setting up Git hooks..."
	@echo '#!/bin/sh' > .git/hooks/pre-commit
	@echo 'make format lint' >> .git/hooks/pre-commit
	@chmod +x .git/hooks/pre-commit
	@echo "Git hooks installed."

# Continuous Integration targets
.PHONY: ci
ci: check-deps cpp test lint

.PHONY: ci-full
ci-full: check-deps cpp python test-coverage docs package

# Docker targets (if Dockerfile exists)
.PHONY: docker-build
docker-build:
	@if [ -f Dockerfile ]; then \
		echo "Building Docker image..."; \
		docker build -t mips-simulator .; \
	else \
		echo "Dockerfile not found."; \
	fi

.PHONY: docker-run
docker-run:
	@echo "Running Docker container..."
	@docker run -p 5000:5000 -it mips-simulator

# Information targets
.PHONY: info
info:
	@echo "MIPS Simulator Project Information"
	@echo "=================================="
	@echo "Build directory: $(BUILD_DIR)"
	@echo "Build type: $(CMAKE_BUILD_TYPE)"
	@echo "Python executable: $(PYTHON_EXECUTABLE)"
	@echo "Jobs: $(JOBS)"
	@echo ""
	@echo "Git status:"
	@git status --porcelain || echo "Not a git repository"
	@echo ""
	@echo "Dependencies:"
	@$(MAKE) check-deps

.PHONY: version
version:
	@echo "MIPS Simulator v1.0.0"
	@echo "Built with:"
	@cmake --version | head -1
	@$(PYTHON_EXECUTABLE) --version
	@g++ --version | head -1 || clang++ --version | head -1

# Make sure intermediate files are not deleted
.SECONDARY: