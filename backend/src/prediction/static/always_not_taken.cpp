#include "prediction/static/always_not_taken.hpp"

AlwaysNotTaken::AlwaysNotTaken() {}

bool AlwaysNotTaken::predict_branch(uint32_t program_counter)
{
    return false;
}

// Static predictor does not update its state
void AlwaysNotTaken::update_predictor(uint32_t program_counter, bool branch_was_taken) {}