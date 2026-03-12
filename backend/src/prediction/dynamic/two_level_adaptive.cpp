#include "prediction/dynamic/two_level_adaptive.hpp"

TwoLevelAdaptive::TwoLevelAdaptive()
{
    global_history_register = 0;
    history_length_mask = 0xFF;
}

bool TwoLevelAdaptive::predict_branch(uint32_t program_counter)
{
    // XOR the program counter with the global history to generate the GShare index
    uint32_t table_index = (program_counter ^ global_history_register) & history_length_mask;
    int current_state = 1;
    std::map<uint32_t, int>::iterator pattern_iterator = pattern_history_table.find(table_index);
    if (pattern_iterator != pattern_history_table.end())
        current_state = pattern_iterator->second;
    return current_state >= 2;
}

void TwoLevelAdaptive::update_predictor(uint32_t program_counter, bool branch_was_taken)
{
    uint32_t table_index = (program_counter ^ global_history_register) & history_length_mask;
    int current_state = 1;
    std::map<uint32_t, int>::iterator pattern_iterator = pattern_history_table.find(table_index);
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
    pattern_history_table[table_index] = current_state;
    uint32_t updated_history = (global_history_register << 1) | (branch_was_taken ? 1 : 0);
    global_history_register = updated_history & history_length_mask;
}