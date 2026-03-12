#pragma once

#include <cstdint>

class BranchPredictor
{
public:
    virtual ~BranchPredictor() = default;

    virtual bool predict_branch(uint32_t program_counter) = 0;
    virtual void update_predictor(uint32_t program_counter, bool branch_was_taken) = 0;
};