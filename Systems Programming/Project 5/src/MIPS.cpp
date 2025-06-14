#include "MIPS.hpp"
#include <iostream>
#include <sstream>
#include <iomanip>
#include <algorithm>

MIPS::MIPS() : registers(32, 0), pc(0x400000), hi(0), lo(0), 
               memory(0x100000, 0), stepMode(false), pipelineEnabled(true),
               cycleCount(0), instructionCount(0) {
    // Initialize $sp (stack pointer) to top of memory
    registers[29] = 0x7fffeffc;
    // Initialize $gp (global pointer)
    registers[28] = 0x10008000;
}

MIPS::~MIPS() {}

void MIPS::loadProgram(const std::vector<std::string>& assembly) {
    instructions = InstructionDecoder::assembleProgram(assembly);

    // Load instructions into memory at 0x400000
    uint32_t addr = 0x400000;
    for (const auto& instr : instructions) {
        setMemoryWord(addr, instr.machineCode);
        addr += 4;
    }

    pc = 0x400000;
    cycleCount = 0;
    instructionCount = 0;
}

void MIPS::reset() {
    std::fill(registers.begin(), registers.end(), 0);
    std::fill(memory.begin(), memory.end(), 0);

    pc = 0x400000;
    hi = lo = 0;
    cycleCount = 0;
    instructionCount = 0;

    // Reinitialize special registers
    registers[29] = 0x7fffeffc; // $sp
    registers[28] = 0x10008000; // $gp

    pipeline = Pipeline();
}

void MIPS::step() {
    if (pc >= 0x400000 + instructions.size() * 4) {
        return; // Program ended
    }

    // Fetch instruction
    uint32_t instrAddr = pc;
    uint32_t machineCode = getMemoryWord(instrAddr);
    Instruction instr = InstructionDecoder::decode(machineCode, instrAddr);

    if (pipelineEnabled) {
        pipeline.tick(instr, pc, registers, memory);
    } else {
        executeSingleCycle(instr);
    }

    cycleCount++;
    if (instr.format != InstructionFormat::NOP) {
        instructionCount++;
    }
}

void MIPS::run() {
    while (pc < 0x400000 + instructions.size() * 4) {
        step();
        if (stepMode) break;
    }
}

void MIPS::executeSingleCycle(const Instruction& instr) {
    uint32_t nextPC = pc + 4;

    switch (instr.format) {
        case InstructionFormat::ADD:
            registers[instr.rd] = registers[instr.rs] + registers[instr.rt];
            break;
        case InstructionFormat::SUB:
            registers[instr.rd] = registers[instr.rs] - registers[instr.rt];
            break;
        case InstructionFormat::AND:
            registers[instr.rd] = registers[instr.rs] & registers[instr.rt];
            break;
        case InstructionFormat::OR:
            registers[instr.rd] = registers[instr.rs] | registers[instr.rt];
            break;
        case InstructionFormat::NOR:
            registers[instr.rd] = ~(registers[instr.rs] | registers[instr.rt]);
            break;
        case InstructionFormat::SLT:
            registers[instr.rd] = ((int32_t)registers[instr.rs] < (int32_t)registers[instr.rt]) ? 1 : 0;
            break;
        case InstructionFormat::ADDI:
            registers[instr.rt] = registers[instr.rs] + instr.signedImmediate;
            break;
        case InstructionFormat::ANDI:
            registers[instr.rt] = registers[instr.rs] & instr.immediate;
            break;
        case InstructionFormat::ORI:
            registers[instr.rt] = registers[instr.rs] | instr.immediate;
            break;
        case InstructionFormat::LW:
            registers[instr.rt] = getMemoryWord(registers[instr.rs] + instr.signedImmediate);
            break;
        case InstructionFormat::SW:
            setMemoryWord(registers[instr.rs] + instr.signedImmediate, registers[instr.rt]);
            break;
        case InstructionFormat::BEQ:
            if (registers[instr.rs] == registers[instr.rt]) {
                nextPC = pc + 4 + (instr.signedImmediate << 2);
            }
            break;
        case InstructionFormat::BNE:
            if (registers[instr.rs] != registers[instr.rt]) {
                nextPC = pc + 4 + (instr.signedImmediate << 2);
            }
            break;
        case InstructionFormat::J:
            nextPC = (pc & 0xF0000000) | (instr.jumpTarget << 2);
            break;
        case InstructionFormat::JAL:
            registers[31] = pc + 8; // Save return address
            nextPC = (pc & 0xF0000000) | (instr.jumpTarget << 2);
            break;
        case InstructionFormat::JR:
            nextPC = registers[instr.rs];
            break;
        case InstructionFormat::NOP:
            break;
        default:
            std::cerr << "Unsupported instruction: " << instr.toString() << std::endl;
            break;
    }

    // Ensure $zero remains zero
    registers[0] = 0;
    pc = nextPC;
}

