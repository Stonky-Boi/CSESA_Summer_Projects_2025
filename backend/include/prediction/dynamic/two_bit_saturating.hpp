#pragma once

#include "prediction/branch_predictor.hpp"
#include <map>
#include <cstdint>

class TwoBitSaturating : public BranchPredictor
{
private:
    std::map<uint32_t, int> branch_history_table;

public:
    TwoBitSaturating();

    bool predict_branch(uint32_t program_counter) override;
    void update_predictor(uint32_t program_counter, bool branch_was_taken) override;
};