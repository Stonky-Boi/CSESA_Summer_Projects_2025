# MIPS Simulator

A comprehensive software-based simulator for the MIPS instruction set architecture with 5-stage pipelining, hazard detection, and branch prediction capabilities. The simulator includes both a C++ backend and a Flask web interface for easy interaction.

## Features

### Core Simulator Features
- **Complete MIPS Instruction Set**: Supports R-type, I-type, and J-type instructions
- **5-Stage Pipeline**: Implements fetch, decode, execute, memory, and writeback stages
- **Hazard Detection**: Detects and handles data hazards, control hazards, and structural hazards
- **Forwarding Unit**: Implements data forwarding to minimize pipeline stalls
- **Branch Prediction**: Multiple prediction algorithms including:
  - Static predictors (always taken, always not taken, BTFN)
  - Dynamic predictors (1-bit bimodal, 2-bit bimodal)
  - Advanced predictors (Gshare, local history, tournament)
- **Performance Analysis**: Detailed statistics on CPI, hazards, and prediction accuracy

### Web Interface Features
- **Interactive Code Editor**: Write and edit MIPS assembly code
- **Real-time Visualization**: View register states, pipeline status, and memory contents
- **Step-by-step Execution**: Step through program execution instruction by instruction
- **Configuration Options**: Configure pipeline settings and branch predictor parameters
- **Example Programs**: Pre-loaded example programs for learning and testing
- **Performance Metrics**: Real-time display of execution statistics

## Project Structure

```
.
├── include/                 # Header files (.hpp)
│   ├── MIPS.hpp            # Main MIPS processor class
│   ├── Instruction.hpp     # Instruction representation and decoding
│   ├── Pipeline.hpp        # 5-stage pipeline implementation
│   ├── HazardDetection.hpp # Hazard detection and forwarding
│   └── BranchPredictor.hpp # Branch prediction algorithms
├── src/                    # Implementation files (.cpp)
│   ├── MIPS.cpp
│   ├── Instruction.cpp
│   ├── Pipeline.cpp
│   ├── HazardDetection.cpp
│   └── BranchPredictor.cpp
├── web/                    # Flask web interface
│   ├── app.py             # Main Flask application
│   ├── templates/         # HTML templates
│   │   ├── base.html
│   │   ├── index.html
│   │   └── simulator.html
│   └── static/            # Static assets
│       ├── css/
│       │   └── style.css
│       └── js/
│           └── simulator.js
├── CMakeLists.txt         # CMake build configuration
├── requirements.txt       # Python dependencies
└── README.md             # This file
```

## Installation and Setup

### Prerequisites
- **C++ Compiler**: GCC 7+ or Clang 6+ with C++17 support
- **CMake**: Version 3.12 or higher
- **Python**: Version 3.7 or higher
- **Make**: Build system

### Building the C++ Simulator

1. **Clone the repository**:
   ```bash
   git clone <repository-url>
   cd MIPS_Simulator
   ```

2. **Create build directory**:
   ```bash
   mkdir build
   cd build
   ```

3. **Configure and build**:
   ```bash
   cmake ..
   make
   ```

4. **Run tests** (optional):
   ```bash
   make test
   ```

### Setting up the Web Interface

1. **Install Python dependencies**:
   ```bash
   pip install -r requirements.txt
   ```

2. **Navigate to web directory**:
   ```bash
   cd web
   ```

3. **Run the Flask application**:
   ```bash
   python app.py
   ```

4. **Access the web interface**:
   Open your browser and go to `http://localhost:5000`

## Usage

### Web Interface Usage

1. **Access the Simulator**: Open `http://localhost:5000` in your web browser
2. **Write Assembly Code**: Enter MIPS assembly code in the editor or select an example
3. **Configure Settings**: 
   - Enable/disable pipeline
   - Select branch predictor type
   - Adjust predictor parameters
4. **Run Simulation**: Click "Run" for full execution or "Step" for step-by-step execution
5. **Analyze Results**: View registers, pipeline state, memory contents, and performance statistics

### Command Line Usage

The C++ simulator can also be run directly from the command line:

```bash
./mips_simulator --input program.asm --config config.json --output results.json
```

### Example Programs

The simulator includes several example programs:

- **Basic Arithmetic**: Simple ADD, SUB, and immediate operations
- **Loop Example**: Demonstrates branching and loop structures
- **Factorial Calculation**: Recursive computation example
- **Memory Access**: Load and store operations

## Supported MIPS Instructions

### R-Type Instructions
- Arithmetic: `ADD`, `SUB`, `MULT`, `DIV`
- Logical: `AND`, `OR`, `NOR`, `XOR`
- Shift: `SLL`, `SRL`, `SRA`
- Comparison: `SLT`
- Jump: `JR`, `JALR`

### I-Type Instructions
- Immediate arithmetic: `ADDI`, `ANDI`, `ORI`, `XORI`
- Memory access: `LW`, `LH`, `LB`, `SW`, `SH`, `SB`
- Branches: `BEQ`, `BNE`, `BLEZ`, `BGTZ`
- Load upper: `LUI`

### J-Type Instructions
- Jumps: `J`, `JAL`

## Branch Prediction Algorithms

### Static Predictors
- **Always Not Taken**: Predicts all branches as not taken
- **Always Taken**: Predicts all branches as taken
- **BTFN**: Backward taken, forward not taken

### Dynamic Predictors
- **1-bit Bimodal**: Single bit per entry prediction table
- **2-bit Bimodal**: Two-bit saturating counter predictor
- **Gshare**: Global history XOR with PC
- **Local History**: Per-branch local history predictor
- **Tournament**: Hybrid predictor combining global and local predictors

## Performance Metrics

The simulator provides detailed performance analysis:

- **CPI (Cycles Per Instruction)**: Overall pipeline efficiency
- **Hazard Statistics**: Data, control, and structural hazard counts
- **Branch Prediction Accuracy**: Prediction success rate
- **Pipeline Utilization**: Stage-by-stage utilization statistics
- **Memory Access Patterns**: Load/store operation analysis

## Development

### Adding New Instructions

1. **Define instruction format** in `Instruction.hpp`
2. **Implement decoding logic** in `Instruction.cpp`
3. **Add execution logic** in `MIPS.cpp` and `Pipeline.cpp`
4. **Update hazard detection** in `HazardDetection.cpp` if needed

### Adding New Branch Predictors

1. **Create predictor class** inheriting from `BranchPredictor`
2. **Implement prediction and update methods**
3. **Add to factory in `BranchPredictorFactory::create`**
4. **Update web interface predictor options**

### Extending the Web Interface

1. **Add new API endpoints** in `web/app.py`
2. **Create or modify templates** in `web/templates/`
3. **Add JavaScript functionality** in `web/static/js/`
4. **Update CSS styles** in `web/static/css/`

## Testing

Run the test suite to verify functionality:

```bash
# C++ tests
cd build
make test

# Python tests
cd web
python -m pytest
```

## Contributing

1. Fork the repository
2. Create a feature branch
3. Make your changes
4. Add tests for new functionality
5. Submit a pull request

## License

This project is licensed under the MIT License. See LICENSE file for details.

## Acknowledgments

- Based on the MIPS architecture specification
- Inspired by classic computer architecture textbooks
- Uses modern C++17 features for clean, efficient code
- Flask web framework for the user interface

## Contact

For questions, issues, or contributions, please create an issue on the repository or contact the development team.

---

**Note**: This simulator is designed for educational purposes to understand computer architecture concepts including instruction set architectures, pipelining, hazard detection, and branch prediction.