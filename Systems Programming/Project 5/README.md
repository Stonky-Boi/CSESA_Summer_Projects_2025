# MIPS-based CPU Simulator

## Overview

This project is a comprehensive software-based simulator for the MIPS instruction set architecture, featuring advanced processor design concepts including 5-stage pipelining, hazard detection, and branch prediction capabilities. The simulator combines a robust C++ backend with an interactive Flask web interface, making it an excellent educational tool for understanding computer architecture principles.

## Features

### Core Simulator Capabilities

The MIPS simulator implements a complete instruction set architecture supporting all three major MIPS instruction types:

- **R-Type Instructions**: Arithmetic operations (ADD, SUB), logical operations (AND, OR, NOR, XOR), shift operations (SLL, SRL, SRA), comparison operations (SLT), and jump register operations (JR)
- **I-Type Instructions**: Immediate arithmetic (ADDI, ANDI, ORI, XORI), memory access (LW, LH, LB, SW, SH, SB), branch operations (BEQ, BNE, BLEZ, BGTZ), and load upper immediate (LUI)
- **J-Type Instructions**: Jump operations (J, JAL) for program control flow

### Advanced Processor Features

The simulator incorporates sophisticated computer architecture concepts:

**5-Stage Pipeline Implementation**: The processor pipeline includes Instruction Fetch (IF), Instruction Decode (ID), Execute (EX), Memory Access (MEM), and Write Back (WB) stages. Each stage operates concurrently to improve instruction throughput and overall processor performance.

**Comprehensive Hazard Detection**: The system detects and handles data hazards (RAW dependencies), control hazards (branch instructions), and structural hazards (resource conflicts). The pipeline includes forwarding mechanisms to minimize stalls and maintain performance.

**Branch Prediction Algorithms**: Multiple prediction strategies are implemented including static predictors (always taken, always not taken, BTFN), dynamic predictors (1-bit bimodal, 2-bit bimodal), and advanced predictors (Gshare, local history, tournament).

### Interactive Web Interface

The Flask-based web interface provides:

- **Real-time Code Editor**: Interactive environment for writing and editing MIPS assembly code
- **Live Visualization**: Dynamic display of register states, pipeline status, and memory contents during execution
- **Step-by-Step Execution**: Granular control allowing instruction-by-instruction program analysis
- **Configuration Options**: Flexible settings for pipeline parameters and branch predictor selection
- **Example Programs**: Pre-loaded educational examples demonstrating various MIPS programming concepts

## Project Structure

```
Project 5/
├── include/                 # Header files (.hpp)
│   ├── Pipeline.hpp        # 5-stage pipeline implementation
│   ├── alu.hpp            # Arithmetic Logic Unit operations
│   ├── branch_predictor.hpp # Branch prediction algorithms
│   ├── instruction_decoder.hpp # Instruction parsing and decoding
│   └── mips_simulator.hpp  # Main simulator class
├── src/                    # Implementation files (.cpp)
│   ├── Pipeline.cpp        # Pipeline stage management
│   ├── alu.cpp            # ALU operation implementations
│   ├── branch_predictor.cpp # Prediction algorithm logic
│   ├── cli_interface.cpp   # Command-line interface
│   ├── instruction_decoder.cpp # Instruction decoding logic
│   ├── main.cpp           # Main program entry point
│   └── mips_simulator.cpp  # Core simulator implementation
├── web/                    # Flask web interface
│   ├── app.py             # Flask application server
│   ├── templates/         # HTML templates
│   │   ├── documentation.html # User documentation
│   │   └── index.html     # Main simulator interface
│   └── static/            # Static web assets
│       ├── css/
│       │   └── styles.css  # Application styling
│       └── js/
│           └── simulator.js # Frontend JavaScript logic
├── CMakeLists.txt          # CMake build configuration
└── README.md              # Project documentation
```

## Installation and Setup

### Prerequisites

The project requires several development tools and libraries:

- **C++ Compiler**: GCC 7+ or Clang 6+ with C++17 standard support
- **Build System**: CMake version 3.12 or higher for cross-platform compilation
- **Python Environment**: Python 3.7+ for the web interface components
- **Make Utility**: Standard build automation tool

### Building the C++ Simulator

1. **Repository Setup**:
   ```bash
   git clone 
   cd MIPS_Simulator
   ```

2. **Build Directory Creation**:
   ```bash
   mkdir build
   cd build
   ```

3. **Configuration and Compilation**:
   ```bash
   cmake ..
   make
   ```

4. **Optional Testing**:
   ```bash
   make test
   ```

### Web Interface Configuration

1. **Python Dependencies**:
   ```bash
   pip install -r requirements.txt
   ```

2. **Web Server Launch**:
   ```bash
   cd web
   python app.py
   ```

3. **Browser Access**: Navigate to `http://localhost:5000` for the interactive interface

## Usage Instructions

### Command Line Operation

The simulator supports various execution modes through command-line parameters:

```bash
./mips_simulator  [options]
```

**Available Options**:
- `--step`: Enable step-by-step execution for detailed program analysis
- `--pipeline`: Activate 5-stage pipeline simulation
- `--branch-pred`: Enable branch prediction mechanisms
- `--pred-type TYPE`: Specify predictor type (static, 1bit, 2bit)

