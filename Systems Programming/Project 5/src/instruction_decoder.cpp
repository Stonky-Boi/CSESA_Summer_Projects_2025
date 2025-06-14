#include "instruction_decoder.hpp"
#include <sstream>
#include <iomanip>

std::string InstructionDecoder::getInstructionName(uint32_t instruction) {
    uint8_t opcode = (instruction >> 26) & 0x3F;
    uint8_t funct = instruction & 0x3F;
    
    if (opcode == 0) {
        switch (funct) {
            case MIPS::FUNCT_ADD: return "add";
            case MIPS::FUNCT_SUB: return "sub";
            case MIPS::FUNCT_AND: return "and";
            case MIPS::FUNCT_OR: return "or";
            case MIPS::FUNCT_SLT: return "slt";
            case MIPS::FUNCT_JR: return "jr";
            default: return "unknown";
        }
    } else {
        switch (opcode) {
            case MIPS::OPCODE_ADDI: return "addi";
            case MIPS::OPCODE_LW: return "lw";
            case MIPS::OPCODE_SW: return "sw";
            case MIPS::OPCODE_BEQ: return "beq";
            case MIPS::OPCODE_BNE: return "bne";
            case MIPS::OPCODE_J: return "j";
            case MIPS::OPCODE_JAL: return "jal";
            default: return "unknown";
        }
    }
}

std::string InstructionDecoder::disassemble(uint32_t instruction) {
    uint8_t opcode = (instruction >> 26) & 0x3F;
    uint8_t rs = (instruction >> 21) & 0x1F;
    uint8_t rt = (instruction >> 16) & 0x1F;
    uint8_t rd = (instruction >> 11) & 0x1F;
    uint16_t immediate = instruction & 0xFFFF;
    uint32_t jump_addr = instruction & 0x3FFFFFF;
    uint8_t funct = instruction & 0x3F;
    
    std::ostringstream oss;
    std::string name = getInstructionName(instruction);
    
    if (opcode == 0) { // R-type
        if (funct == MIPS::FUNCT_JR) {
            oss << name << " " << getRegisterName(rs);
        } else {
            oss << name << " " << getRegisterName(rd) << ", " 
                << getRegisterName(rs) << ", " << getRegisterName(rt);
        }
    } else if (opcode == MIPS::OPCODE_J || opcode == MIPS::OPCODE_JAL) { // J-type
        oss << name << " 0x" << std::hex << (jump_addr << 2);
    } else { // I-type
        if (opcode == MIPS::OPCODE_LW || opcode == MIPS::OPCODE_SW) {
            oss << name << " " << getRegisterName(rt) << ", " 
                << std::dec << (int16_t)immediate << "(" << getRegisterName(rs) << ")";
        } else if (opcode == MIPS::OPCODE_BEQ || opcode == MIPS::OPCODE_BNE) {
            oss << name << " " << getRegisterName(rs) << ", " << getRegisterName(rt) 
                << ", " << std::dec << (int16_t)immediate;
        } else {
            oss << name << " " << getRegisterName(rt) << ", " << getRegisterName(rs) 
                << ", " << std::dec << (int16_t)immediate;
        }
    }
    
    return oss.str();
}

std::string InstructionDecoder::getRegisterName(int reg) {
    const std::string reg_names[] = {
        "$zero", "$at", "$v0", "$v1", "$a0", "$a1", "$a2", "$a3",
        "$t0", "$t1", "$t2", "$t3", "$t4", "$t5", "$t6", "$t7",
        "$s0", "$s1", "$s2", "$s3", "$s4", "$s5", "$s6", "$s7",
        "$t8", "$t9", "$k0", "$k1", "$gp", "$sp", "$fp", "$ra"
    };
    
    if (reg >= 0 && reg < 32) {
        return reg_names[reg];
    }
    return "$unknown";
}

bool InstructionDecoder::isRType(uint8_t opcode) { return opcode == 0; }
bool InstructionDecoder::isIType(uint8_t opcode) { return opcode != 0 && opcode != 2 && opcode != 3; }
bool InstructionDecoder::isJType(uint8_t opcode) { return opcode == 2 || opcode == 3; }
