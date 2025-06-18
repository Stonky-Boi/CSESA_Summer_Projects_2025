#pragma once
#include <vector>
#include <cstdint>
#include <string>

class Pipeline {
public:
    enum Stage {
        IF = 0,  // Instruction Fetch
        ID = 1,  // Instruction Decode
        EX = 2,  // Execute
        MEM = 3, // Memory Access
        WB = 4   // Write Back
    };
    
    struct PipelineRegister {
        // IF/ID
        uint32_t if_id_pc;
        uint32_t if_id_instruction;
        bool if_id_valid;
        
        // ID/EX
        uint32_t id_ex_pc;
        uint32_t id_ex_rs_data;
        uint32_t id_ex_rt_data;
        uint32_t id_ex_immediate;
        uint8_t id_ex_rs;
        uint8_t id_ex_rt;
        uint8_t id_ex_rd;
        uint8_t id_ex_opcode;
        uint8_t id_ex_funct;
        bool id_ex_reg_write;
        bool id_ex_mem_read;
        bool id_ex_mem_write;
        bool id_ex_branch;
        bool id_ex_jump;
        bool id_ex_valid;
        
        // EX/MEM
        uint32_t ex_mem_pc;
        uint32_t ex_mem_alu_result;
        uint32_t ex_mem_rt_data;
        uint8_t ex_mem_rd;
        bool ex_mem_reg_write;
        bool ex_mem_mem_read;
        bool ex_mem_mem_write;
        bool ex_mem_zero;
        bool ex_mem_valid;
        
        // MEM/WB
        uint32_t mem_wb_alu_result;
        uint32_t mem_wb_mem_data;
        uint8_t mem_wb_rd;
        bool mem_wb_reg_write;
        bool mem_wb_mem_to_reg;
        bool mem_wb_valid;
    };
    
    Pipeline();
    ~Pipeline();
    
    void reset();
    void advance();
    bool detectDataHazard() const;
    bool detectControlHazard() const;
    void insertStall();
    void flush();
    
    PipelineRegister& getRegisters();
    const PipelineRegister& getRegisters() const;
    std::string getStateString() const;
    
private:
    PipelineRegister registers;
    std::vector<bool> stall_stages;
    std::vector<bool> flush_stages;
};
