#pragma once

#include "pipeline/pipeline_state.hpp"
#include "architecture/register_file.hpp"

class DecodeStage
{
public:
    DecodeStage();

    void execute(
        bool is_stalled,
        PipelineState &pipeline_state,
        const RegisterFile &register_file) const;
};