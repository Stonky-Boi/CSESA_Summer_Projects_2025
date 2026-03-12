#pragma once

#include "prediction/branch_predictor.hpp"
#include <map>
#include <cstdint>

class LocalHistory : public BranchPredictor
{
private:
    std::map<uint32_t, uint32_t> local_history_table;
    std::map<uint32_t, int> pattern_history_table;
    uint32_t history_length_mask;

public:
    LocalHistory();

    bool predict_branch(uint32_t program_counter) override;
    void update_predictor(uint32_t program_counter, bool branch_was_taken) override;
};