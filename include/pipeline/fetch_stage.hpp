#pragma once

#include "pipeline/pipeline_state.hpp"
#include "architecture/program_counter.hpp"
#include "instruction/instruction.hpp"
#include <map>
#include <cstdint>

class FetchStage
{
public:
    FetchStage();

    void execute(
        bool is_stalled,
        PipelineState &pipeline_state,
        ProgramCounter &program_counter,
        const std::map<uint32_t, Instruction> &instruction_memory) const;
};