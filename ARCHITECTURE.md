# Ember MIPS Architecture Reference

Ember enforces a strict, bare-metal **MIPS32 Release 6** instruction set. Unlike educational simulators (such as MARS or SPIM) that utilize a macro-assembler to hide hardware complexities, Ember requires developers to write authentic, cycle-accurate hardware instructions. 

This document outlines the memory layout, supported instruction set, and strict architectural rules required to write compatible assembly for the Ember simulation engine. 

> Ember implements only the base CPU architecture. Extensions such as DSP modules, coprocessors, or virtualization are not included.

## 1. Memory Map

Ember implements a unified, flat, byte-addressable memory model using a **little-endian** layout. While the architecture defines multiple data formats, Ember primarily operates on 32-bit words.

The simulator supports two assembly sections:
* **`.text`** (`0x00400000`): Instruction memory. The Program Counter (PC) initializes here.
* **`.data`** (`0x10010000`): Static data and variables.

> `.stack` or `.heap` sections are not explicitly implemented. Stack behavior is managed directly by programs using normal memory and the `$sp` register.

**Important Addressing Rule:** Ember does *not* support direct label addressing in memory instructions. You cannot write `LW $t0, my_variable`. You must manually load the `.data` base address into a pointer register and use standard byte offsets.

```assembly
# CORRECT: Hardware-accurate memory access
LUI $t9, 4097          # Load upper immediate: 4097 = 0x1001. $t9 becomes 0x10010000
LW $t0, 0($t9)         # Load the first word from the data segment
LW $t1, 4($t9)         # Load the second word from the data segment
```

## 2. The Register File

Ember provides 32 general-purpose 32-bit integer registers. Standard MIPS ABI naming conventions are supported by the lexer and UI, as well as numeric names (`r0`-`r31`).

**Architectural Notes:**

  * **`$zero` (`r0`)**: Hard-wired to zero. Any attempt to write to `r0` is ignored.
  * **Program Counter (PC)**: Maintained internally. It is not directly accessible as a register but is modified indirectly via control-flow instructions.
  * **`HI` / `LO` Registers**: As per MIPS32 Release 6, these legacy multiplication/division registers have been removed and are not implemented in Ember.

| Number | Name | Usage | Preserved on Call? |
| :--- | :--- | :--- | :--- |
| **0** | `$zero` | Constant 0 (Hardware wired to 0) | N/A |
| **1** | `$at` | Assembler Temporary (Reserved) | No |
| **2-3** | `$v0 - $v1` | Values for results and Syscall codes | No |
| **4-7** | `$a0 - $a3` | Function arguments | No |
| **8-15** | `$t0 - $t7` | Temporary registers | No |
| **16-23** | `$s0 - $s7` | Saved registers | **Yes** |
| **24-25** | `$t8 - $t9` | Additional temporary registers | No |
| **26-27** | `$k0 - $k1` | Reserved for OS kernel | N/A |
| **28** | `$gp` | Global Pointer | **Yes** |
| **29** | `$sp` | Stack Pointer | **Yes** |
| **30** | `$fp` | Frame Pointer | **Yes** |
| **31** | `$ra` | Return Address (populated by JAL) | No |

## 3. Strict Hardware Syntax (No Pseudo-Instructions)

Ember **does not** feature a macro-expander. Pseudo-instructions are strictly prohibited. You must translate common pseudo-instructions into their true hardware equivalents before assembling.

| Prohibited Pseudo-Instruction | Ember Hardware Translation | Description |
| :--- | :--- | :--- |
| `LI $t0, 10` | `ADDI $t0, $zero, 10` | Load Immediate |
| `MOVE $t1, $t2` | `ADD $t1, $t2, $zero` | Move Register |
| `BGT $t0, $t1, loop` | `SLT $at, $t1, $t0`<br>`BNE $at, $zero, loop` | Branch if Greater Than (2 regs) |
| `BLE $t0, $t1, loop` | `SLT $at, $t1, $t0`<br>`BEQ $at, $zero, loop` | Branch if Less/Equal (2 regs) |
| `LA $t0, label` | `LUI $t0, upper(label)`<br>`ORI $t0, $t0, lower(label)` | Load Address (Manual) |

## 4. Supported Instruction Set

The instruction set implemented in Ember provides broad functionality while avoiding unnecessary complexity. *(Note: Some comparison instructions included here provide redundant functionality but are retained as native instructions to simplify assembly programming).*

All mnemonics must be **UPPERCASE**. Registers must be **lowercase**.

