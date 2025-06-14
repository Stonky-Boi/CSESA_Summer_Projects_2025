# MIPS-Based CPU Simulator
![GitHub Created At](https://img.shields.io/github/created-at/Stonky-Boi/MIPS-Based CPU Simulator)
![GitHub contributors](https://img.shields.io/github/contributors/Stonky-Boi/MIPS-Based CPU Simulator)
![GitHub License](https://img.shields.io/github/license/Stonky-Boi/MIPS-Based CPU Simulator)

A comprehensive software-based simulator that emulates the execution of MIPS assembly code with support for instruction decoding, pipelined execution, hazard detection, and branch prediction.

## 🚀 Features

- **Complete MIPS Instruction Set Support**: R-type, I-type, and J-type instructions
- **5-Stage Pipeline Simulation**: Fetch, Decode, Execute, Memory, Write-back stages
- **Advanced Branch Prediction**: Static and dynamic predictors with statistics
- **Hazard Detection & Forwarding**: Data hazard detection with pipeline stalling
- **Memory Management**: Simulated main memory with configurable size
- **Register File Emulation**: 32 general-purpose MIPS registers
- **Web Interface**: Flask-based web application for interactive simulation
- **Python Bindings**: C++ core with Python wrapper for flexibility
- **Step-by-Step Execution**: Debug mode with cycle-by-cycle analysis
- **Performance Statistics**: CPI, branch prediction accuracy, hazard statistics

## 📁 Project Structure

```
mips_simulator/
├── src/
│   ├── core/                    # Core C++ simulator components
│   │   ├── mips_simulator.h     # Main simulator class
│   │   ├── mips_simulator.cpp
│   │   ├── instruction.h        # Instruction representation
│   │   ├── instruction.cpp
│   │   ├── memory.h             # Memory subsystem
│   │   ├── memory.cpp
│   │   ├── register_file.h      # Register file implementation
│   │   ├── register_file.cpp
│   │   ├── pipeline.h           # 5-stage pipeline
│   │   ├── pipeline.cpp
│   │   ├── branch_predictor.h   # Branch prediction unit
│   │   ├── branch_predictor.cpp
│   │   ├── hazard_detection.h   # Hazard detection unit
│   │   └── hazard_detection.cpp
│   └── utils/                   # Utility components
│       ├── instruction_parser.h # Assembly parser
│       ├── instruction_parser.cpp
│       ├── disassembler.h       # Instruction disassembler
│       └── disassembler.cpp
├── python_bindings/             # Python interface
│   ├── pybind_wrapper.cpp       # Pybind11 bindings
│   └── __init__.py
├── flask_app/                   # Web interface
│   ├── app.py                   # Flask application
│   ├── templates/
│   │   └── index.html           # Web interface template
│   └── static/
│       ├── style.css            # Styling
│       └── script.js            # Frontend JavaScript
├── tests/                       # Test suite
│   ├── test_mips_simulator.cpp
│   ├── test_pipeline.cpp
│   └── test_branch_predictor.cpp
├── examples/                    # Example programs
│   ├── sample_program.s
│   ├── test_hazards.s
│   └── branch_test.s
├── docs/                        # Documentation
│   ├── API.md
│   └── ARCHITECTURE.md
├── CMakeLists.txt               # CMake build configuration
├── setup.py                    # Python package setup
├── requirements.txt             # Python dependencies
├── README.md                    # This file
├── .gitignore                   # Git ignore patterns
└── Makefile                     # Build shortcuts
```

## 🛠️ Building and Installation

### Prerequisites

- **C++ Compiler**: GCC 8+ or Clang 10+ with C++17 support
- **CMake**: Version 3.15 or higher
- **Python**: Version 3.8 or higher
- **pybind11**: For Python bindings
- **Flask**: For web interface

### Option 1: Build with CMake (Recommended)

```bash
# Clone the repository
git clone https://github.com/your-repo/mips-simulator.git
cd mips-simulator

# Create build directory
mkdir build && cd build

# Configure with CMake
cmake -DCMAKE_BUILD_TYPE=Release ..

# Build the project
make -j$(nproc)

# Run tests (optional)
make test

# Install (optional)
sudo make install
```

### Option 2: Build Python Package

```bash
# Install Python dependencies
pip install -r requirements.txt

# Build and install Python package
pip install .

# Or for development
pip install -e .
```

### Option 3: Quick Build with Makefile

```bash
# Build everything
make all

# Build only C++ components
make cpp

# Build Python bindings
make python

# Clean build artifacts
make clean
```

## 🎯 Usage

### Command Line Interface

```bash
# Run a MIPS assembly program
./build/mips_simulator_cli examples/sample_program.s

# Interactive mode
./build/mips_simulator_cli --interactive

# Enable debug output
./build/mips_simulator_cli --debug examples/sample_program.s
```

### Python Interface

```python
import mips_simulator_core as mips

# Create simulator instance
sim = mips.MIPSSimulator(
    memory_size=1024*1024,
    enable_pipelining=True,
    enable_branch_prediction=True,
    predictor_type="dynamic"
)

# Load a program
success = sim.load_program("examples/sample_program.s")

# Execute step by step
while not sim.is_halted():
    sim.step()
    print(f"PC: 0x{sim.get_program_counter():08X}")
    print(f"Cycle: {sim.get_cycle_count()}")

# Get final statistics
stats = sim.get_statistics()
print(f"Instructions executed: {stats.instructions_executed}")
print(f"CPI: {stats.cpi:.2f}")
print(f"Branch prediction accuracy: {stats.branch_prediction_accuracy:.1%}")
```

### Web Interface

```bash
# Start Flask development server
cd flask_app
python app.py

# Open browser to http://localhost:5000
```

The web interface provides:
- **Program Editor**: Write and load MIPS assembly code
- **Execution Controls**: Step, run, reset buttons
- **State Visualization**: Registers, memory, pipeline view
- **Statistics Dashboard**: Performance metrics and graphs
- **Disassembly View**: Current instruction and program listing

## 📚 Supported Instructions

### R-Type Instructions
- **Arithmetic**: `add`, `addu`, `sub`, `subu`
- **Logical**: `and`, `or`, `xor`, `nor`
- **Comparison**: `slt`, `sltu`
- **Shift**: `sll`, `srl`, `sra`
- **Jump Register**: `jr`, `jalr`

### I-Type Instructions
- **Arithmetic Immediate**: `addi`, `addiu`
- **Logical Immediate**: `andi`, `ori`, `xori`
- **Load Upper**: `lui`
- **Memory Access**: `lw`, `sw`, `lb`, `sb`
- **Branches**: `beq`, `bne`, `blez`, `bgtz`

### J-Type Instructions
- **Jumps**: `j`, `jal`

### Special Instructions
- **No Operation**: `nop`
- **Halt**: `halt` (custom instruction to end simulation)

## 🏗️ Architecture Overview

### Core Components

1. **MIPSSimulator**: Main orchestrator class that coordinates all components
2. **Memory**: Simulates main memory with word/byte access
3. **RegisterFile**: Implements 32 general-purpose registers
4. **Pipeline**: 5-stage pipeline with hazard handling
5. **BranchPredictor**: Multiple prediction algorithms (static/dynamic)
6. **HazardDetection**: Detects and resolves data/control hazards

### Pipeline Stages

1. **IF (Instruction Fetch)**: Fetches instruction from memory
2. **ID (Instruction Decode)**: Decodes instruction and reads registers
3. **EX (Execute)**: Performs ALU operations
4. **MEM (Memory Access)**: Accesses data memory for load/store
5. **WB (Write Back)**: Writes results back to register file

### Branch Prediction

- **Static Predictors**: Always taken, always not taken, backward taken
- **Dynamic Predictors**: 1-bit, 2-bit saturating counter, GShare
- **Branch Target Buffer**: Caches branch target addresses
- **Return Address Stack**: Optimizes function return prediction

## 🧪 Testing

### Running Tests

```bash
# Build and run C++ tests
mkdir build && cd build
cmake -DBUILD_TESTS=ON ..
make
./run_tests

# Run Python tests
python -m pytest tests/

# Run with coverage
python -m pytest --cov=mips_simulator tests/
```

### Test Categories

- **Unit Tests**: Individual component testing
- **Integration Tests**: Multi-component interaction testing
- **Performance Tests**: Benchmarking and performance validation
- **Example Programs**: Real MIPS programs for end-to-end testing

## 📊 Performance Analysis

The simulator provides comprehensive performance metrics:

- **Cycles Per Instruction (CPI)**: Overall performance measure
- **Branch Prediction Accuracy**: Percentage of correct predictions
- **Cache Hit Rates**: Memory subsystem performance
- **Pipeline Utilization**: Efficiency of pipeline usage
- **Hazard Statistics**: Frequency and types of hazards

## 🐛 Debugging Features

- **Step-by-step execution**: Execute one instruction at a time
- **Breakpoints**: Pause execution at specific addresses
- **Register/memory inspection**: View and modify processor state
- **Disassembly view**: See assembly code with current position
- **Execution history**: Track instruction execution sequence
- **Pipeline visualization**: See instructions in each pipeline stage

## 🤝 Contributing

We welcome contributions! Please see our [Contributing Guidelines](CONTRIBUTING.md) for details.

### Development Setup

```bash
# Clone repository
git clone https://github.com/your-repo/mips-simulator.git
cd mips-simulator

# Install development dependencies
pip install -r requirements-dev.txt

# Install pre-commit hooks
pre-commit install

# Run development build
make dev
```

### Code Style

- **C++**: Follow Google C++ Style Guide
- **Python**: Use Black formatter and follow PEP 8
- **Documentation**: Use Doxygen for C++ and docstrings for Python

## 📖 Documentation

- **API Documentation**: [docs/API.md](docs/API.md)
- **Architecture Guide**: [docs/ARCHITECTURE.md](docs/ARCHITECTURE.md)
- **Examples**: See [examples/](examples/) directory
- **Online Docs**: https://mips-simulator.readthedocs.io/

## 📄 License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## 👥 Authors

- **MIPS Simulator Team** - *Initial work* - [GitHub](https://github.com/your-repo)

## 🙏 Acknowledgments

- Computer Organization and Design textbook by Patterson & Hennessy
- MIPS Technologies documentation
- Educational simulator projects that inspired this work
- Open source community for tools and libraries used

## 📞 Support

- **Issues**: [GitHub Issues](https://github.com/your-repo/mips-simulator/issues)
- **Discussions**: [GitHub Discussions](https://github.com/your-repo/mips-simulator/discussions)
- **Email**: developer@mipssimulator.org

---

**Happy Simulating! 🎉**