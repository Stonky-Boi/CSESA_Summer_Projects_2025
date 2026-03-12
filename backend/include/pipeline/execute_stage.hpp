#pragma once

#include "pipeline/pipeline_state.hpp"
#include "pipeline/hazard_unit.hpp"
#include "instruction/instruction.hpp"
#include <cstdint>

struct ExecuteResult
{
    bool is_branch_instruction = false;
    bool branch_taken = false;
    uint32_t branch_target = 0;
    bool was_predicted_taken = false;
    uint32_t predicted_target = 0;
};

class ExecuteStage
{
private:
    bool is_load_instruction(OperationType operation) const;

public:
    ExecuteStage();

    ExecuteResult execute(
        PipelineState &pipeline_state,
        const HazardUnit &hazard_unit) const;
};