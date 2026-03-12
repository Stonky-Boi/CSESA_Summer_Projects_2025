#pragma once

#include <cstdint>

class StatisticsTracker
{
private:
    uint64_t total_cycles;
    uint64_t total_instructions;
    uint64_t pipeline_stalls;
    uint64_t pipeline_flushes;
    uint64_t branches_executed;
    uint64_t branches_taken;
    uint64_t branches_mispredicted;

public:
    StatisticsTracker();

    void increment_total_cycles();
    uint64_t get_total_cycles() const;

    void increment_total_instructions();
    void increment_pipeline_stalls();
    void increment_pipeline_flushes();
    void increment_branches_executed();
    void increment_branches_taken();
    void increment_branches_mispredicted();

    void print_statistics_report() const;
};