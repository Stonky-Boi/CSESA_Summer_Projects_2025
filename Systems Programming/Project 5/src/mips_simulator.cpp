#include "mips_simulator.hpp"
#include "instruction_decoder.hpp"
#include "alu.hpp"
#include "pipeline.hpp"
#include "branch_predictor.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>

MIPSSimulator::MIPSSimulator() 
    : registers(32, 0), memory(65536, 0), pc(0), halted(false), 
      step_mode(false), pipeline_enabled(false), branch_prediction_enabled(false),
      prediction_type("static") {
    initializePipeline();
    branch_stats = {0, 0, 0};
}

MIPSSimulator::~MIPSSimulator() {}

bool MIPSSimulator::loadProgram(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    std::string line;
    uint32_t address = 0;
    
    while (std::getline(file, line)) {
        if (line.empty() || line[0] == '#') continue;
        
        // Parse hex instruction
        uint32_t instruction = std::stoul(line, nullptr, 16);
        
        if (address + 3 < memory.size()) {
            memory[address] = (instruction >> 24) & 0xFF;
            memory[address + 1] = (instruction >> 16) & 0xFF;
            memory[address + 2] = (instruction >> 8) & 0xFF;
            memory[address + 3] = instruction & 0xFF;
            address += 4;
        }
    }
    
    file.close();
    reset();
    return true;
}

bool MIPSSimulator::loadProgramFromString(const std::string& program) {
    std::istringstream iss(program);
    std::string line;
    uint32_t address = 0;
    
    while (std::getline(iss, line)) {
        if (line.empty() || line[0] == '#') continue;
        
        try {
            uint32_t instruction = std::stoul(line, nullptr, 16);
            
            if (address + 3 < memory.size()) {
                memory[address] = (instruction >> 24) & 0xFF;
                memory[address + 1] = (instruction >> 16) & 0xFF;
                memory[address + 2] = (instruction >> 8) & 0xFF;
                memory[address + 3] = instruction & 0xFF;
                address += 4;
            }
        } catch (const std::exception& e) {
            return false;
        }
    }
    
    reset();
    return true;
}

void MIPSSimulator::reset() {
    std::fill(registers.begin(), registers.end(), 0);
    pc = 0;
    halted = false;
    if (pipeline_enabled) {
        initializePipeline();
    }
    branch_stats = {0, 0, 0};
}

bool MIPSSimulator::step() {
    if (halted) return false;
    
    if (pipeline_enabled) {
        advancePipeline();
    } else {
        // Fetch
        if (!isValidAddress(pc)) {
            halted = true;
            return false;
        }
        
        uint32_t instruction = (memory[pc] << 24) | (memory[pc + 1] << 16) | 
                              (memory[pc + 2] << 8) | memory[pc + 3];
        
        // Decode and Execute
        Instruction instr = decodeInstruction(instruction);
        if (!executeInstruction(instr)) {
            halted = true;
            return false;
        }
    }
    
    registers[0] = 0; // $zero always zero
    return !halted;
}

void MIPSSimulator::run() {
    while (!halted && step()) {
        if (step_mode) break;
    }
}

MIPSSimulator::Instruction MIPSSimulator::decodeInstruction(uint32_t instruction) {
    Instruction instr;
    instr.raw = instruction;
    instr.opcode = (instruction >> 26) & 0x3F;
    instr.rs = (instruction >> 21) & 0x1F;
    instr.rt = (instruction >> 16) & 0x1F;
    instr.rd = (instruction >> 11) & 0x1F;
    instr.shamt = (instruction >> 6) & 0x1F;
    instr.funct = instruction & 0x3F;
    instr.immediate = instruction & 0xFFFF;
    instr.jump_addr = instruction & 0x3FFFFFF;
    
    if (instr.opcode == 0) {
        instr.type = "R";
    } else if (instr.opcode == 2 || instr.opcode == 3) {
        instr.type = "J";
    } else {
        instr.type = "I";
    }
    
    return instr;
}

