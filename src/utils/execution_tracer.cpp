#include "utils/execution_tracer.hpp"
#include <iostream>
#include <string>

ExecutionTracer::ExecutionTracer(bool enable_tracing)
{
    tracing_enabled = enable_tracing;
}

void ExecutionTracer::print_cycle_trace(uint64_t clock_cycle, const PipelineState &pipeline_state) const
{
    if (!tracing_enabled)
        return;

    std::string fetch_string = pipeline_state.read_if_id_latch().is_valid ? pipeline_state.read_if_id_latch().current_instruction.get_assembly_string() : "NOP";
    std::string decode_string = pipeline_state.read_id_ex_latch().is_valid ? pipeline_state.read_id_ex_latch().current_instruction.get_assembly_string() : "NOP";
    std::string execute_string = pipeline_state.read_ex_mem_latch().is_valid ? pipeline_state.read_ex_mem_latch().current_instruction.get_assembly_string() : "NOP";
    std::string memory_string = pipeline_state.read_mem_wb_latch().is_valid ? pipeline_state.read_mem_wb_latch().current_instruction.get_assembly_string() : "NOP";

    std::cout << "[Cycle " << clock_cycle << "] "
              << "IF: {" << fetch_string << "} | "
              << "ID: {" << decode_string << "} | "
              << "EX: {" << execute_string << "} | "
              << "MEM: {" << memory_string << "}"
              << std::endl;
}