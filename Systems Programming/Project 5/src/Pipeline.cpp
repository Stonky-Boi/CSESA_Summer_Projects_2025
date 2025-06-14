#include "Pipeline.hpp"
#include <sstream>
#include <iomanip>

Pipeline::Pipeline() : stall(false), flush(false), stallCycles(0), flushCycles(0) {}

void Pipeline::tick(const Instruction& fetchedInstruction, uint32_t pc,
                   std::vector<uint32_t>& registers, std::vector<uint8_t>& memory) {

    // Check for hazards
    HazardType hazard = hazardUnit.detectHazard(fetchedInstruction, ID_EX.instruction, 
                                               EX_MEM.instruction, MEM_WB.instruction);

    if (hazard == HazardType::DATA_HAZARD_RAW) {
        if (hazardUnit.shouldStall(fetchedInstruction, ID_EX.instruction, EX_MEM.instruction)) {
            stallPipeline();
            stallCycles++;
            return;
        }
    }

    if (hazard == HazardType::CONTROL_HAZARD) {
        if (hazardUnit.shouldFlush(ID_EX.instruction)) {
            flushPipeline();
            flushCycles++;
        }
    }

    // Execute pipeline stages in reverse order
    executeWB(registers);
    executeMEM(memory);
    executeEX();
    executeID(registers);
    executeIF(fetchedInstruction, pc);

    stall = false;
    flush = false;
}

void Pipeline::executeIF(const Instruction& instruction, uint32_t pc) {
    if (!stall) {
        IF_ID.valid = true;
        IF_ID.instruction = instruction;
        IF_ID.pc = pc;
    }
}

void Pipeline::executeID(std::vector<uint32_t>& registers) {
    if (flush) {
        ID_EX.valid = false;
        return;
    }

    if (IF_ID.valid) {
        ID_EX = IF_ID;
        ID_EX.valid = true;

        // Decode and read registers
        const Instruction& instr = ID_EX.instruction;

        // Set control signals
        ID_EX.regWrite = instr.writesRD() || instr.writesRT();
        ID_EX.memRead = instr.isLoad();
        ID_EX.memWrite = instr.isStore();
        ID_EX.branch = instr.isBranch();
        ID_EX.jump = instr.isJump();

        // Determine write register
        if (instr.writesRD()) {
            ID_EX.writeReg = instr.rd;
        } else if (instr.writesRT()) {
            ID_EX.writeReg = instr.rt;
        }

        IF_ID.valid = false;
    }
}

void Pipeline::executeEX() {
    if (ID_EX.valid) {
        EX_MEM = ID_EX;
        EX_MEM.valid = true;

        const Instruction& instr = EX_MEM.instruction;

        // Perform ALU operation
        uint32_t op1 = registers[instr.rs];
        uint32_t op2;

        if (instr.type == InstructionType::I_TYPE) {
            op2 = instr.signedImmediate;
        } else {
            op2 = registers[instr.rt];
        }

        // Apply forwarding if needed
        op1 = hazardUnit.getForwardedValue(instr.rs, op1);
        if (instr.type == InstructionType::R_TYPE) {
            op2 = hazardUnit.getForwardedValue(instr.rt, op2);
        }

        EX_MEM.aluResult = performALU(op1, op2, instr.format);

        // Branch calculation
        if (instr.isBranch()) {
            bool taken = false;
            switch (instr.format) {
                case InstructionFormat::BEQ:
                    taken = (op1 == op2);
                    break;
                case InstructionFormat::BNE:
                    taken = (op1 != op2);
                    break;
                default:
                    break;
            }

            if (taken) {
                // Branch taken - need to flush IF and ID stages
                flush = true;
            }
        }

        ID_EX.valid = false;
    }
}