### Arithmetic

  * `ADD`, `ADDI`, `ADDU`, `SUB`, `SUBU`
  * `MUL`, `MUH`, `MULU`, `MUHU` *(Release 6 standard, outputs directly to GPRs)*
  * `DIV`, `DIVU`, `MOD`, `MODU`

### Logical

  * `AND`, `ANDI`, `OR`, `ORI`, `XOR`, `XORI`, `NOR`, `NAND`

### Shift and Rotate

  * `SLL`, `SRL`, `SRA`
  * `SLLV`, `SRLV`, `SRAV`
  * `ROTL`, `ROTR`, `ROTLV`, `ROTRV`

### Comparison

  * `SLT`, `SGT`, `SLE`, `SGE`, `SEQ`, `SNE`
  * `SLTI`, `SGTI`, `SLEI`, `SGEI`, `SEQI`, `SNEI`
  * `SLTU`, `SGTU`, `SLEU`, `SGEU`
  * `SLTIU`, `SGTIU`, `SLEIU`, `SGEIU`

### Bit Manipulation & Selection

  * **Bit Ops:** `CLO`, `CLZ`, `EXT`, `INS`, `SEB`, `SEH`
  * **Conditional Select:** `SELEQZ`, `SELNEZ`

### Memory Access & Address Generation

  * **Generation:** `LUI`, `LSA`
  * **Loads:** `LB`, `LBU`, `LH`, `LHU`, `LW`, `LL`
  * **Stores:** `SB`, `SH`, `SW`, `SC`

### Control Flow

  * **Jumps:** `J`, `JR`, `JAL`, `JALR`
  * **Branches:** `B`, `BAL`, `BEQ`, `BNE`, `BGEZ`, `BLEZ`, `BGTZ`, `BLTZ`
  * *(All branches in Ember resolve **without** a delay slot to simplify the pipeline).*

### System / Exception

  * `NOP`, `BREAK`, `SYSCALL`, `TEQ`, `TNE`

## 5. Execution Modes, Pipeline Dynamics & Hazards

Ember supports two execution modes:

1.  **Instruction Interpreter:** Sequential execution of instructions without pipeline simulation (1 instruction = 1 cycle).
2.  **Pipelined Execution:** Cycle-accurate simulation modeling the classic 5-stage MIPS datapath: **Fetch (IF)**, **Decode (ID)**, **Execute (EX)**, **Memory (MEM)**, and **Writeback (WB)**.

### Data Hazards & Forwarding

Ember utilizes a Hardware Hazard Unit to resolve Read-After-Write (RAW) data dependencies.

  * **ALU-to-ALU Forwarding:** Results are forwarded directly from the EX/MEM latch to the ALU input, incurring **0 stall cycles**.
  * **Load-Use Stalls:** If an instruction requires a value currently being loaded from memory (`LW`), the Hazard Unit injects a **1-cycle `NOP` bubble** to allow the memory read to complete.

### Control Hazards & Branch Prediction

Branch conditions are evaluated in the Execute stage. Ember supports modular, swappable branch predictors to handle control hazards. Mispredictions flush the erroneously fetched instructions (IF and ID stages).

  * **Static Predictors:** Always Taken, Always Not Taken.
  * **Dynamic Predictors:** 1-Bit Predictor, 2-Bit Saturating Counter, Local History, Global History, and Two-Level Adaptive Predictor (GShare).

## 6. Execution Trace & Statistics

When tracing is enabled, Ember produces a detailed pipeline trace showing the instruction present in each stage during every cycle, along with PC values, register accesses, and memory mutations.

The simulator dynamically tracks and reports:

  * **Instruction Stats:** Total instructions executed, categorized instruction counts.
  * **Pipeline Stats:** Total cycles, overall CPI, total stalls, and pipeline flushes.
  * **Hazard & Memory Stats:** Total RAW hazards, loads executed, and stores executed.
  * **Branch Stats:** Total branches executed, branches taken, mispredictions, and overall predictor accuracy.

## 7. System Coprocessor (SYSCALL)

Ember supports environment calls via the `SYSCALL` instruction. The desired operation code must be loaded into register `$v0` prior to execution. Since floating-point operations are not supported, float-based syscalls are omitted.

| Code | Operation | Arguments | Result |
| :--- | :--- | :--- | :--- |
| **1** | Print Integer | `$a0` = Integer to print | Console output |
| **4** | Print String | `$a0` = Address of null-terminated string | Console output |
| **5** | Read Integer | None | `$v0` = Integer read from user |
| **8** | Read String | `$a0` = Buffer address, `$a1` = Max length | String written to memory |
| **10** | Exit Program | None | Halts the simulator gracefully |
| **11** | Print Character | `$a0` = Character to print | Console output |
| **12** | Read Character | None | `$v0` = Character read from user |