#include "prediction/dynamic/global_history.hpp"

GlobalHistory::GlobalHistory()
{
    global_history_register = 0;
    history_length_mask = 0xFF;
}

bool GlobalHistory::predict_branch(uint32_t program_counter)
{
    int current_state = 1;
    std::map<uint32_t, int>::iterator pattern_iterator = pattern_history_table.find(global_history_register);
    if (pattern_iterator != pattern_history_table.end())
        current_state = pattern_iterator->second;
    return current_state >= 2;
}

void GlobalHistory::update_predictor(uint32_t program_counter, bool branch_was_taken)
{
    int current_state = 1;
    std::map<uint32_t, int>::iterator pattern_iterator = pattern_history_table.find(global_history_register);
    if (pattern_iterator != pattern_history_table.end())
        current_state = pattern_iterator->second;
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
    pattern_history_table[global_history_register] = current_state;
    uint32_t updated_history = (global_history_register << 1) | (branch_was_taken ? 1 : 0);
    global_history_register = updated_history & history_length_mask;
}