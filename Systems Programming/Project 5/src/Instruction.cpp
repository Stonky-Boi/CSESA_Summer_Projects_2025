#include "Instruction.hpp"
#include <sstream>
#include <iostream>
#include <map>
#include <algorithm>
#include <regex>

Instruction::Instruction() : machineCode(0), type(InstructionType::R_TYPE), 
                           format(InstructionFormat::NOP), rs(0), rt(0), rd(0), 
                           shamt(0), funct(0), immediate(0), signedImmediate(0), 
                           jumpTarget(0), opcode(0), address(0) {}

Instruction::Instruction(uint32_t machineCode, uint32_t address) 
    : machineCode(machineCode), address(address) {
    decode();
}

void Instruction::decode() {
    opcode = (machineCode >> 26) & 0x3F;

    if (opcode == 0) {
        // R-type instruction
        type = InstructionType::R_TYPE;
        rs = (machineCode >> 21) & 0x1F;
        rt = (machineCode >> 16) & 0x1F;
        rd = (machineCode >> 11) & 0x1F;
        shamt = (machineCode >> 6) & 0x1F;
        funct = machineCode & 0x3F;

        format = InstructionDecoder::getInstructionFormat(opcode, funct);
    } else if (opcode == 2 || opcode == 3) {
        // J-type instruction
        type = InstructionType::J_TYPE;
        jumpTarget = machineCode & 0x3FFFFFF;

        format = (opcode == 2) ? InstructionFormat::J : InstructionFormat::JAL;
    } else {
        // I-type instruction
        type = InstructionType::I_TYPE;
        rs = (machineCode >> 21) & 0x1F;
        rt = (machineCode >> 16) & 0x1F;
        immediate = machineCode & 0xFFFF;
        signedImmediate = (int16_t)immediate;

        format = InstructionDecoder::getInstructionFormat(opcode, 0);
    }
}

std::string Instruction::toString() const {
    std::stringstream ss;

    switch (format) {
        case InstructionFormat::ADD:
            ss << "add $" << (int)rd << ", $" << (int)rs << ", $" << (int)rt;
            break;
        case InstructionFormat::SUB:
            ss << "sub $" << (int)rd << ", $" << (int)rs << ", $" << (int)rt;
            break;
        case InstructionFormat::AND:
            ss << "and $" << (int)rd << ", $" << (int)rs << ", $" << (int)rt;
            break;
        case InstructionFormat::OR:
            ss << "or $" << (int)rd << ", $" << (int)rs << ", $" << (int)rt;
            break;
        case InstructionFormat::ADDI:
            ss << "addi $" << (int)rt << ", $" << (int)rs << ", " << signedImmediate;
            break;
        case InstructionFormat::LW:
            ss << "lw $" << (int)rt << ", " << signedImmediate << "($" << (int)rs << ")";
            break;
        case InstructionFormat::SW:
            ss << "sw $" << (int)rt << ", " << signedImmediate << "($" << (int)rs << ")";
            break;
        case InstructionFormat::BEQ:
            ss << "beq $" << (int)rs << ", $" << (int)rt << ", " << signedImmediate;
            break;
        case InstructionFormat::BNE:
            ss << "bne $" << (int)rs << ", $" << (int)rt << ", " << signedImmediate;
            break;
        case InstructionFormat::J:
            ss << "j " << jumpTarget;
            break;
        case InstructionFormat::JAL:
            ss << "jal " << jumpTarget;
            break;
        case InstructionFormat::JR:
            ss << "jr $" << (int)rs;
            break;
        case InstructionFormat::NOP:
            ss << "nop";
            break;
        default:
            ss << "unknown";
            break;
    }

    return ss.str();
}

bool Instruction::isBranch() const {
    return format == InstructionFormat::BEQ || format == InstructionFormat::BNE ||
           format == InstructionFormat::BLEZ || format == InstructionFormat::BGTZ ||
           format == InstructionFormat::BLTZ || format == InstructionFormat::BGEZ;
}

bool Instruction::isJump() const {
    return format == InstructionFormat::J || format == InstructionFormat::JAL ||
           format == InstructionFormat::JR || format == InstructionFormat::JALR;
}

bool Instruction::isLoad() const {
    return format == InstructionFormat::LW || format == InstructionFormat::LH ||
           format == InstructionFormat::LB || format == InstructionFormat::LBU ||
           format == InstructionFormat::LHU;
}

bool Instruction::isStore() const {
    return format == InstructionFormat::SW || format == InstructionFormat::SH ||
           format == InstructionFormat::SB;
}

bool Instruction::usesRS() const {
    return type == InstructionType::I_TYPE || 
           (type == InstructionType::R_TYPE && format != InstructionFormat::NOP);
}

bool Instruction::usesRT() const {
    return (type == InstructionType::R_TYPE && format != InstructionFormat::JR) ||
           (type == InstructionType::I_TYPE && !isLoad() && format != InstructionFormat::LUI);
}

bool Instruction::writesRD() const {
    return type == InstructionType::R_TYPE && 
           format != InstructionFormat::JR && format != InstructionFormat::NOP;
}

bool Instruction::writesRT() const {
    return type == InstructionType::I_TYPE && 
           (isLoad() || format == InstructionFormat::ADDI || 
            format == InstructionFormat::ANDI || format == InstructionFormat::ORI ||
            format == InstructionFormat::LUI);
}

// InstructionDecoder implementation
Instruction InstructionDecoder::decode(uint32_t machineCode, uint32_t address) {
    return Instruction(machineCode, address);
}

