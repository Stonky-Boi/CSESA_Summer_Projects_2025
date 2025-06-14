#include "pipeline.hpp"
#include "instruction_decoder.hpp"
#include <sstream>
#include <iomanip>

Pipeline::Pipeline() {
    reset();
    stall_stages.resize(5, false);
    flush_stages.resize(5, false);
}

Pipeline::~Pipeline() {}

void Pipeline::reset() {
    // Initialize IF/ID pipeline register
    registers.if_id_pc = 0;
    registers.if_id_instruction = 0;
    registers.if_id_valid = false;
    
    // Initialize ID/EX pipeline register
    registers.id_ex_pc = 0;
    registers.id_ex_rs_data = 0;
    registers.id_ex_rt_data = 0;
    registers.id_ex_immediate = 0;
    registers.id_ex_rs = 0;
    registers.id_ex_rt = 0;
    registers.id_ex_rd = 0;
    registers.id_ex_opcode = 0;
    registers.id_ex_funct = 0;
    registers.id_ex_reg_write = false;
    registers.id_ex_mem_read = false;
    registers.id_ex_mem_write = false;
    registers.id_ex_branch = false;
    registers.id_ex_jump = false;
    registers.id_ex_valid = false;
    
    // Initialize EX/MEM pipeline register
    registers.ex_mem_pc = 0;
    registers.ex_mem_alu_result = 0;
    registers.ex_mem_rt_data = 0;
    registers.ex_mem_rd = 0;
    registers.ex_mem_reg_write = false;
    registers.ex_mem_mem_read = false;
    registers.ex_mem_mem_write = false;
    registers.ex_mem_zero = false;
    registers.ex_mem_valid = false;
    
    // Initialize MEM/WB pipeline register
    registers.mem_wb_alu_result = 0;
    registers.mem_wb_mem_data = 0;
    registers.mem_wb_rd = 0;
    registers.mem_wb_reg_write = false;
    registers.mem_wb_mem_to_reg = false;
    registers.mem_wb_valid = false;
    
    // Reset stall and flush flags
    std::fill(stall_stages.begin(), stall_stages.end(), false);
    std::fill(flush_stages.begin(), flush_stages.end(), false);
}

void Pipeline::advance() {
    // Check for stalls and flushes
    bool should_stall = false;
    bool should_flush = false;
    
    for (int i = 0; i < 5; i++) {
        if (stall_stages[i]) should_stall = true;
        if (flush_stages[i]) should_flush = true;
    }
    
    if (should_flush) {
        flush();
        return;
    }
    
    if (should_stall) {
        insertStall();
        return;
    }
    
    // Normal pipeline advancement
    // Move data from MEM/WB (no advancement needed for WB stage)
    
    // Move data from EX/MEM to MEM/WB
    registers.mem_wb_alu_result = registers.ex_mem_alu_result;
    registers.mem_wb_mem_data = 0; // Would be loaded from memory in real implementation
    registers.mem_wb_rd = registers.ex_mem_rd;
    registers.mem_wb_reg_write = registers.ex_mem_reg_write;
    registers.mem_wb_mem_to_reg = registers.ex_mem_mem_read;
    registers.mem_wb_valid = registers.ex_mem_valid;
    
    // Move data from ID/EX to EX/MEM
    registers.ex_mem_pc = registers.id_ex_pc;
    registers.ex_mem_alu_result = 0; // Would be computed by ALU
    registers.ex_mem_rt_data = registers.id_ex_rt_data;
    registers.ex_mem_rd = (registers.id_ex_opcode == 0) ? registers.id_ex_rd : registers.id_ex_rt;
    registers.ex_mem_reg_write = registers.id_ex_reg_write;
    registers.ex_mem_mem_read = registers.id_ex_mem_read;
    registers.ex_mem_mem_write = registers.id_ex_mem_write;
    registers.ex_mem_zero = false; // Would be set by ALU
    registers.ex_mem_valid = registers.id_ex_valid;
    
    // Move data from IF/ID to ID/EX
    registers.id_ex_pc = registers.if_id_pc;
    
    if (registers.if_id_valid) {
        uint32_t instruction = registers.if_id_instruction;
        uint8_t opcode = (instruction >> 26) & 0x3F;
        uint8_t rs = (instruction >> 21) & 0x1F;
        uint8_t rt = (instruction >> 16) & 0x1F;
        uint8_t rd = (instruction >> 11) & 0x1F;
        uint16_t immediate = instruction & 0xFFFF;
        uint8_t funct = instruction & 0x3F;
        
        registers.id_ex_rs = rs;
        registers.id_ex_rt = rt;
        registers.id_ex_rd = rd;
        registers.id_ex_immediate = immediate;
        registers.id_ex_opcode = opcode;
        registers.id_ex_funct = funct;
        
        // Set control signals based on instruction type
        registers.id_ex_reg_write = (opcode == 0 && funct != MIPS::FUNCT_JR) || 
                                   (opcode == MIPS::OPCODE_ADDI) || 
                                   (opcode == MIPS::OPCODE_LW);
        registers.id_ex_mem_read = (opcode == MIPS::OPCODE_LW);
        registers.id_ex_mem_write = (opcode == MIPS::OPCODE_SW);
        registers.id_ex_branch = (opcode == MIPS::OPCODE_BEQ) || (opcode == MIPS::OPCODE_BNE);
        registers.id_ex_jump = (opcode == MIPS::OPCODE_J) || (opcode == MIPS::OPCODE_JAL);
    }
    
    registers.id_ex_valid = registers.if_id_valid;
    
    // Clear IF/ID for next instruction fetch
    registers.if_id_valid = false;
}

