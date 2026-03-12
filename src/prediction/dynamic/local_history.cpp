#include "prediction/dynamic/local_history.hpp"

LocalHistory::LocalHistory()
{
    // Assume an 8-bit local history
    history_length_mask = 0xFF;
}

bool LocalHistory::predict_branch(uint32_t program_counter)
{
    uint32_t current_history = 0;

    std::map<uint32_t, uint32_t>::iterator history_iterator = local_history_table.find(program_counter);
    if (history_iterator != local_history_table.end())
        current_history = history_iterator->second;

    int current_state = 1;
    std::map<uint32_t, int>::iterator pattern_iterator = pattern_history_table.find(current_history);
    if (pattern_iterator != pattern_history_table.end())
        current_state = pattern_iterator->second;

    return current_state >= 2;
}

void LocalHistory::update_predictor(uint32_t program_counter, bool branch_was_taken)
{
    uint32_t current_history = 0;

    std::map<uint32_t, uint32_t>::iterator history_iterator = local_history_table.find(program_counter);
    if (history_iterator != local_history_table.end())
        current_history = history_iterator->second;

    int current_state = 1;
    std::map<uint32_t, int>::iterator pattern_iterator = pattern_history_table.find(current_history);
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

    pattern_history_table[current_history] = current_state;

    // Shift in the new outcome to the local history
    uint32_t updated_history = (current_history << 1) | (branch_was_taken ? 1 : 0);
    local_history_table[program_counter] = updated_history & history_length_mask;
}