#pragma once
#include <cstdint>
#include <string>
namespace MIPS {
    const uint8_t OPC_R     = 0x00;
    const uint8_t OPC_ADDI  = 0x08;
    const uint8_t OPC_LW    = 0x23;
    const uint8_t OPC_SW    = 0x2B;
    const uint8_t OPC_BEQ   = 0x04;
    const uint8_t OPC_BNE   = 0x05;
    const uint8_t OPC_LUI   = 0x0F;
    const uint8_t OPC_J     = 0x02;
    const uint8_t OPC_JAL   = 0x03;
    const uint8_t FUNC_SYSC=0x0C;
    // extend as needed...
}
class InstructionDecoder {
public:
    struct Instr { uint32_t raw; uint8_t opcode,rs,rt,rd,shamt,funct; uint16_t imm; uint32_t addr; };
    static Instr decode(uint32_t raw);
    static std::string disassemble(uint32_t raw);
};