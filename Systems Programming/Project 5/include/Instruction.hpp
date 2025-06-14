#ifndef INSTRUCTION_HPP
#define INSTRUCTION_HPP

#include <cstdint>
#include <string>
#include <vector>

enum class InstructionType {
    R_TYPE,
    I_TYPE, 
    J_TYPE
};

enum class InstructionFormat {
    // R-type instructions
    ADD, SUB, AND, OR, NOR, SLT, SLL, SRL, SRA, SLLV, SRLV, SRAV,
    JR, JALR, MFHI, MFLO, MTHI, MTLO, MULT, MULTU, DIV, DIVU,

    // I-type instructions
    ADDI, ADDIU, ANDI, ORI, XORI, SLTI, SLTIU,
    LW, LH, LB, LBU, LHU, SW, SH, SB,
    BEQ, BNE, BLEZ, BGTZ, BLTZ, BGEZ,
    LUI,

    // J-type instructions
    J, JAL,

    // Special
    NOP,
    UNKNOWN
};

struct Instruction {
    uint32_t machineCode;
    InstructionType type;
    InstructionFormat format;

    // R-type fields
    uint8_t rs, rt, rd, shamt, funct;

    // I-type fields
    uint16_t immediate;
    int16_t signedImmediate;

    // J-type fields
    uint32_t jumpTarget;

    // Common fields
    uint8_t opcode;

    // Metadata
    std::string assembly;
    uint32_t address;

    Instruction();
    Instruction(uint32_t machineCode, uint32_t address);

    void decode();
    std::string toString() const;
    bool isBranch() const;
    bool isJump() const;
    bool isLoad() const;
    bool isStore() const;
    bool usesRS() const;
    bool usesRT() const;
    bool writesRD() const;
    bool writesRT() const;
};

class InstructionDecoder {
public:
    static Instruction decode(uint32_t machineCode, uint32_t address);
    static std::vector<Instruction> assembleProgram(const std::vector<std::string>& assembly);

private:
    static InstructionFormat getInstructionFormat(uint8_t opcode, uint8_t funct);
    static std::string formatToString(InstructionFormat format);
};

#endif // INSTRUCTION_HPP