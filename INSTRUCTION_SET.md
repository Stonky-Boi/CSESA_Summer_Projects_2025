# Ember Instruction Set Reference

This document details every MIPS32 Release 6 instruction supported by the Ember simulator. Because Ember is a strict hardware simulator, you must provide exact registers and immediate values as defined by the syntax.

## Operand Notation
* `$rd`: Destination register
* `$rs`: First source register
* `$rt`: Second source register (or destination for I-Type)
* `imm`: 16-bit immediate value (integer)
* `offset`: 16-bit address offset (integer)
* `target`: Code label (e.g., `loop`)
* `sa`: 5-bit shift amount (0-31)
* `pos`, `size`: Bitfield position and size (0-31)

---

## 1. Arithmetic Instructions
> Note: Ember implements MIPS32 Release 6. The `HI`/`LO` registers are removed. Multiplication and division instructions output directly to general-purpose registers.

| Mnemonic | Syntax | Operation | Description |
| :--- | :--- | :--- | :--- |
| **ADD** | `ADD $rd, $rs, $rt` | `$rd = $rs + $rt` | Add (Signed). Throws overflow exception. |
| **ADDU** | `ADDU $rd, $rs, $rt`| `$rd = $rs + $rt` | Add (Unsigned). No overflow exception. |
| **ADDI** | `ADDI $rt, $rs, imm`| `$rt = $rs + imm` | Add Immediate (Signed). `imm` is sign-extended. |
| **SUB** | `SUB $rd, $rs, $rt` | `$rd = $rs - $rt` | Subtract (Signed). Throws overflow exception. |
| **SUBU** | `SUBU $rd, $rs, $rt`| `$rd = $rs - $rt` | Subtract (Unsigned). No overflow exception. |
| **MUL** | `MUL $rd, $rs, $rt` | `$rd = ($rs * $rt)[31:0]` | Multiply. Stores lower 32 bits of signed product. |
| **MUH** | `MUH $rd, $rs, $rt` | `$rd = ($rs * $rt)[63:32]`| Multiply High. Stores upper 32 bits of signed product. |
| **MULU**| `MULU $rd, $rs, $rt`| `$rd = ($rs * $rt)[31:0]` | Multiply Unsigned (Lower 32 bits). |
| **MUHU**| `MUHU $rd, $rs, $rt`| `$rd = ($rs * $rt)[63:32]`| Multiply High Unsigned (Upper 32 bits). |
| **DIV** | `DIV $rd, $rs, $rt` | `$rd = $rs / $rt` | Divide (Signed). Stores quotient. |
| **MOD** | `MOD $rd, $rs, $rt` | `$rd = $rs % $rt` | Modulo (Signed). Stores remainder. |
| **DIVU**| `DIVU $rd, $rs, $rt`| `$rd = $rs / $rt` | Divide (Unsigned). Stores quotient. |
| **MODU**| `MODU $rd, $rs, $rt`| `$rd = $rs % $rt` | Modulo (Unsigned). Stores remainder. |

---

## 2. Logical Instructions

| Mnemonic | Syntax | Operation | Description |
| :--- | :--- | :--- | :--- |
| **AND** | `AND $rd, $rs, $rt` | `$rd = $rs & $rt` | Bitwise AND. |
| **OR** | `OR $rd, $rs, $rt` | `$rd = $rs \| $rt` | Bitwise OR. |
| **XOR** | `XOR $rd, $rs, $rt` | `$rd = $rs ^ $rt` | Bitwise Exclusive OR. |
| **NOR** | `NOR $rd, $rs, $rt` | `$rd = ~($rs \| $rt)`| Bitwise NOR. |
| **NAND**| `NAND $rd, $rs, $rt`| `$rd = ~($rs & $rt)`| Bitwise NAND. |
| **ANDI**| `ANDI $rt, $rs, imm`| `$rt = $rs & imm` | Bitwise AND Immediate. `imm` is zero-extended. |
| **ORI** | `ORI $rt, $rs, imm` | `$rt = $rs \| imm` | Bitwise OR Immediate. `imm` is zero-extended. |
| **XORI**| `XORI $rt, $rs, imm`| `$rt = $rs ^ imm` | Bitwise XOR Immediate. `imm` is zero-extended. |

