#pragma once

#include "instruction/instruction.hpp"
#include <cstdint>

struct IfIdLatch
{
    Instruction current_instruction;
    uint32_t program_counter = 0;
    uint32_t incremented_program_counter = 0;
    bool was_predicted_taken = false;
    uint32_t predicted_target = 0;
    bool is_valid = false;
};

struct IdExLatch
{
    Instruction current_instruction;
    uint32_t program_counter = 0;
    uint32_t incremented_program_counter = 0;
    uint32_t register_read_data_one = 0;
    uint32_t register_read_data_two = 0;
    int32_t immediate_value = 0;
    int register_source = 0;
    int register_target = 0;
    int register_destination = 0;
    bool was_predicted_taken = false;
    uint32_t predicted_target = 0;
    bool is_valid = false;
};

struct ExMemLatch
{
    Instruction current_instruction;
    uint32_t program_counter = 0;
    uint32_t alu_result = 0;
    uint32_t store_data = 0;
    int write_register_index = 0;
    bool is_valid = false;
};

struct MemWbLatch
{
    Instruction current_instruction;
    uint32_t program_counter = 0;
    uint32_t alu_result = 0;
    uint32_t memory_read_data = 0;
    int write_register_index = 0;
    bool is_valid = false;
};

class PipelineState
{
private:
    IfIdLatch current_if_id;
    IfIdLatch next_if_id;

    IdExLatch current_id_ex;
    IdExLatch next_id_ex;

    ExMemLatch current_ex_mem;
    ExMemLatch next_ex_mem;

    MemWbLatch current_mem_wb;
    MemWbLatch next_mem_wb;

public:

    PipelineState();

    IfIdLatch read_if_id_latch() const;
    void write_if_id_latch(const IfIdLatch &new_latch);
    void flush_if_id_latch();
    IfIdLatch read_next_if_id_latch() const;
    void inject_prediction_to_if_id(bool taken, uint32_t target);

    IdExLatch read_id_ex_latch() const;
    void write_id_ex_latch(const IdExLatch &new_latch);
    void flush_id_ex_latch();

    ExMemLatch read_ex_mem_latch() const;
    void write_ex_mem_latch(const ExMemLatch &new_latch);
    void flush_ex_mem_latch();

    MemWbLatch read_mem_wb_latch() const;
    void write_mem_wb_latch(const MemWbLatch &new_latch);
    void flush_mem_wb_latch();

    void update_clock_cycle();
};