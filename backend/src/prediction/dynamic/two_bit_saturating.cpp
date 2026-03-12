#include "prediction/dynamic/two_bit_saturating.hpp"

TwoBitSaturating::TwoBitSaturating(){}

bool TwoBitSaturating::predict_branch(uint32_t program_counter)
{
    std::map<uint32_t, int>::iterator map_iterator = branch_history_table.find(program_counter);
    // Default state is 1 (Weakly Not Taken)
    int current_state = 1;
    if (map_iterator != branch_history_table.end())
        current_state = map_iterator->second;
    return current_state >= 2;
}

void TwoBitSaturating::update_predictor(uint32_t program_counter, bool branch_was_taken)
{
    std::map<uint32_t, int>::iterator map_iterator = branch_history_table.find(program_counter);
    int current_state = 1;
    if (map_iterator != branch_history_table.end())
        current_state = map_iterator->second;
    if (branch_was_taken)
    {
        if (current_state < 3)
            current_state++;
    }
    else
    {
        if (current_state > 0)
            current_state--;
    }
    branch_history_table[program_counter] = current_state;
}