InstructionFormat InstructionDecoder::getInstructionFormat(uint8_t opcode, uint8_t funct) {
    if (opcode == 0) {
        // R-type instructions based on function code
        switch (funct) {
            case 0x20: return InstructionFormat::ADD;
            case 0x22: return InstructionFormat::SUB;
            case 0x24: return InstructionFormat::AND;
            case 0x25: return InstructionFormat::OR;
            case 0x27: return InstructionFormat::NOR;
            case 0x2A: return InstructionFormat::SLT;
            case 0x00: return InstructionFormat::SLL;
            case 0x02: return InstructionFormat::SRL;
            case 0x03: return InstructionFormat::SRA;
            case 0x08: return InstructionFormat::JR;
            case 0x09: return InstructionFormat::JALR;
            default: return InstructionFormat::UNKNOWN;
        }
    } else {
        // I-type and J-type instructions based on opcode
        switch (opcode) {
            case 0x08: return InstructionFormat::ADDI;
            case 0x0C: return InstructionFormat::ANDI;
            case 0x0D: return InstructionFormat::ORI;
            case 0x0E: return InstructionFormat::XORI;
            case 0x0A: return InstructionFormat::SLTI;
            case 0x23: return InstructionFormat::LW;
            case 0x21: return InstructionFormat::LH;
            case 0x20: return InstructionFormat::LB;
            case 0x24: return InstructionFormat::LBU;
            case 0x25: return InstructionFormat::LHU;
            case 0x2B: return InstructionFormat::SW;
            case 0x29: return InstructionFormat::SH;
            case 0x28: return InstructionFormat::SB;
            case 0x04: return InstructionFormat::BEQ;
            case 0x05: return InstructionFormat::BNE;
            case 0x06: return InstructionFormat::BLEZ;
            case 0x07: return InstructionFormat::BGTZ;
            case 0x0F: return InstructionFormat::LUI;
            case 0x02: return InstructionFormat::J;
            case 0x03: return InstructionFormat::JAL;
            default: return InstructionFormat::UNKNOWN;
        }
    }
}

std::vector<Instruction> InstructionDecoder::assembleProgram(const std::vector<std::string>& assembly) {
    std::vector<Instruction> instructions;
    std::map<std::string, uint32_t> labels;
    uint32_t address = 0x400000;

    // First pass: collect labels
    for (const auto& line : assembly) {
        std::string trimmed = line;
        trimmed.erase(0, trimmed.find_first_not_of(" \t"));
        trimmed.erase(trimmed.find_last_not_of(" \t") + 1);

        if (trimmed.empty() || trimmed[0] == '#') continue;

        if (trimmed.back() == ':') {
            std::string label = trimmed.substr(0, trimmed.length() - 1);
            labels[label] = address;
        } else {
            address += 4;
        }
    }

    // Second pass: assemble instructions
    address = 0x400000;
    for (const auto& line : assembly) {
        std::string trimmed = line;
        trimmed.erase(0, trimmed.find_first_not_of(" \t"));
        trimmed.erase(trimmed.find_last_not_of(" \t") + 1);

        if (trimmed.empty() || trimmed[0] == '#' || trimmed.back() == ':') continue;

        uint32_t machineCode = assembleInstruction(trimmed, address, labels);
        instructions.push_back(Instruction(machineCode, address));
        address += 4;
    }

    return instructions;
}

uint32_t InstructionDecoder::assembleInstruction(const std::string& assembly, 
                                               uint32_t address,
                                               const std::map<std::string, uint32_t>& labels) {
    std::istringstream iss(assembly);
    std::string opcode;
    iss >> opcode;

    // Convert to lowercase
    std::transform(opcode.begin(), opcode.end(), opcode.begin(), ::tolower);

    uint32_t machineCode = 0;

    // Simple instruction assembly (basic subset)
    if (opcode == "add") {
        uint8_t rd, rs, rt;
        parseRTypeOperands(iss, rd, rs, rt);
        machineCode = (0 << 26) | (rs << 21) | (rt << 16) | (rd << 11) | (0 << 6) | 0x20;
    } else if (opcode == "sub") {
        uint8_t rd, rs, rt;
        parseRTypeOperands(iss, rd, rs, rt);
        machineCode = (0 << 26) | (rs << 21) | (rt << 16) | (rd << 11) | (0 << 6) | 0x22;
    } else if (opcode == "addi") {
        uint8_t rt, rs;
        int16_t imm;
        parseITypeOperands(iss, rt, rs, imm);
        machineCode = (8 << 26) | (rs << 21) | (rt << 16) | (imm & 0xFFFF);
    } else if (opcode == "lw") {
        uint8_t rt, rs;
        int16_t offset;
        parseLoadStoreOperands(iss, rt, offset, rs);
        machineCode = (0x23 << 26) | (rs << 21) | (rt << 16) | (offset & 0xFFFF);
    } else if (opcode == "sw") {
        uint8_t rt, rs;
        int16_t offset;
        parseLoadStoreOperands(iss, rt, offset, rs);
        machineCode = (0x2B << 26) | (rs << 21) | (rt << 16) | (offset & 0xFFFF);
    } else if (opcode == "beq") {
        uint8_t rs, rt;
        int16_t offset;
        parseBranchOperands(iss, rs, rt, offset, address, labels);
        machineCode = (4 << 26) | (rs << 21) | (rt << 16) | (offset & 0xFFFF);
    } else if (opcode == "j") {
        uint32_t target;
        parseJumpOperands(iss, target, labels);
        machineCode = (2 << 26) | (target & 0x3FFFFFF);
    } else if (opcode == "nop") {
        machineCode = 0;
    }

    return machineCode;
}

// Helper functions for parsing operands would be implemented here
void InstructionDecoder::parseRTypeOperands(std::istringstream& iss, 
                                          uint8_t& rd, uint8_t& rs, uint8_t& rt) {
    std::string token;
    // Parse $rd, $rs, $rt format
    // Implementation details omitted for brevity
}