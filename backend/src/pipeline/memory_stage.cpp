#include "pipeline/memory_stage.hpp"

MemoryStage::MemoryStage() {}

void MemoryStage::execute(
    PipelineState &pipeline_state,
    Memory &system_memory) const
{
    ExMemLatch current_latch = pipeline_state.read_ex_mem_latch();
    MemWbLatch next_latch;

    if (!current_latch.is_valid)
    {
        next_latch.is_valid = false;
        pipeline_state.write_mem_wb_latch(next_latch);
        return;
    }

    OperationType operation = current_latch.current_instruction.get_operation();
    uint32_t memory_read_data = 0;
    uint32_t effective_address = current_latch.alu_result;

    if (operation == OperationType::LW || operation == OperationType::LL)
        memory_read_data = system_memory.read_word(effective_address);
    else if (operation == OperationType::LH)
    {
        uint16_t halfword_value = system_memory.read_halfword(effective_address);
        memory_read_data = static_cast<uint32_t>(static_cast<int16_t>(halfword_value));
    }
    else if (operation == OperationType::LHU)
        memory_read_data = static_cast<uint32_t>(system_memory.read_halfword(effective_address));
    else if (operation == OperationType::LB)
    {
        uint8_t byte_value = system_memory.read_byte(effective_address);
        memory_read_data = static_cast<uint32_t>(static_cast<int8_t>(byte_value));
    }
    else if (operation == OperationType::LBU)
        memory_read_data = static_cast<uint32_t>(system_memory.read_byte(effective_address));
    else if (operation == OperationType::SW || operation == OperationType::SC)
        system_memory.write_word(effective_address, current_latch.store_data);
    else if (operation == OperationType::SH)
        system_memory.write_halfword(effective_address, static_cast<uint16_t>(current_latch.store_data & 0xFFFF));
    else if (operation == OperationType::SB)
        system_memory.write_byte(effective_address, static_cast<uint8_t>(current_latch.store_data & 0xFF));

    next_latch.current_instruction = current_latch.current_instruction;
    next_latch.program_counter = current_latch.program_counter;
    next_latch.alu_result = current_latch.alu_result;
    next_latch.memory_read_data = memory_read_data;
    next_latch.write_register_index = current_latch.write_register_index;
    next_latch.is_valid = true;

    pipeline_state.write_mem_wb_latch(next_latch);
}