void Pipeline::executeMEM(std::vector<uint8_t>& memory) {
    if (EX_MEM.valid) {
        MEM_WB = EX_MEM;
        MEM_WB.valid = true;

        const Instruction& instr = MEM_WB.instruction;

        if (instr.isLoad()) {
            // Load from memory
            uint32_t address = EX_MEM.aluResult;
            MEM_WB.memoryData = getMemoryWord(memory, address);
            MEM_WB.writeData = MEM_WB.memoryData;
        } else if (instr.isStore()) {
            // Store to memory
            uint32_t address = EX_MEM.aluResult;
            uint32_t data = registers[instr.rt];
            setMemoryWord(memory, address, data);
        } else {
            // ALU result
            MEM_WB.writeData = EX_MEM.aluResult;
        }

        EX_MEM.valid = false;
    }
}

void Pipeline::executeWB(std::vector<uint32_t>& registers) {
    if (MEM_WB.valid && MEM_WB.regWrite) {
        if (MEM_WB.writeReg != 0) { // Don't write to $zero
            registers[MEM_WB.writeReg] = MEM_WB.writeData;

            // Setup forwarding for next instructions
            hazardUnit.setupForwarding(MEM_WB.writeReg, 4, MEM_WB.writeData);
        }

        MEM_WB.valid = false;
    }
}

uint32_t Pipeline::performALU(uint32_t op1, uint32_t op2, InstructionFormat format) {
    switch (format) {
        case InstructionFormat::ADD:
        case InstructionFormat::ADDI:
        case InstructionFormat::LW:
        case InstructionFormat::SW:
            return op1 + op2;
        case InstructionFormat::SUB:
            return op1 - op2;
        case InstructionFormat::AND:
        case InstructionFormat::ANDI:
            return op1 & op2;
        case InstructionFormat::OR:
        case InstructionFormat::ORI:
            return op1 | op2;
        case InstructionFormat::NOR:
            return ~(op1 | op2);
        case InstructionFormat::SLT:
        case InstructionFormat::SLTI:
            return ((int32_t)op1 < (int32_t)op2) ? 1 : 0;
        default:
            return 0;
    }
}

void Pipeline::stallPipeline() {
    stall = true;
}

void Pipeline::flushPipeline() {
    IF_ID.valid = false;
    ID_EX.valid = false;
    flush = true;
}

uint32_t Pipeline::getForwardedValue(uint8_t reg, uint32_t originalValue) {
    return hazardUnit.getForwardedValue(reg, originalValue);
}

std::string Pipeline::getState() const {
    std::stringstream ss;

    ss << "Pipeline State:\n";
    ss << "IF/ID: " << (IF_ID.valid ? IF_ID.instruction.toString() : "empty") << "\n";
    ss << "ID/EX: " << (ID_EX.valid ? ID_EX.instruction.toString() : "empty") << "\n";
    ss << "EX/MEM: " << (EX_MEM.valid ? EX_MEM.instruction.toString() : "empty") << "\n";
    ss << "MEM/WB: " << (MEM_WB.valid ? MEM_WB.instruction.toString() : "empty") << "\n";

    ss << "\nPipeline Statistics:\n";
    ss << "Stall Cycles: " << stallCycles << "\n";
    ss << "Flush Cycles: " << flushCycles << "\n";

    return ss.str();
}

PipelineStage Pipeline::getStage(int stage) const {
    switch (stage) {
        case 1: return IF_ID;
        case 2: return ID_EX;
        case 3: return EX_MEM;
        case 4: return MEM_WB;
        default: return PipelineStage();
    }
}

uint32_t Pipeline::getMemoryWord(const std::vector<uint8_t>& memory, uint32_t address) const {
    if (address + 3 < memory.size()) {
        return (memory[address] << 24) | 
               (memory[address + 1] << 16) |
               (memory[address + 2] << 8) |
               memory[address + 3];
    }
    return 0;
}

void Pipeline::setMemoryWord(std::vector<uint8_t>& memory, uint32_t address, uint32_t value) {
    if (address + 3 < memory.size()) {
        memory[address] = (value >> 24) & 0xFF;
        memory[address + 1] = (value >> 16) & 0xFF;
        memory[address + 2] = (value >> 8) & 0xFF;
        memory[address + 3] = value & 0xFF;
    }
}