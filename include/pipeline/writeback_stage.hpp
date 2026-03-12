#pragma once

#include "pipeline/pipeline_state.hpp"
#include "architecture/register_file.hpp"

class WritebackStage
{
private:
    bool is_load_instruction(OperationType operation) const;

public:
    WritebackStage();

    void execute(
        const PipelineState &pipeline_state,
        RegisterFile &register_file) const;
};