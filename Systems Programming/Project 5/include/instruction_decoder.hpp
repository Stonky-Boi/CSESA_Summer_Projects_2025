#pragma once
#include <cstdint>
#include <string>

// MIPS instruction opcodes and function codes
namespace MIPS {
    // R-type instructions (opcode = 0x00)
    const uint8_t OPCODE_RTYPE = 0x00;
    const uint8_t FUNCT_ADD = 0x20;
    const uint8_t FUNCT_ADDU = 0x21;
    const uint8_t FUNCT_SUB = 0x22;
    const uint8_t FUNCT_SUBU = 0x23;
    const uint8_t FUNCT_AND = 0x24;
    const uint8_t FUNCT_OR = 0x25;
    const uint8_t FUNCT_XOR = 0x26;
    const uint8_t FUNCT_NOR = 0x27;
    const uint8_t FUNCT_SLT = 0x28;
    const uint8_t FUNCT_SLTU = 0x29;
    const uint8_t FUNCT_SLL = 0x00;
    const uint8_t FUNCT_SRL = 0x02;
    const uint8_t FUNCT_SRA = 0x03;
    const uint8_t FUNCT_JR = 0x08;
    
    // I-type instructions
    const uint8_t OPCODE_ADDI = 0x08;
    const uint8_t OPCODE_ADDIU = 0x09;
    const uint8_t OPCODE_ANDI = 0x0C;
    const uint8_t OPCODE_ORI = 0x0D;
    const uint8_t OPCODE_XORI = 0x0E;
    const uint8_t OPCODE_LUI = 0x0F;
    const uint8_t OPCODE_LW = 0x23;
    const uint8_t OPCODE_SW = 0x2B;
    const uint8_t OPCODE_BEQ = 0x04;
    const uint8_t OPCODE_BNE = 0x05;
    const uint8_t OPCODE_SLTI = 0x0A;
    const uint8_t OPCODE_SLTIU = 0x0B;
    
    // J-type instructions
    const uint8_t OPCODE_J = 0x02;
    const uint8_t OPCODE_JAL = 0x03;
    
    // Register names
    const int REG_ZERO = 0;
    const int REG_AT = 1;
    const int REG_V0 = 2;
    const int REG_V1 = 3;
    const int REG_A0 = 4;
    const int REG_A1 = 5;
    const int REG_A2 = 6;
    const int REG_A3 = 7;
    const int REG_T0 = 8;
    const int REG_T1 = 9;
    const int REG_T2 = 10;
    const int REG_T3 = 11;
    const int REG_T4 = 12;
    const int REG_T5 = 13;
    const int REG_T6 = 14;
    const int REG_T7 = 15;
    const int REG_S0 = 16;
    const int REG_S1 = 17;
    const int REG_S2 = 18;
    const int REG_S3 = 19;
    const int REG_S4 = 20;
    const int REG_S5 = 21;
    const int REG_S6 = 22;
    const int REG_S7 = 23;
    const int REG_T8 = 24;
    const int REG_T9 = 25;
    const int REG_K0 = 26;
    const int REG_K1 = 27;
    const int REG_GP = 28;
    const int REG_SP = 29;
    const int REG_FP = 30;
    const int REG_RA = 31;
}

class InstructionDecoder {
public:
    static std::string getInstructionName(uint32_t instruction);
    static std::string disassemble(uint32_t instruction);
    static std::string getRegisterName(int reg);
    static bool isRType(uint8_t opcode);
    static bool isIType(uint8_t opcode);
    static bool isJType(uint8_t opcode);
};