bool MIPSSimulator::executeInstruction(const Instruction& instr) {
    uint32_t next_pc = pc + 4;
    bool branch_taken = false;
    
    if (instr.type == "R") {
        ALU::Result result;
        
        switch (instr.funct) {
            case MIPS::FUNCT_ADD:
                result = ALU::execute(registers[instr.rs], registers[instr.rt], ALU::ADD);
                registers[instr.rd] = result.value;
                break;
            case MIPS::FUNCT_SUB:
                result = ALU::execute(registers[instr.rs], registers[instr.rt], ALU::SUB);
                registers[instr.rd] = result.value;
                break;
            case MIPS::FUNCT_AND:
                result = ALU::execute(registers[instr.rs], registers[instr.rt], ALU::AND);
                registers[instr.rd] = result.value;
                break;
            case MIPS::FUNCT_OR:
                result = ALU::execute(registers[instr.rs], registers[instr.rt], ALU::OR);
                registers[instr.rd] = result.value;
                break;
            case MIPS::FUNCT_SLT:
                result = ALU::execute(registers[instr.rs], registers[instr.rt], ALU::SLT);
                registers[instr.rd] = result.value;
                break;
            case MIPS::FUNCT_JR:
                next_pc = registers[instr.rs];
                break;
        }
    } else if (instr.type == "I") {
        uint32_t imm_extended = signExtend16(instr.immediate);
        
        switch (instr.opcode) {
            case MIPS::OPCODE_ADDI:
                registers[instr.rt] = registers[instr.rs] + imm_extended;
                break;
            case MIPS::OPCODE_LW:
                if (isValidAddress(registers[instr.rs] + imm_extended)) {
                    uint32_t addr = registers[instr.rs] + imm_extended;
                    registers[instr.rt] = (memory[addr] << 24) | (memory[addr + 1] << 16) |
                                         (memory[addr + 2] << 8) | memory[addr + 3];
                }
                break;
            case MIPS::OPCODE_SW:
                if (isValidAddress(registers[instr.rs] + imm_extended)) {
                    uint32_t addr = registers[instr.rs] + imm_extended;
                    memory[addr] = (registers[instr.rt] >> 24) & 0xFF;
                    memory[addr + 1] = (registers[instr.rt] >> 16) & 0xFF;
                    memory[addr + 2] = (registers[instr.rt] >> 8) & 0xFF;
                    memory[addr + 3] = registers[instr.rt] & 0xFF;
                }
                break;
            case MIPS::OPCODE_BEQ:
                if (registers[instr.rs] == registers[instr.rt]) {
                    next_pc = pc + 4 + (imm_extended << 2);
                    branch_taken = true;
                }
                if (branch_prediction_enabled) {
                    branch_stats.total_branches++;
                    bool predicted = predictBranch(pc);
                    if (predicted == branch_taken) {
                        branch_stats.correct_predictions++;
                    } else {
                        branch_stats.incorrect_predictions++;
                    }
                    updateBranchPredictor(pc, branch_taken);
                }
                break;
            case MIPS::OPCODE_BNE:
                if (registers[instr.rs] != registers[instr.rt]) {
                    next_pc = pc + 4 + (imm_extended << 2);
                    branch_taken = true;
                }
                if (branch_prediction_enabled) {
                    branch_stats.total_branches++;
                    bool predicted = predictBranch(pc);
                    if (predicted == branch_taken) {
                        branch_stats.correct_predictions++;
                    } else {
                        branch_stats.incorrect_predictions++;
                    }
                    updateBranchPredictor(pc, branch_taken);
                }
                break;
        }
    } else if (instr.type == "J") {
        switch (instr.opcode) {
            case MIPS::OPCODE_J:
                next_pc = (pc & 0xF0000000) | (instr.jump_addr << 2);
                break;
            case MIPS::OPCODE_JAL:
                registers[31] = pc + 8; // Return address
                next_pc = (pc & 0xF0000000) | (instr.jump_addr << 2);
                break;
        }
    }
    
    pc = next_pc;
    return true;
}

uint32_t MIPSSimulator::signExtend16(uint16_t value) {
    if (value & 0x8000) {
        return value | 0xFFFF0000;
    }
    return value;
}

bool MIPSSimulator::isValidAddress(uint32_t address) const {
    return address < memory.size() - 3;
}

void MIPSSimulator::initializePipeline() {
    pipeline_stages.resize(5);
    for (auto& stage : pipeline_stages) {
        stage.instruction = 0;
        stage.pc = 0;
        stage.valid = false;
        stage.stall = false;
        stage.flush = false;
    }
}

void MIPSSimulator::advancePipeline() {
    // Handle hazards
    if (detectHazards()) {
        handleHazards();
        return;
    }
    
    // Advance pipeline stages
    for (int i = 4; i >= 1; i--) {
        pipeline_stages[i] = pipeline_stages[i - 1];
    }
    
    // Fetch new instruction
    if (!halted && isValidAddress(pc)) {
        uint32_t instruction = (memory[pc] << 24) | (memory[pc + 1] << 16) | 
                              (memory[pc + 2] << 8) | memory[pc + 3];
        pipeline_stages[0].instruction = instruction;
        pipeline_stages[0].pc = pc;
        pipeline_stages[0].valid = true;
        pc += 4;
    } else {
        pipeline_stages[0].valid = false;
    }
}

