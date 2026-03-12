#include "prediction/static/always_taken.hpp"

AlwaysTaken::AlwaysTaken() {}

bool AlwaysTaken::predict_branch(uint32_t program_counter)
{
    return true;
}

// Static predictor does not update its state
void AlwaysTaken::update_predictor(uint32_t program_counter, bool branch_was_taken) {}