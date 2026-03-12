#pragma once

#include "pipeline/pipeline_state.hpp"
#include "architecture/memory.hpp"

class MemoryStage
{
public:
    MemoryStage();

    void execute(
        PipelineState &pipeline_state,
        Memory &system_memory) const;
};