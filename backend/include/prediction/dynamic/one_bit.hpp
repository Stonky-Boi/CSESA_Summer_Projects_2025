#pragma once

#include "prediction/branch_predictor.hpp"
#include <map>
#include <cstdint>

class OneBit : public BranchPredictor
{
private:
    std::map<uint32_t, bool> branch_history_table;

public:
    OneBit();

    bool predict_branch(uint32_t program_counter) override;
    void update_predictor(uint32_t program_counter, bool branch_was_taken) override;
};