---

## 3. Shift and Rotate Instructions

| Mnemonic | Syntax | Operation | Description |
| :--- | :--- | :--- | :--- |
| **SLL** | `SLL $rd, $rt, sa` | `$rd = $rt << sa` | Shift Left Logical by constant `sa`. |
| **SRL** | `SRL $rd, $rt, sa` | `$rd = $rt >> sa` | Shift Right Logical (Zero-fill) by constant `sa`. |
| **SRA** | `SRA $rd, $rt, sa` | `$rd = $rt >> sa` | Shift Right Arithmetic (Sign-extend) by constant. |
| **SLLV**| `SLLV $rd, $rt, $rs`| `$rd = $rt << $rs` | Shift Left Logical by variable amount in `$rs`. |
| **SRLV**| `SRLV $rd, $rt, $rs`| `$rd = $rt >> $rs` | Shift Right Logical by variable amount in `$rs`. |
| **SRAV**| `SRAV $rd, $rt, $rs`| `$rd = $rt >> $rs` | Shift Right Arithmetic by variable amount in `$rs`. |
| **ROTL**| `ROTL $rd, $rt, sa` | `$rd = rotl($rt, sa)`| Rotate Left by constant `sa`. |
| **ROTR**| `ROTR $rd, $rt, sa` | `$rd = rotr($rt, sa)`| Rotate Right by constant `sa`. |
| **ROTLV**| `ROTLV $rd, $rt, $rs`| `$rd = rotl($rt, $rs)`| Rotate Left by variable amount in `$rs`. |
| **ROTRV**| `ROTRV $rd, $rt, $rs`| `$rd = rotr($rt, $rs)`| Rotate Right by variable amount in `$rs`. |

---

## 4. Comparison Instructions
*Note: Ember includes extended comparison instructions as native operations for completeness.*

| Mnemonic | Syntax | Description |
| :--- | :--- | :--- |
| **SLT / SLTU** | `SLT $rd, $rs, $rt` | Set on Less Than (Signed / Unsigned). `$rd = ($rs < $rt) ? 1 : 0` |
| **SGT / SGTU** | `SGT $rd, $rs, $rt` | Set on Greater Than. `$rd = ($rs > $rt) ? 1 : 0` |
| **SLE / SLEU** | `SLE $rd, $rs, $rt` | Set on Less or Equal. `$rd = ($rs <= $rt) ? 1 : 0` |
| **SGE / SGEU** | `SGE $rd, $rs, $rt` | Set on Greater or Equal. `$rd = ($rs >= $rt) ? 1 : 0` |
| **SEQ / SNE** | `SEQ $rd, $rs, $rt` | Set on Equal (`SEQ`) / Set on Not Equal (`SNE`). |

**Immediate Variants (I-Type):**
Append `I` (or `IU` for unsigned) to any of the above to compare `$rs` against a 16-bit immediate value, storing the boolean result in `$rt`. 
*Example:* `SLTI $rt, $rs, imm`

---

## 5. Bit Manipulation & Conditional Select

| Mnemonic | Syntax | Description |
| :--- | :--- | :--- |
| **CLO** | `CLO $rd, $rs` | Count Leading Ones in `$rs`, store count in `$rd`. |
| **CLZ** | `CLZ $rd, $rs` | Count Leading Zeros in `$rs`, store count in `$rd`. |
| **EXT** | `EXT $rt, $rs, pos, size`| Extract bitfield from `$rs` starting at `pos` with length `size`, store zero-extended in `$rt`. |
| **INS** | `INS $rt, $rs, pos, size`| Insert lower `size` bits of `$rs` into `$rt` at position `pos`. |
| **SEB** | `SEB $rd, $rt` | Sign-Extend Byte. Takes 8 LSBs of `$rt`, sign-extends to 32 bits into `$rd`. |
| **SEH** | `SEH $rd, $rt` | Sign-Extend Halfword. Takes 16 LSBs of `$rt`, sign-extends to 32 bits into `$rd`. |
| **SELEQZ**| `SELEQZ $rd, $rs, $rt` | Select if Zero: `$rd = ($rt == 0) ? $rs : 0`. |
| **SELNEZ**| `SELNEZ $rd, $rs, $rt` | Select if Not Zero: `$rd = ($rt != 0) ? $rs : 0`. |

