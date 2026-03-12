#include "prediction/dynamic/one_bit.hpp"

OneBit::OneBit() {}

bool OneBit::predict_branch(uint32_t program_counter)
{
    std::map<uint32_t, bool>::iterator map_iterator = branch_history_table.find(program_counter);
    if (map_iterator == branch_history_table.end())
        return false;
    return map_iterator->second;
}

void OneBit::update_predictor(uint32_t program_counter, bool branch_was_taken)
{
    branch_history_table[program_counter] = branch_was_taken;
}