bool Pipeline::detectDataHazard() const {
    if (!registers.id_ex_valid || !registers.ex_mem_valid) {
        return false;
    }
    
    // RAW hazard: instruction in ID stage reads register that instruction in EX stage will write
    if (registers.ex_mem_reg_write && registers.ex_mem_rd != 0) {
        if (registers.ex_mem_rd == registers.id_ex_rs || 
            registers.ex_mem_rd == registers.id_ex_rt) {
            return true;
        }
    }
    
    // Load-use hazard: load instruction in EX stage, use in ID stage
    if (registers.ex_mem_mem_read && registers.ex_mem_rd != 0) {
        if (registers.ex_mem_rd == registers.id_ex_rs || 
            registers.ex_mem_rd == registers.id_ex_rt) {
            return true;
        }
    }
    
    return false;
}

bool Pipeline::detectControlHazard() const {
    return registers.id_ex_valid && (registers.id_ex_branch || registers.id_ex_jump);
}

void Pipeline::insertStall() {
    // Insert bubble in ID/EX stage
    registers.id_ex_valid = false;
    registers.id_ex_reg_write = false;
    registers.id_ex_mem_read = false;
    registers.id_ex_mem_write = false;
    registers.id_ex_branch = false;
    registers.id_ex_jump = false;
    
    // Reset stall flags
    std::fill(stall_stages.begin(), stall_stages.end(), false);
}

void Pipeline::flush() {
    // Flush IF/ID and ID/EX stages (for branch misprediction)
    registers.if_id_valid = false;
    registers.id_ex_valid = false;
    registers.id_ex_reg_write = false;
    registers.id_ex_mem_read = false;
    registers.id_ex_mem_write = false;
    registers.id_ex_branch = false;
    registers.id_ex_jump = false;
    
    // Reset flush flags
    std::fill(flush_stages.begin(), flush_stages.end(), false);
}

Pipeline::PipelineRegister& Pipeline::getRegisters() {
    return registers;
}

const Pipeline::PipelineRegister& Pipeline::getRegisters() const {
    return registers;
}

std::string Pipeline::getStateString() const {
    std::ostringstream oss;
    
    oss << "Pipeline State:\n";
    oss << "================\n";
    
    // IF/ID Stage
    oss << "IF/ID: ";
    if (registers.if_id_valid) {
        oss << "PC=0x" << std::hex << std::setw(8) << std::setfill('0') << registers.if_id_pc
            << " Instr=0x" << std::hex << std::setw(8) << std::setfill('0') << registers.if_id_instruction;
    } else {
        oss << "NOP";
    }
    oss << "\n";
    
    // ID/EX Stage
    oss << "ID/EX: ";
    if (registers.id_ex_valid) {
        oss << "PC=0x" << std::hex << std::setw(8) << std::setfill('0') << registers.id_ex_pc
            << " Op=" << std::dec << (int)registers.id_ex_opcode
            << " Rs=$" << (int)registers.id_ex_rs
            << " Rt=$" << (int)registers.id_ex_rt
            << " Rd=$" << (int)registers.id_ex_rd;
    } else {
        oss << "NOP";
    }
    oss << "\n";
    
    // EX/MEM Stage
    oss << "EX/MEM: ";
    if (registers.ex_mem_valid) {
        oss << "PC=0x" << std::hex << std::setw(8) << std::setfill('0') << registers.ex_mem_pc
            << " ALU=0x" << std::hex << std::setw(8) << std::setfill('0') << registers.ex_mem_alu_result
            << " Rd=$" << std::dec << (int)registers.ex_mem_rd
            << " RegWr=" << (registers.ex_mem_reg_write ? "1" : "0")
            << " MemRd=" << (registers.ex_mem_mem_read ? "1" : "0")
            << " MemWr=" << (registers.ex_mem_mem_write ? "1" : "0");
    } else {
        oss << "NOP";
    }
    oss << "\n";
    
    // MEM/WB Stage
    oss << "MEM/WB: ";
    if (registers.mem_wb_valid) {
        oss << "ALU=0x" << std::hex << std::setw(8) << std::setfill('0') << registers.mem_wb_alu_result
            << " MemData=0x" << std::hex << std::setw(8) << std::setfill('0') << registers.mem_wb_mem_data
            << " Rd=$" << std::dec << (int)registers.mem_wb_rd
            << " RegWr=" << (registers.mem_wb_reg_write ? "1" : "0")
            << " MemToReg=" << (registers.mem_wb_mem_to_reg ? "1" : "0");
    } else {
        oss << "NOP";
    }
    oss << "\n";
    
    return oss.str();
}