bool MIPSSimulator::detectHazards() {
    // Simplified hazard detection
    if (!pipeline_stages[1].valid || !pipeline_stages[2].valid) {
        return false;
    }
    
    Instruction id_instr = decodeInstruction(pipeline_stages[1].instruction);
    Instruction ex_instr = decodeInstruction(pipeline_stages[2].instruction);
    
    // Data hazard: RAW (Read After Write)
    if (ex_instr.type == "R" || (ex_instr.type == "I" && ex_instr.opcode == MIPS::OPCODE_LW)) {
        uint8_t dest_reg = (ex_instr.type == "R") ? ex_instr.rd : ex_instr.rt;
        if (dest_reg != 0 && (dest_reg == id_instr.rs || dest_reg == id_instr.rt)) {
            return true;
        }
    }
    
    return false;
}

void MIPSSimulator::handleHazards() {
    // Insert pipeline stall
    for (int i = 1; i < 5; i++) {
        pipeline_stages[i].stall = true;
    }
}

// Getter and setter methods
uint32_t MIPSSimulator::getRegister(int reg) const {
    if (reg >= 0 && reg < 32) return registers[reg];
    return 0;
}

void MIPSSimulator::setRegister(int reg, uint32_t value) {
    if (reg >= 1 && reg < 32) registers[reg] = value;
}

uint32_t MIPSSimulator::getMemory(uint32_t address) const {
    if (isValidAddress(address)) {
        return (memory[address] << 24) | (memory[address + 1] << 16) |
               (memory[address + 2] << 8) | memory[address + 3];
    }
    return 0;
}

void MIPSSimulator::setMemory(uint32_t address, uint32_t value) {
    if (isValidAddress(address)) {
        memory[address] = (value >> 24) & 0xFF;
        memory[address + 1] = (value >> 16) & 0xFF;
        memory[address + 2] = (value >> 8) & 0xFF;
        memory[address + 3] = value & 0xFF;
    }
}

uint32_t MIPSSimulator::getPC() const { return pc; }
void MIPSSimulator::setPC(uint32_t new_pc) { pc = new_pc; }
bool MIPSSimulator::isHalted() const { return halted; }
void MIPSSimulator::setStepMode(bool mode) { step_mode = mode; }
bool MIPSSimulator::getStepMode() const { return step_mode; }

void MIPSSimulator::enablePipeline(bool enable) {
    pipeline_enabled = enable;
    if (enable) initializePipeline();
}

void MIPSSimulator::enableBranchPrediction(bool enable, const std::string& type) {
    branch_prediction_enabled = enable;
    prediction_type = type;
}

bool MIPSSimulator::predictBranch(uint32_t pc) {
    if (prediction_type == "static") {
        return false; // Static not-taken
    } else if (prediction_type == "dynamic") {
        return branch_history_table[pc];
    }
    return false;
}

void MIPSSimulator::updateBranchPredictor(uint32_t pc, bool taken) {
    if (prediction_type == "dynamic") {
        branch_history_table[pc] = taken;
    }
}

std::string MIPSSimulator::getStateString() const {
    std::ostringstream oss;
    oss << "PC: 0x" << std::hex << std::setw(8) << std::setfill('0') << pc << "\n";
    oss << "Registers:\n";
    for (int i = 0; i < 32; i += 4) {
        oss << "$" << std::setw(2) << std::dec << i << "-$" << (i+3) << ": ";
        for (int j = 0; j < 4; j++) {
            oss << "0x" << std::hex << std::setw(8) << std::setfill('0') << registers[i+j] << " ";
        }
        oss << "\n";
    }
    oss << "Halted: " << (halted ? "Yes" : "No") << "\n";
    return oss.str();
}

std::string MIPSSimulator::getPipelineStateString() const {
    std::ostringstream oss;
    oss << "Pipeline State:\n";
    const std::string stage_names[] = {"IF", "ID", "EX", "MEM", "WB"};
    for (int i = 0; i < 5; i++) {
        oss << stage_names[i] << ": ";
        if (pipeline_stages[i].valid) {
            oss << "0x" << std::hex << std::setw(8) << std::setfill('0') 
                << pipeline_stages[i].instruction;
        } else {
            oss << "NOP";
        }
        oss << "\n";
    }
    return oss.str();
}

std::string MIPSSimulator::getBranchPredictionStats() const {
    std::ostringstream oss;
    oss << "Branch Prediction Statistics:\n";
    oss << "Total Branches: " << branch_stats.total_branches << "\n";
    oss << "Correct Predictions: " << branch_stats.correct_predictions << "\n";
    oss << "Incorrect Predictions: " << branch_stats.incorrect_predictions << "\n";
    if (branch_stats.total_branches > 0) {
        double accuracy = (double)branch_stats.correct_predictions / branch_stats.total_branches * 100.0;
        oss << "Accuracy: " << std::fixed << std::setprecision(2) << accuracy << "%\n";
    }
    return oss.str();
}
