#include "pipeline/fetch_stage.hpp"

FetchStage::FetchStage() {}

void FetchStage::execute(
    bool is_stalled,
    PipelineState &pipeline_state,
    ProgramCounter &program_counter,
    const std::map<uint32_t, Instruction> &instruction_memory) const
{
    if (is_stalled)
    {
        // Freeze the stage: carry over the current latch state and do not increment the PC.
        pipeline_state.write_if_id_latch(pipeline_state.read_if_id_latch());
        return;
    }

    uint32_t current_address = program_counter.read_address();
    std::map<uint32_t, Instruction>::const_iterator map_iterator = instruction_memory.find(current_address);

    if (map_iterator == instruction_memory.end())
    {
        // End of program reached or jumping to unmapped memory. Insert a pipeline bubble.
        IfIdLatch bubble_latch;
        bubble_latch.is_valid = false;
        pipeline_state.write_if_id_latch(bubble_latch);
        return;
    }

    IfIdLatch new_latch;
    new_latch.current_instruction = map_iterator->second;
    new_latch.program_counter = current_address;
    new_latch.incremented_program_counter = current_address + 4;
    new_latch.is_valid = true;

    pipeline_state.write_if_id_latch(new_latch);
    program_counter.increment_address();
}