---

## 6. Address Generation & Memory Access

| Mnemonic | Syntax | Description |
| :--- | :--- | :--- |
| **LUI** | `LUI $rt, imm` | Load Upper Immediate. Shifts `imm` left by 16 bits, stores in `$rt`. Lower 16 bits are 0. |
| **LSA** | `LSA $rd, $rs, $rt, sa`| Load Scaled Address. `$rd = ($rs << sa) + $rt`. (`sa` must be 1-4). |
| **LW / SW**| `LW $rt, offset($rs)` | Load / Store Word (32-bit). Address must be 4-byte aligned. |
| **LH / SH**| `LH $rt, offset($rs)` | Load / Store Halfword (16-bit). Address must be 2-byte aligned. `LH` sign-extends. |
| **LHU** | `LHU $rt, offset($rs)` | Load Halfword Unsigned (16-bit). Zero-extends. |
| **LB / SB**| `LB $rt, offset($rs)` | Load / Store Byte (8-bit). `LB` sign-extends. |
| **LBU** | `LBU $rt, offset($rs)` | Load Byte Unsigned. Zero-extends. |
| **LL / SC**| `LL $rt, offset($rs)` | Load Linked / Store Conditional. (Used for atomic operations). |

---

## 7. Control Flow (Jumps & Branches)
*Note: Ember implements Release 6 branches, which resolve immediately. There are NO branch delay slots in Ember.*

| Mnemonic | Syntax | Description |
| :--- | :--- | :--- |
| **J** | `J target` | Jump unconditionally to `target` label. |
| **JR** | `JR $rs` | Jump Register. Jump unconditionally to address stored in `$rs`. |
| **JAL** | `JAL target` | Jump and Link. Jump to `target`, store return address (PC+4) in `$ra`. |
| **JALR**| `JALR $rd, $rs`| Jump and Link Register. Jump to `$rs`, store return address in `$rd` (defaults to `$ra`). |
| **B** | `B target` | Branch unconditionally to `target`. |
| **BAL** | `BAL target` | Branch and Link. Branch to `target`, store return address in `$ra`. |
| **BEQ** | `BEQ $rs, $rt, target`| Branch if Equal (`$rs == $rt`). |
| **BNE** | `BNE $rs, $rt, target`| Branch if Not Equal (`$rs != $rt`). |
| **BGEZ**| `BGEZ $rs, target` | Branch if Greater Than or Equal to Zero (`$rs >= 0`). |
| **BLEZ**| `BLEZ $rs, target` | Branch if Less Than or Equal to Zero (`$rs <= 0`). |
| **BGTZ**| `BGTZ $rs, target` | Branch if Greater Than Zero (`$rs > 0`). |
| **BLTZ**| `BLTZ $rs, target` | Branch if Less Than Zero (`$rs < 0`). |

---

## 8. System & Exception Instructions

| Mnemonic | Syntax | Description |
| :--- | :--- | :--- |
| **NOP** | `NOP` | No Operation. Incurs a 1-cycle delay. Physically encoded as `SLL $zero, $zero, 0`. |
| **SYSCALL**| `SYSCALL` | Triggers a system environment call based on the value in `$v0`. |
| **BREAK**| `BREAK` | Triggers a breakpoint exception (used for debugging execution halts). |
| **TEQ** | `TEQ $rs, $rt` | Trap if Equal. Halts execution with an exception if `$rs == $rt`. |
| **TNE** | `TNE $rs, $rt` | Trap if Not Equal. Halts execution with an exception if `$rs != $rt`. |