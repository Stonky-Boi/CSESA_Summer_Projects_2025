#pragma once

#include <string>
#include <cstdint>

enum class InstructionFormat
{
    R_TYPE,
    I_TYPE,
    J_TYPE,
    UNKNOWN_FORMAT
};

enum class OperationType
{
    ADD, ADDI, ADDU, SUB, SUBU,
    MUL, MUH, MULU, MUHU,
    DIV, DIVU, MOD, MODU,
    AND, ANDI, OR, ORI, XOR, XORI, NOR, NAND,
    SLL, SRL, SRA, SLLV, SRLV, SRAV,
    ROTL, ROTR, ROTLV, ROTRV,
    SLT, SGT, SLE, SGE, SEQ, SNE,
    SLTI, SGTI, SLEI, SGEI, SEQI, SNEI,
    SLTU, SGTU, SLEU, SGEU,
    SLTIU, SGTIU, SLEIU, SGEIU,
    CLO, CLZ, EXT, INS, SEB, SEH,
    SELEQZ, SELNEZ,
    LUI, LSA,
    LB, LBU, LH, LHU, LW, LL,
    SB, SH, SW, SC,
    J, JR, JAL, JALR,
    B, BAL, BEQ, BNE, BGEZ, BLEZ, BGTZ, BLTZ,
    NOP, BREAK, SYSCALL, TEQ, TNE,
    UNKNOWN_OPERATION
};

class Instruction
{
private:
    InstructionFormat format;
    OperationType operation;
    int register_source;
    int register_target;
    int register_destination;
    uint32_t shift_amount;
    int32_t immediate_value;
    uint32_t jump_address;
    std::string assembly_string;

public:
    Instruction();

    void set_format(InstructionFormat new_format);
    InstructionFormat get_format() const;

    void set_operation(OperationType new_operation);
    OperationType get_operation() const;

    void set_register_source(int register_index);
    int get_register_source() const;

    void set_register_target(int register_index);
    int get_register_target() const;

    void set_register_destination(int register_index);
    int get_register_destination() const;

    void set_shift_amount(uint32_t amount);
    uint32_t get_shift_amount() const;

    void set_immediate_value(int32_t value);
    int32_t get_immediate_value() const;

    void set_jump_address(uint32_t address);
    uint32_t get_jump_address() const;

    void set_assembly_string(const std::string &string_value);
    std::string get_assembly_string() const;
};