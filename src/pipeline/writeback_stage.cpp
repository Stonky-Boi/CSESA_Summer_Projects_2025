#include "pipeline/writeback_stage.hpp"

WritebackStage::WritebackStage() {}

bool WritebackStage::is_load_instruction(OperationType operation) const
{
    return operation == OperationType::LB || operation == OperationType::LBU ||
           operation == OperationType::LH || operation == OperationType::LHU ||
           operation == OperationType::LW || operation == OperationType::LL;
}

void WritebackStage::execute(
    const PipelineState &pipeline_state,
    RegisterFile &register_file) const
{
    MemWbLatch current_latch = pipeline_state.read_mem_wb_latch();
    if (!current_latch.is_valid)
        return;

    if (current_latch.write_register_index != 0)
    {
        OperationType operation = current_latch.current_instruction.get_operation();
        uint32_t data_to_write = 0;
        if (is_load_instruction(operation))
            data_to_write = current_latch.memory_read_data;
        else
            data_to_write = current_latch.alu_result;
        register_file.write_register(current_latch.write_register_index, data_to_write);
    }
}