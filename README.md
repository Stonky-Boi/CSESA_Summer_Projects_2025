# MIPS Assembly Simulator
![GitHub Created At](https://img.shields.io/github/created-at/Stonky-Boi/Ember)
![GitHub contributors](https://img.shields.io/github/contributors/Stonky-Boi/Ember)
![GitHub License](https://img.shields.io/github/license/Stonky-Boi/Ember)

Ember is a bare-metal, cycle-accurate MIPS CPU simulator built with a high-performance C++ backend and a reactive Flutter graphical user interface. 

Unlike educational simulators that abstract away hardware realities, Ember strictly enforces authentic ISA execution. It simulates a 5-stage pipeline (Fetch, Decode, Execute, Memory, Writeback), resolves data hazards via multiplexed forwarding or load-use stalls, and implements swappable hardware branch predictors to visualize cycle-by-cycle execution.

## Core Features
* **Cycle-Accurate Pipeline:** Watch instructions flow through the 5 stages of a simulated datapath in real-time.
* **Hardware Branch Prediction:** Test the efficiency of your code against static and dynamic history-based predictors.
* **Data Hazard Resolution:** The simulator automatically detects dependencies, applying ALU forwarding or injecting `NOP` bubbles when necessary.
* **Strict Hardware Syntax:** Enforces true architectural MIPS syntax (no pseudo-instructions), providing a realistic low-level programming experience.
* **Reactive GUI:** A Flutter desktop app that communicates with the C++ engine via JSON IPC to instantly visualize memory arrays, register mutations, and pipeline latches.

---

## Prerequisites
To compile and run Ember, you need the following installed on your system:
* **C++ Build Tools:** `CMake` (v3.10+) and a C++17 compatible compiler (`g++` or `clang`).
* **Flutter SDK:** Flutter v3.10+ (configured for desktop development: macOS, Windows, or Linux).

---

## Setup & Installation

Ember is designed as a monorepo containing both the C++ simulation engine and the Flutter UI. You must compile the backend before the frontend can run.

### 1. Build the C++ Engine
Navigate to the `backend` directory, generate the CMake build files, and compile the executable.
```bash
cd backend
mkdir build
cd build
cmake ..
make
```

This will generate the `ember` executable inside the `backend/build/` directory.

### 2. Configure the Flutter Frontend

Navigate to the `frontend` directory and fetch the required Dart dependencies (like `provider`).

```bash
cd ../../frontend
flutter pub get
```

**⚠️ macOS Users - Important Sandbox Note:**
By default, Apple prevents Flutter desktop apps from executing local C++ binaries. To use the Ember UI on a Mac, you must disable the App Sandbox.

1. Open `frontend/macos/Runner/DebugProfile.entitlements` and `Release.entitlements`.
2. Change the value of `<key>com.apple.security.app-sandbox</key>` from `<true/>` to `<false/>`.
3. Run `flutter clean` before launching the app.

---

## Usage: Graphical User Interface

The easiest way to use Ember is through the Flutter desktop dashboard.

```bash
cd frontend
flutter run -d macos   # Replace 'macos' with 'windows' or 'linux' depending on your OS
```

1. **Write Code:** Type or paste your MIPS assembly into the left-hand editor panel.
2. **Select Predictor:** Choose a branch prediction architecture from the dropdown (e.g., `always_not_taken`, `two_level`).
3. **Compile & Run:** Click the run button to send the code to the C++ engine.
4. **Step Through Time:** Use the control panel to step forward and backward through individual clock cycles. Watch the Pipeline, Register Grid, and Memory Viewer update dynamically.

---

## Usage: Command Line Interface (Headless Mode)

You can bypass the UI and run the C++ simulator directly from the terminal. This is highly useful for generating statistical execution reports (CPI, stall counts, branch mispredictions).

```bash
cd backend/build
./ember <path_to_assembly_file> [options]
```

### CLI Arguments

* `--mode=<type>`:
* `interpreter`: Executes sequentially (1 instruction = 1 cycle).
* `pipeline` (Default): Simulates the 5-stage superscalar hardware pipeline.


* `--predictor=<type>`: Selects the branch prediction algorithm (only active if mode is `pipeline`).
* `always_not_taken` (Default)
* `always_taken`
* `one_bit` / `two_bit`
* `local_history` / `global_history` / `two_level` (GShare)


* `--trace`: Prints the raw assembly instructions residing in the IF, ID, EX, and MEM latches every clock cycle.
* `--json`: Outputs the execution trace as a structured JSON stream (used internally by the Flutter UI).

**Example CLI Command:**

```bash
./ember ../../test_programs/fibonacci.s --mode=pipeline --predictor=two_level --trace
```

---

## Writing Ember Assembly

Ember is a **strict hardware simulator**. It does not feature a macro-expander for pseudo-instructions. Programs must be written exactly as the CPU executes them.

* **No Pseudo-Instructions:** You cannot use `LI` (Load Immediate) or `MOVE`. You must use `ADDI $v0, $zero, 10` or `ADD $t1, $t2, $zero`.
* **Hardware Memory Addressing:** You cannot load a `.data` label directly (e.g., `LW $t0, my_var`). You must establish a base pointer using `LUI` and use explicit byte offsets.
* **Capitalization:** Instruction mnemonics (`ADD`, `SW`, `SYSCALL`) must be uppercase. Registers (`$t0`, `$zero`) must be lowercase.

### Example Program (Fibonacci Sequence to Memory)

Here is a valid, Ember-compatible program to test the pipeline and memory viewer:

```assembly
.data
fib0: .word 0
fib1: .word 1

.text
main:
    # 1. Setup Base Memory Pointer (0x10010000)
    LUI $t9, 4097          
    ADDI $t8, $t9, 8       
    
    # 2. Setup Counters
    ADDI $t0, $zero, 6     
    ADDI $t4, $zero, 1     

    # 3. Load Base Values
    LW $t1, 0($t9)         
    LW $t2, 4($t9)         

loop:
    BEQ $t0, $zero, end    
    
    # 4. Calculate & Store
    ADD $t3, $t1, $t2      
    SW $t3, 0($t8)         
    
    # 5. Shift Window
    ADD $t1, $t2, $zero    
    ADD $t2, $t3, $zero    
    
    # 6. Update Pointers & Loop
    ADDI $t8, $t8, 4       
    SUB $t0, $t0, $t4      
    J loop                 

end:
    ADDI $v0, $zero, 10    
    SYSCALL
```

## Building a Standalone Application

You can compile Ember into a standalone macOS application bundle (`.app`) that internally packages the C++ simulation engine.

### 1. Package the Release Backend
Build an optimized release version of the C++ engine and stage it in the Flutter assets directory:
```bash
cd backend
rm -rf build && mkdir build && cd build
cmake -DCMAKE_BUILD_TYPE=Release ..
make

cd ../../frontend
mkdir -p assets
cp ../backend/build/ember assets/ember
```

### 2. Compile the Native App
Ensure `assets/ember` is registered under the `assets:` section in your `frontend/pubspec.yaml`. Then, build the macOS application:
```bash
flutter build macos
```
Your compiled application will be generated at `build/macos/Build/Products/Release/frontend.app`. You can rename this to `Ember.app` and move it to your `/Applications` directory.

### 3. Apply the Custom App Icon
To apply the custom Ember icon to your standalone app:
1. Open `frontend/lib/icon/ember.svg` in a web browser and copy the image to your clipboard.
2. Right-click your `Ember.app` in Finder and select **Get Info** (`Cmd + I`).
3. Click the small generic app icon in the top-left corner of the window to highlight it.
4. Press `Cmd + V` to paste the custom SVG icon.