uint32_t MIPS::getRegister(int reg) const {
    if (reg >= 0 && reg < 32) {
        return registers[reg];
    }
    return 0;
}

void MIPS::setRegister(int reg, uint32_t value) {
    if (reg > 0 && reg < 32) { // $zero is read-only
        registers[reg] = value;
    }
}

uint32_t MIPS::getPC() const {
    return pc;
}

void MIPS::setPC(uint32_t value) {
    pc = value;
}

uint32_t MIPS::getMemoryWord(uint32_t address) const {
    if (address + 3 < memory.size()) {
        // Big-endian
        return (memory[address] << 24) | 
               (memory[address + 1] << 16) |
               (memory[address + 2] << 8) |
               memory[address + 3];
    }
    return 0;
}

void MIPS::setMemoryWord(uint32_t address, uint32_t value) {
    if (address + 3 < memory.size()) {
        // Big-endian
        memory[address] = (value >> 24) & 0xFF;
        memory[address + 1] = (value >> 16) & 0xFF;
        memory[address + 2] = (value >> 8) & 0xFF;
        memory[address + 3] = value & 0xFF;
    }
}

uint8_t MIPS::getMemoryByte(uint32_t address) const {
    if (address < memory.size()) {
        return memory[address];
    }
    return 0;
}

void MIPS::setMemoryByte(uint32_t address, uint8_t value) {
    if (address < memory.size()) {
        memory[address] = value;
    }
}

void MIPS::enablePipeline(bool enable) {
    pipelineEnabled = enable;
}

void MIPS::setStepMode(bool enable) {
    stepMode = enable;
}

std::string MIPS::getRegisterState() const {
    std::stringstream ss;
    const char* regNames[] = {
        "$zero", "$at", "$v0", "$v1", "$a0", "$a1", "$a2", "$a3",
        "$t0", "$t1", "$t2", "$t3", "$t4", "$t5", "$t6", "$t7",
        "$s0", "$s1", "$s2", "$s3", "$s4", "$s5", "$s6", "$s7",
        "$t8", "$t9", "$k0", "$k1", "$gp", "$sp", "$fp", "$ra"
    };

    for (int i = 0; i < 32; i++) {
        ss << regNames[i] << ": 0x" << std::hex << std::setw(8) << std::setfill('0') 
           << registers[i] << " (" << std::dec << registers[i] << ")\n";
    }
    ss << "PC: 0x" << std::hex << std::setw(8) << std::setfill('0') << pc << "\n";
    ss << "HI: 0x" << std::hex << std::setw(8) << std::setfill('0') << hi << "\n";
    ss << "LO: 0x" << std::hex << std::setw(8) << std::setfill('0') << lo << "\n";

    return ss.str();
}

std::string MIPS::getMemoryState(uint32_t start, uint32_t length) const {
    std::stringstream ss;
    for (uint32_t i = 0; i < length; i += 4) {
        uint32_t addr = start + i;
        uint32_t word = getMemoryWord(addr);
        ss << "0x" << std::hex << std::setw(8) << std::setfill('0') << addr 
           << ": 0x" << std::setw(8) << std::setfill('0') << word << "\n";
    }
    return ss.str();
}

std::string MIPS::getPipelineState() const {
    if (pipelineEnabled) {
        return pipeline.getState();
    }
    return "Pipeline disabled";
}

bool MIPS::hasPipelineHazard() const {
    return pipelineEnabled && pipeline.isStalled();
}

void MIPS::flushPipeline() {
    if (pipelineEnabled) {
        pipeline.flushPipeline();
    }
}