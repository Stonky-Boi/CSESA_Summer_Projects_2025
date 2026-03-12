#pragma once

#include "pipeline/pipeline_state.hpp"
#include <cstdint>

class ExecutionTracer
{
private:
    bool tracing_enabled;

public:
    ExecutionTracer(bool enable_tracing);

    void print_cycle_trace(uint64_t clock_cycle, const PipelineState &pipeline_state) const;
};