#include "utils/execution_tracer.hpp"
#include <iostream>
#include <string>

ExecutionTracer::ExecutionTracer(bool enable_tracing, bool use_json)
{
    tracing_enabled = enable_tracing;
    output_json = use_json;
}

void ExecutionTracer::print_cycle_trace(
    uint64_t clock_cycle,
    const PipelineState &pipeline_state,
    const RegisterFile &register_file) const
{
    if (!tracing_enabled)
        return;

    std::string fetch_string = pipeline_state.read_if_id_latch().is_valid ? pipeline_state.read_if_id_latch().current_instruction.get_assembly_string() : "NOP";
    std::string decode_string = pipeline_state.read_id_ex_latch().is_valid ? pipeline_state.read_id_ex_latch().current_instruction.get_assembly_string() : "NOP";
    std::string execute_string = pipeline_state.read_ex_mem_latch().is_valid ? pipeline_state.read_ex_mem_latch().current_instruction.get_assembly_string() : "NOP";
    std::string memory_string = pipeline_state.read_mem_wb_latch().is_valid ? pipeline_state.read_mem_wb_latch().current_instruction.get_assembly_string() : "NOP";

    if (output_json)
    {
        std::cout << "{\"type\": \"cycle\", \"cycle\": " << clock_cycle << ", \"pipeline\": {";
        std::cout << "\"IF\": \"" << fetch_string << "\", ";
        std::cout << "\"ID\": \"" << decode_string << "\", ";
        std::cout << "\"EX\": \"" << execute_string << "\", ";
        std::cout << "\"MEM\": \"" << memory_string << "\"}, \"registers\": {";

        for (int index = 0; index < 32; ++index)
        {
            std::cout << "\"" << index << "\": " << register_file.read_register(index);
            if (index < 31)
                std::cout << ", ";
        }
        std::cout << "}}" << std::endl;
    }
    else
    {
        std::cout << "[Cycle " << clock_cycle << "] "
                  << "IF: {" << fetch_string << "} | "
                  << "ID: {" << decode_string << "} | "
                  << "EX: {" << execute_string << "} | "
                  << "MEM: {" << memory_string << "}"
                  << std::endl;
    }
}