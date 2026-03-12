#pragma once

#include "prediction/branch_predictor.hpp"

class AlwaysNotTaken : public BranchPredictor
{
public:
    AlwaysNotTaken();

    bool predict_branch(uint32_t program_counter) override;
    void update_predictor(uint32_t program_counter, bool branch_was_taken) override;
};