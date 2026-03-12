#pragma once

#include "pipeline/pipeline_state.hpp"
#include "architecture/register_file.hpp"
#include <cstdint>

class ExecutionTracer
{
private:
    bool tracing_enabled;
    bool output_json;

public:
    ExecutionTracer(bool enable_tracing, bool use_json);

    void print_cycle_trace(
        uint64_t clock_cycle,
        const PipelineState &pipeline_state,
        const RegisterFile &register_file) const;
};