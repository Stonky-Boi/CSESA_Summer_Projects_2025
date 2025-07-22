```markdown
# MIPS Assembly Simulator

This project is a C++ based MIPS I instruction set simulator with a Python Dash/Flask web interface. It is designed to parse and execute MIPS assembly code, emulating the register file, memory, and the fetch-decode-execute cycle of a basic MIPS processor.

## Features

-   **MIPS Instruction Set**: Implements a core subset of MIPS instructions, including R-Type, I-Type, and J-Type.
-   **Memory & Registers**: Emulates a 32-register file and a 4KB main memory space.
-   **MARS-like Syntax**: Parses `.data` and `.text` sections, supporting labels and `.asciiz` directives.
-   **Web Interface**: A user-friendly interface built with Dash to write code, execute it, and view the system state.
-   **Execution Modes**:
    -   **Full Run**: Executes the entire program at once.
    -   **Step-by-Step**: Executes one instruction at a time, allowing for detailed debugging.
-   **State Display**: The UI displays the current values of the Program Counter (PC), all 32 registers, and the relevant memory segments.

## Project Structure

```
/
|-- CMakeLists.txt
|-- README.md
|-- requirements.txt
|-- include/
|   |-- instruction.hpp
|   |-- memory.hpp
|   |-- mips_simulator.hpp
|   |-- register_file.hpp
|-- src/
|   |-- instruction.cpp
|   |-- main.cpp
|   |-- memory.cpp
|   |-- mips_simulator.cpp
|   |-- register_file.cpp
|-- web/
    |-- app.py
```

## Build and Run

### Prerequisites

-   A C++ compiler that supports C++17 (e.g., GCC, Clang)
-   CMake (version 3.10 or higher)
-   Python (version 3.8 or higher)

### 1. Build the C++ Simulator

First, compile the C++ backend.

```
# Create a build directory
mkdir build
cd build

# Configure the project with CMake
cmake ..

# Build the executable
make
```

This will create an executable named `mips_simulator` in the `web` directory.

### 2. Set up the Python Environment

Install the necessary Python packages for the web interface.

```
# Navigate to the root directory
pip install -r requirements.txt
```

### 3. Run the Application

Once the C++ backend is built and Python dependencies are installed, run the web server.

```
# Navigate to the web directory
cd web

# Run the Flask/Dash application
python app.py
```

Open your web browser and navigate to `http://127.0.0.1:8050` to use the simulator.
```