**Example Usage**:
```bash
./mips_simulator program.txt --pipeline --branch-pred --pred-type 2bit
```

### Interactive CLI Interface

The command-line interface provides comprehensive debugging capabilities:

**Program Control Commands**:
- `load `: Load MIPS program from file
- `loadhex`: Interactive hexadecimal program input
- `step` or `s`: Execute single instruction
- `run` or `r`: Execute complete program
- `reset`: Reset simulator to initial state

**State Inspection Commands**:
- `state` or `st`: Display complete system state
- `registers` or `reg`: Show all register values
- `memory `: Display memory contents at specified address
- `disasm `: Disassemble instruction at given address

**Configuration Commands**:
- `pipeline `: Toggle pipeline simulation
- `branch  [type]`: Configure branch prediction
- `stats`: Display performance statistics

### Web Interface Usage

The browser-based interface offers intuitive program development and analysis:

1. **Program Entry**: Input MIPS machine code in hexadecimal format or select from example programs
2. **Execution Control**: Use Step button for instruction-by-instruction analysis or Run for complete execution
3. **Real-time Monitoring**: Observe register changes, pipeline states, and memory modifications during execution
4. **Performance Analysis**: View branch prediction statistics and pipeline utilization metrics

## Example Programs

### Basic Arithmetic Operations

```assembly
# Simple addition program
20020005  # addi $v0, $zero, 5
20030003  # addi $v1, $zero, 3
00622020  # add $a0, $v1, $v0
AC040000  # sw $a0, 0($zero)
```

### Loop Implementation

```assembly
# Counting loop demonstration
20020000  # addi $v0, $zero, 0     # counter = 0
2003000A  # addi $v1, $zero, 10    # limit = 10
20420001  # addi $v0, $v0, 1       # counter++
1443FFFD  # bne $v0, $v1, -3       # branch if not equal
```

### Function Call Example

```assembly
# Function call with return
20040005  # addi $a0, $zero, 5     # argument setup
0C000010  # jal function           # call function
00822020  # add $a0, $a0, $v0      # result processing
03E00008  # jr $ra                 # return
```

## Performance Analysis

The simulator provides comprehensive performance metrics:

- **CPI (Cycles Per Instruction)**: Measures overall pipeline efficiency and instruction throughput
- **Hazard Statistics**: Detailed counts of data, control, and structural hazards encountered during execution
- **Branch Prediction Accuracy**: Success rate analysis for different prediction algorithms
- **Pipeline Utilization**: Stage-by-stage utilization statistics showing pipeline efficiency
- **Memory Access Patterns**: Analysis of load/store operation characteristics

## Troubleshooting

### Common Issues and Solutions

**Instruction Format Errors**:
- Ensure all instructions use 32-bit hexadecimal format (exactly 8 characters)
- Verify that comments follow the # symbol convention
- Check for proper line termination and spacing

**Memory Access Problems**:
- Confirm memory addresses fall within the valid range (0-65535)
- Ensure word-aligned access for load/store operations
- Initialize memory locations before attempting read operations

**Pipeline Behavior Issues**:
- Data hazards may cause automatic pipeline stalls
- Branch mispredictions will trigger pipeline stage flushing
- Use step-by-step execution mode for detailed pipeline analysis

### Best Practices

- Begin with simple programs and gradually increase complexity
- Utilize step-by-step execution for comprehensive debugging
- Monitor register and memory states throughout program execution
- Enable pipeline visualization to understand instruction flow patterns
- Analyze branch prediction statistics for performance optimization insights

## Development and Extension

### Adding New Instructions

1. Define instruction format specifications in `Instruction.hpp`
2. Implement decoding logic in `Instruction.cpp`
3. Add execution logic to `MIPS.cpp` and `Pipeline.cpp`
4. Update hazard detection mechanisms in `HazardDetection.cpp` if required

### Implementing New Branch Predictors

1. Create predictor class inheriting from `BranchPredictor` base class
2. Implement prediction and update methods according to algorithm specifications
3. Add predictor to factory pattern in `BranchPredictorFactory::create`
4. Update web interface predictor selection options

### Web Interface Enhancement

1. Add new API endpoints in `web/app.py` for additional functionality
2. Create or modify HTML templates in `web/templates/` directory
3. Implement JavaScript functionality in `web/static/js/`
4. Update CSS styling in `web/static/css/` for visual improvements

## Educational Applications

This MIPS simulator serves as an comprehensive educational tool for computer architecture courses, providing hands-on experience with:

- **Instruction Set Architecture**: Understanding different instruction types and their encoding
- **Pipeline Design**: Visualizing how modern processors achieve instruction-level parallelism
- **Hazard Management**: Learning about data dependencies and their resolution strategies
- **Branch Prediction**: Exploring different prediction algorithms and their performance implications
- **Performance Analysis**: Understanding the relationship between architectural features and system performance

The combination of command-line and web interfaces accommodates different learning styles and use cases, from detailed debugging sessions to interactive demonstrations.
