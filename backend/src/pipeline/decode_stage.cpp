#include "pipeline/decode_stage.hpp"

DecodeStage::DecodeStage() {}

void DecodeStage::execute(
    bool is_stalled,
    PipelineState &pipeline_state,
    const RegisterFile &register_file) const
{
    IfIdLatch current_latch = pipeline_state.read_if_id_latch();

    if (!current_latch.is_valid || is_stalled)
    {
        // If the incoming instruction is invalid, or if the hazard unit dictates a stall,
        // we must insert a bubble (NOP) into the execute stage.
        IdExLatch bubble_latch;
        bubble_latch.is_valid = false;
        pipeline_state.write_id_ex_latch(bubble_latch);
        return;
    }

    IdExLatch new_latch;
    new_latch.current_instruction = current_latch.current_instruction;
    new_latch.program_counter = current_latch.program_counter;
    new_latch.incremented_program_counter = current_latch.incremented_program_counter;

    new_latch.register_source = current_latch.current_instruction.get_register_source();
    new_latch.register_target = current_latch.current_instruction.get_register_target();
    new_latch.register_destination = current_latch.current_instruction.get_register_destination();
    new_latch.immediate_value = current_latch.current_instruction.get_immediate_value();

    // Read the values from the Register File simultaneously
    new_latch.register_read_data_one = register_file.read_register(new_latch.register_source);
    new_latch.register_read_data_two = register_file.read_register(new_latch.register_target);
    new_latch.was_predicted_taken = current_latch.was_predicted_taken;
    new_latch.predicted_target = current_latch.predicted_target;

    new_latch.is_valid = true;
    pipeline_state.write_id_ex_latch(new_latch);
}