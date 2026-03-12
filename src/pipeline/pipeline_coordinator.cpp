#include "pipeline/pipeline_coordinator.hpp"

PipelineCoordinator::PipelineCoordinator(
    RegisterFile &registers,
    Memory &memory,
    ProgramCounter &pc,
    const std::map<uint32_t, Instruction> &program_instructions,
    BranchPredictor *predictor,
    StatisticsTracker &stats,
    ExecutionTracer &tracer) : register_file(registers),
                               system_memory(memory),
                               program_counter(pc),
                               instruction_memory(program_instructions),
                               branch_predictor(predictor),
                               statistics_tracker(stats),
                               execution_tracer(tracer) {}

bool PipelineCoordinator::check_if_pipeline_is_empty() const
{
    return !pipeline_state.read_if_id_latch().is_valid &&
           !pipeline_state.read_id_ex_latch().is_valid &&
           !pipeline_state.read_ex_mem_latch().is_valid &&
           !pipeline_state.read_mem_wb_latch().is_valid;
}

bool PipelineCoordinator::handle_system_calls_in_writeback()
{
    MemWbLatch current_latch = pipeline_state.read_mem_wb_latch();
    if (current_latch.is_valid && current_latch.current_instruction.get_operation() == OperationType::SYSCALL)
        return system_coprocessor.handle_syscall(register_file, system_memory);
    return true;
}

void PipelineCoordinator::execute_program()
{
    bool continue_execution = true;
    bool has_fetched_first_instruction = false;

    while (continue_execution)
    {
        statistics_tracker.increment_total_cycles();
        bool is_stalled = hazard_unit.requires_load_use_stall(
            pipeline_state.read_if_id_latch(),
            pipeline_state.read_id_ex_latch());
        if (is_stalled)
            statistics_tracker.increment_pipeline_stalls();

        // Reverse Execution
        writeback_stage.execute(pipeline_state, register_file);
        if (pipeline_state.read_mem_wb_latch().is_valid)
            statistics_tracker.increment_total_instructions();
        continue_execution = handle_system_calls_in_writeback();

        memory_stage.execute(pipeline_state, system_memory);
        ExecuteResult execute_result = execute_stage.execute(pipeline_state, hazard_unit);
        decode_stage.execute(is_stalled, pipeline_state, register_file);
        fetch_stage.execute(is_stalled, pipeline_state, program_counter, instruction_memory);

        // --- Branch Prediction (Fetch Phase) ---
        if (!is_stalled && pipeline_state.read_if_id_latch().is_valid)
        {
            uint32_t fetched_program_counter = pipeline_state.read_if_id_latch().program_counter;
            if (branch_predictor != nullptr)
            {
                bool is_predicted_taken = branch_predictor->predict_branch(fetched_program_counter);
                std::map<uint32_t, uint32_t>::iterator buffer_iterator = branch_target_buffer.find(fetched_program_counter);
                if (is_predicted_taken && buffer_iterator != branch_target_buffer.end())
                    program_counter.write_address(buffer_iterator->second);
            }
        }

        // --- Branch Resolution (Execute Phase) ---
        if (execute_result.is_branch_instruction)
        {
            uint32_t branch_program_counter = pipeline_state.read_ex_mem_latch().program_counter;
            statistics_tracker.increment_branches_executed();
            if (execute_result.branch_taken)
                statistics_tracker.increment_branches_taken();
            bool was_predicted_taken = false;
            uint32_t predicted_target = branch_program_counter + 4;
            if (branch_predictor != nullptr)
            {
                was_predicted_taken = branch_predictor->predict_branch(branch_program_counter);
                std::map<uint32_t, uint32_t>::iterator buffer_iterator = branch_target_buffer.find(branch_program_counter);
                if (was_predicted_taken && buffer_iterator != branch_target_buffer.end())
                    predicted_target = buffer_iterator->second;
                branch_predictor->update_predictor(branch_program_counter, execute_result.branch_taken);
                if (execute_result.branch_taken)
                    branch_target_buffer[branch_program_counter] = execute_result.branch_target;
            }
            uint32_t actual_target = execute_result.branch_taken ? execute_result.branch_target : branch_program_counter + 4;
            if (predicted_target != actual_target)
            {
                pipeline_state.flush_if_id_latch();
                pipeline_state.flush_id_ex_latch();
                program_counter.write_address(actual_target);
                statistics_tracker.increment_branches_mispredicted();
                statistics_tracker.increment_pipeline_flushes();
            }
        }

        pipeline_state.update_clock_cycle();
        execution_tracer.print_cycle_trace(statistics_tracker.get_total_cycles(), pipeline_state);

        if (pipeline_state.read_if_id_latch().is_valid)
            has_fetched_first_instruction = true;
        if (has_fetched_first_instruction && check_if_pipeline_is_empty())
            continue_execution = false;
    }
}